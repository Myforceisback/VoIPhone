#include <pjmedia/port.h>
#include <pjlib.h>
#include <stdio.h>

static FILE *text_file = NULL;
static char file_buffer[SAMPLES_PER_FRAME]; // Буфер для хранения данных из файла

// Функция для открытия файла
pj_status_t open_text_file(const char *file_path)
{
    text_file = fopen(file_path, "r");
    if (!text_file) {
        PJ_LOG(1, (THIS_FILE, "Не удалось открыть файл: %s", file_path));
        return PJ_EIO;
    }
    return PJ_SUCCESS;
}

// Функция для закрытия файла
void close_text_file()
{
    if (text_file) {
        fclose(text_file);
        text_file = NULL;
    }
}

// Функция для чтения данных из файла в фрейм
static pj_status_t custom_get_frame(pjmedia_port *port, pjmedia_frame *frame)
{
    if (!text_file) {
        PJ_LOG(1, (THIS_FILE, "Файл не открыт для чтения"));
        return PJ_EIO;
    }

    // Читаем данные из файла в буфер
    size_t bytes_read = fread(file_buffer, 1, sizeof(file_buffer), text_file);

    // Проверяем, что мы достигли конца файла, и если так, перезапускаем чтение с начала
    if (bytes_read < sizeof(file_buffer)) {
        rewind(text_file);  // Перемещаем указатель на начало файла
        fread(file_buffer + bytes_read, 1, sizeof(file_buffer) - bytes_read, text_file); // дочитываем недостающие данные
    }

    // Копируем данные из буфера в аудиофрейм
    pj_memcpy(frame->buf, file_buffer, sizeof(file_buffer));
    frame->size = sizeof(file_buffer);  // Устанавливаем размер фрейма
    frame->type = PJMEDIA_FRAME_TYPE_AUDIO;

    return PJ_SUCCESS;
}
pj_status_t open_text_file(const char *file_path)
{
    text_file = fopen(file_path, "r");
    if (!text_file) {
        PJ_LOG(1, (THIS_FILE, "Не удалось открыть файл: %s", file_path));
        return PJ_EIO;
    }
    return PJ_SUCCESS;
}
int main()
{
    pjsua_config cfg;
    pjsua_logging_config log_cfg;
    pjsua_media_config media_cfg;
    pj_pool_t *pool;

    // Инициализация PJSUA
    pjsua_create();

    // Конфигурация PJSUA
    pjsua_config_default(&cfg);
    pjsua_logging_config_default(&log_cfg);
    pjsua_media_config_default(&media_cfg);

    // Инициализация PJSUA
    pjsua_init(&cfg, &log_cfg, &media_cfg);

    // Открытие RTP-транспорта (для передачи медиа)
    pjsua_transport_config transport_cfg;
    pjsua_transport_config_default(&transport_cfg);
    transport_cfg.port = 5060;
    pjsua_transport_create(PJSIP_TRANSPORT_UDP, &transport_cfg, NULL);

    // Запуск PJSUA
    pjsua_start();

    // Создаем memory pool для пользовательского медиа порта
    pool = pjsua_pool_create("media_pool", 512, 512);

    // Создаем пользовательский медиа-порт
    create_custom_media_port(pool);

    // Подключаем его к конференц мосту
    connect_custom_port_to_conference();

    // Ожидаем событий (например, звонков или подключения к конференции)
    pjsua_handle_events(NULL);

    // Остановка PJSUA
    pjsua_destroy();
    
    return 0;
}
int main()
{
    // Открытие текстового файла перед началом передачи данных
    pj_status_t status = open_text_file("path/to/textfile.txt");
    if (status != PJ_SUCCESS) {
        PJ_LOG(1, (THIS_FILE, "Ошибка открытия текстового файла"));
        return -1;
    }

    // Инициализация PJSUA и настройка порта конференции
    ...

    // После окончания передачи данных, закрываем файл
    close_text_file();

    // Остановка PJSUA
    pjsua_destroy();
    
    return 0;
}
