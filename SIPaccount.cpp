#include <pjsua-lib/pjsua.h>
#include <pjmedia/port.h>
#include <stdio.h>

#define THIS_FILE "custom_media_file_send.c"
#define PORT "custom-media-port"

static pjsua_conf_port_id conf_port_id;
static pjmedia_port *custom_media_port;
static FILE *file_to_send = NULL;

#define FRAME_SIZE 320 // размер фрейма в байтах

// Открытие файла для передачи
pj_status_t open_file(const char *file_path)
{
    file_to_send = fopen(file_path, "rb");
    if (!file_to_send) {
        PJ_LOG(1, (THIS_FILE, "Не удалось открыть файл: %s", file_path));
        return PJ_EIO;
    }
    return PJ_SUCCESS;
}

// Закрытие файла после передачи
void close_file()
{
    if (file_to_send) {
        fclose(file_to_send);
        file_to_send = NULL;
    }
}
// Функция для передачи кадров через медиапорт
static pj_status_t custom_get_frame(pjmedia_port *port, pjmedia_frame *frame)
{
    if (!file_to_send) {
        PJ_LOG(1, (THIS_FILE, "Файл не открыт для передачи"));
        return PJ_EIO;
    }

    // Читаем данные из файла и записываем в фрейм
    size_t bytes_read = fread(frame->buf, 1, FRAME_SIZE, file_to_send);

    if (bytes_read < FRAME_SIZE) {
        // Если достигли конца файла, закрываем и останавливаем передачу
        PJ_LOG(3, (THIS_FILE, "Передача файла завершена"));
        close_file();
        frame->type = PJMEDIA_FRAME_TYPE_NONE; // Устанавливаем тип как NONE, чтобы завершить
    } else {
        frame->size = FRAME_SIZE;  // Размер данных
        frame->type = PJMEDIA_FRAME_TYPE_AUDIO; // Используем тип аудиофрейма
    }

    return PJ_SUCCESS;
}

// Создание медиапорта
pj_status_t create_custom_media_port(pj_pool_t *pool)
{
    pj_status_t status;

    status = pjmedia_port_create(pool, PORT, 8000, 1, FRAME_SIZE, 16, 0, &custom_media_port);
    if (status != PJ_SUCCESS) {
        PJ_LOG(1, (THIS_FILE, "Ошибка создания медиа порта"));
        return status;
    }

    // Присваиваем функцию для получения фреймов
    custom_media_port->get_frame = &custom_get_frame;

    return PJ_SUCCESS;
}
void connect_custom_port_to_conference()
{
    pj_status_t status = pjsua_conf_add_port(pjsua_get_conf_bridge(), custom_media_port, &conf_port_id);
    if (status != PJ_SUCCESS) {
        PJ_LOG(1, (THIS_FILE, "Ошибка добавления медиа порта в конференцию"));
        return;
    }

    // Подключаем порт к конференц мосту
    pjsua_conf_connect(conf_port_id, 0);  // Передаем данные в общий конференц-мост
}
int main(int argc, char *argv[])
{
    pjsua_config cfg;
    pjsua_logging_config log_cfg;
    pjsua_media_config media_cfg;
    pj_pool_t *pool;

    if (argc < 2) {
        printf("Использование: %s <путь к файлу>\n", argv[0]);
        return 1;
    }

    // Инициализация PJSUA
    pjsua_create();

    // Конфигурация PJSUA
    pjsua_config_default(&cfg);
    pjsua_logging_config_default(&log_cfg);
    pjsua_media_config_default(&media_cfg);

    // Инициализация PJSUA
    pjsua_init(&cfg, &log_cfg, &media_cfg);

    // Открытие RTP-транспорта
    pjsua_transport_config transport_cfg;
    pjsua_transport_config_default(&transport_cfg);
    transport_cfg.port = 5060;
    pjsua_transport_create(PJSIP_TRANSPORT_UDP, &transport_cfg, NULL);

    // Запуск PJSUA
    pjsua_start();

    // Открываем файл для передачи
    if (open_file(argv[1]) != PJ_SUCCESS) {
        return 1;
    }

    // Создаем memory pool для медиапорта
    pool = pjsua_pool_create("media_pool", 512, 512);

    // Создаем и подключаем медиапорт к конференц-мосту
    create_custom_media_port(pool);
    connect_custom_port_to_conference();

    // Ожидаем событий
    pjsua_handle_events(NULL);

    // Остановка PJSUA
    pjsua_destroy();

    return 0;
}
