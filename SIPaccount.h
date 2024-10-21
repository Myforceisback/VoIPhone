EpConfig ep_cfg;
ep_cfg.uaConfig.threadCnt = 0; // Отключение потоков для аудио устройств
ep_cfg.medConfig.hasIoqueue = PJ_FALSE; // Отключение использования аудио устройств
ep.libInit(ep_cfg);

class CustomMediaPort : public AudioMedia {
private:
    vector<uint8_t> binaryData;  // Данные для передачи
    unsigned frameSize;          // Размер одного фрейма

public:
    CustomMediaPort(const vector<uint8_t>& data, unsigned frame_size)
        : binaryData(data), frameSize(frame_size) {}

    // Имитация получения данных
    virtual void getFrame(AudioFrame &frame) override {
        // Генерируем "аудио" данные
        frame.buf = (void*)binaryData.data();
        frame.size = frameSize;
        frame.type = PJMEDIA_FRAME_TYPE_AUDIO;  // Тип - аудио фрейм

        cout << "Sending fake audio data of size: " << frame.size << endl;
    }

    // Обработка полученных данных
    virtual void putFrame(const AudioFrame &frame) override {
        // Обрабатываем полученные данные
        cout << "Received frame of size: " << frame.size << endl;
    }
};

Call *call = new Call(ep);
CallOpParam prm;
prm.statusCode = PJSIP_SC_OK;

call->makeCall("sip:destination@domain.com", prm);

// Ожидаем соединения
pj_thread_sleep(1000);

CallInfo ci = call->getInfo();
AudioMedia *media = static_cast<AudioMedia*>(call->getMedia(ci.media[0].index));

// Создаем кастомный медиа-порт для передачи фейковых аудиофреймов
vector<uint8_t> binaryData(160, 0x55);  // Пример фейковых данных
CustomMediaPort *mediaPort = new CustomMediaPort(binaryData, 160);

// Подключаем кастомный порт к звонку
mediaPort->startTransmit(*media);

pj_size_t pjmedia_codec_g711_encode(pjmedia_codec *codec,
                                    const pjmedia_frame *input,
                                    pjmedia_frame *output)
{
    // Ваша логика по замене аудио на бинарные данные
    memcpy(output->buf, input->buf, input->size); // Передача данных как есть
    output->size = input->size;
    return PJ_SUCCESS;
}

#include <stdio.h>

// Глобальная переменная для файла, чтобы открыть его один раз
static FILE *binary_file = NULL;

// Функция кодирования с чтением из файла
pj_size_t pjmedia_codec_g711_encode(pjmedia_codec *codec,
                                    const pjmedia_frame *input,
                                    pjmedia_frame *output)
{
    // Размер буфера для данных, которые будут передаваться
    size_t bytes_to_read = output->size;
    
    // Если файл еще не открыт, открываем его
    if (binary_file == NULL) {
        binary_file = fopen("binary_data.bin", "rb");
        if (binary_file == NULL) {
            PJ_LOG(3, ("G711", "Failed to open binary file for reading"));
            return PJ_EFILE;
        }
    }

    // Читаем данные из файла в буфер output
    size_t bytes_read = fread(output->buf, 1, bytes_to_read, binary_file);

    // Если данные закончились, закрываем файл и завершаем
    if (bytes_read < bytes_to_read) {
        PJ_LOG(3, ("G711", "Reached end of file, closing binary file."));
        fclose(binary_file);
        binary_file = NULL;
        memset(output->buf + bytes_read, 0, bytes_to_read - bytes_read); // Заполнение остатка нулями
    }

    output->size = bytes_read;  // Устанавливаем фактический размер данных
    output->type = PJMEDIA_FRAME_TYPE_AUDIO;

    return PJ_SUCCESS;
}
