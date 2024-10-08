#include <pjsua-lib/pjsua.h>
#include <pjmedia/conference.h>
#include <pjlib.h>
#include <stdio.h>

#define THIS_FILE "app.c"
#define SIP_DOMAIN "sip:your_sip_server"
#define SIP_USER "your_sip_user"
#define SIP_PASSWD "your_sip_password"
#define BINARY_FILE_PATH "your_binary_file.bin"

// Глобальные переменные для хранения состояния звонка и файла
static pjsua_call_id current_call = PJSUA_INVALID_ID;
static FILE *binary_file = NULL;
static size_t binary_file_size;
static char *binary_buffer = NULL;
static size_t bytes_read = 0;
static int custom_conf_slot = -1;

// Функция обработки ошибок
static void error_exit(const char *title, pj_status_t status) {
    pjsua_perror(THIS_FILE, title, status);
    if (binary_file) fclose(binary_file);
    exit(1);
}

// Пользовательская функция для передачи бинарного файла по фреймам
static pj_status_t custom_conf_port_put_frame(pjmedia_port *port, const pjmedia_frame *frame) {
    PJ_UNUSED_ARG(port);

    if (binary_file && binary_buffer) {
        // Читаем данные из файла, отправляем фреймами
        size_t bytes_to_read = frame->size;
        if (bytes_read < binary_file_size) {
            size_t actual_bytes_read = fread(binary_buffer, 1, bytes_to_read, binary_file);
            if (actual_bytes_read > 0) {
                pj_memcpy(frame->buf, binary_buffer, actual_bytes_read);
                frame->size = actual_bytes_read;
                frame->bit_info = 0;
                bytes_read += actual_bytes_read;
                PJ_LOG(3, (THIS_FILE, "Transmitting binary data: %zu bytes", actual_bytes_read));
            } else {
                PJ_LOG(3, (THIS_FILE, "End of binary file reached"));
                frame->size = 0; // Отправка закончена
            }
        } else {
            PJ_LOG(3, (THIS_FILE, "All data has been transmitted"));
            frame->size = 0;
        }
    }
    return PJ_SUCCESS;
}

// Обработка входящего звонка
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
    pjsua_call_info call_info;

    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(rdata);

    pjsua_call_get_info(call_id, &call_info);
    PJ_LOG(3, (THIS_FILE, "Incoming call from %.*s", (int)call_info.remote_info.slen, call_info.remote_info.ptr));

    // Принять входящий звонок автоматически
    pjsua_call_answer(call_id, 200, NULL, NULL);
    current_call = call_id;
}

// Обработка изменения состояния медиа-потока
static void on_call_media_state(pjsua_call_id call_id) {
    pjsua_call_info call_info;

    pjsua_call_get_info(call_id, &call_info);

    if (call_info.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
        // Подключение кастомного порта к конференции
        if (custom_conf_slot != -1) {
            pjsua_conf_connect(custom_conf_slot, call_info.conf_slot);
            pjsua_conf_connect(call_info.conf_slot, custom_conf_slot);
            PJ_LOG(3, (THIS_FILE, "Custom media port connected to the call"));
        }
    }
}

// Функция для совершения исходящего звонка
static void make_call(const char *dest_uri) {
    pj_str_t uri = pj_str((char*)dest_uri);
    pj_status_t status = pjsua_call_make_call(pjsua_acc_get_default(), &uri, 0, NULL, NULL, NULL);
    if (status != PJ_SUCCESS) {
        error_exit("Error making call", status);
    }
}

int main() {
    pjsua_acc_id acc_id;
    pj_status_t status;

    // Открываем бинарный файл для передачи данных
    binary_file = fopen(BINARY_FILE_PATH, "rb");
    if (!binary_file) {
        fprintf(stderr, "Error opening binary file: %s\n", BINARY_FILE_PATH);
        return 1;
    }

    // Получаем размер файла
    fseek(binary_file, 0, SEEK_END);
    binary_file_size = ftell(binary_file);
    fseek(binary_file, 0, SEEK_SET);

    // Создаем буфер для чтения данных из файла
    binary_buffer = (char*)malloc(160); // Размер буфера под один RTP фрейм (160 байт для 8000Hz 20мс)
    if (!binary_buffer) {
        fprintf(stderr, "Error allocating memory for binary file buffer\n");
        fclose(binary_file);
        return 1;
    }

    // Инициализация PJSUA
    status = pjsua_create();
    if (status != PJ_SUCCESS) error_exit("Error in pjsua_create()", status);

    // Настройки PJSUA
    pjsua_config cfg;
    pjsua_config_default(&cfg);
    cfg.cb.on_incoming_call = &on_incoming_call;
    cfg.cb.on_call_media_state = &on_call_media_state;
    status = pjsua_init(&cfg, NULL, NULL);
    if (status != PJ_SUCCESS) error_exit("Error in pjsua_init()", status);

    // Настройка транспортного уровня
    pjsua_transport_config transport_cfg;
    pjsua_transport_config_default(&transport_cfg);
    transport_cfg.port = 5060;
    status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &transport_cfg, NULL);
    if (status != PJ_SUCCESS) error_exit("Error creating transport", status);

    // Старт PJSUA
    status = pjsua_start();
    if (status != PJ_SUCCESS) error_exit("Error starting pjsua", status);

    // Настройки аккаунта
    pjsua_acc_config acc_cfg;
    pjsua_acc_config_default(&acc_cfg);
    acc_cfg.id = pj_str("<" SIP_DOMAIN ">");
    acc_cfg.reg_uri = pj_str("sip:" SIP_DOMAIN);
    acc_cfg.cred_count = 1;
    acc_cfg.cred_info[0].realm = pj_str("*");
    acc_cfg.cred_info[0].scheme = pj_str("digest");
    acc_cfg.cred_info[0].username = pj_str(SIP_USER);
    acc_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    acc_cfg.cred_info[0].data = pj_str(SIP_PASSWD);

    status = pjsua_acc_add(&acc_cfg, PJ_TRUE, &acc_id);
    if (status != PJ_SUCCESS) error_exit("Error adding account", status);

    // Создание пользовательского порта конференц-моста
    pjmedia_port *conf_port;
    pjmedia_conf_port_info info;
    pjmedia_conf_port_info_default(&info, 16000, 1, PJMEDIA_CONF_USE_DEJITTER);

    status = pjmedia_conf_add_port(pjsua_get_conf(), NULL, PJMEDIA_PORT_CUSTOM, &info, &conf_port);
    if (status != PJ_SUCCESS) error_exit("Error adding custom conf port", status);

    // Устанавливаем пользовательскую функцию для обработки фреймов
    conf_port->put_frame = &custom_conf_port_put_frame;

    // Добавление пользовательского порта в конференцию
    status = pjsua_conf_add_port(conf_port, &custom_conf_slot);
    if (status != PJ_SUCCESS) error_exit("Error adding port to conference", status);

    // Совершаем исходящий звонок
    make_call("sip:destination_user@sip_server");

    // Ожидание завершения
    pj_thread_sleep(60000);

    // Освобождение ресурсов
    fclose(binary_file);
    free(binary_buffer);
    pjsua_destroy();

    return 0;
}
