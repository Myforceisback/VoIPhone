Для сборки PJSIP в виде библиотеки для использования на платформе FreeRTOS необходимо выполнить несколько шагов: адаптировать PJSIP под архитектуру, интегрировать с FreeRTOS и собрать проект. Вот подробная инструкция:

Шаг 1: Подготовка исходников PJSIP

Клонируйте PJSIP:
Скачайте исходный код PJSIP из официального репозитория:
git clone https://github.com/pjsip/pjproject.git
cd pjproject
Создайте файл config_site.h:
В корневом каталоге библиотеки создайте файл config_site.h для настройки сборки.
Минимальная конфигурация для FreeRTOS:

#define PJ_CONFIG_MINIMAL_SIZE 1
#include <pj/config_site_sample.h>

// Уровень логирования (уменьшить для экономии ресурсов)
#define PJ_LOG_MAX_LEVEL 3

// Отключение POSIX функций
#define PJ_HAS_STDIO 0
#define PJ_HAS_ERRNO_VAR 0

// Замена select() и других POSIX функций
#define PJSIP_DONT_USE_SYS_SELECT 1
#define PJ_IOQUEUE_MAX_HANDLES 16

// FreeRTOS и стек TCP/IP
#define PJ_HAS_THREADS 1
#define PJ_OS_HAS_CHECK_STACK 0
#define PJ_TIMER_USE_RBTREE 1

// Настройка LwIP (если используется)
#define PJ_HAS_LWIP 1
#define LWIP_COMPAT_SOCKETS 1
Удалите ненужные модули PJSIP:
Если вам не требуется поддержка видео или сложных медиа-функций, удалите модули pjmedia-videodev и связанные исходники из проекта.
Шаг 2: Адаптация PJSIP для FreeRTOS

Реализуйте адаптер FreeRTOS для потоков:
PJSIP ожидает POSIX API для управления потоками, но FreeRTOS использует свои примитивы. Вам нужно реализовать функции-обертки, которые будут использовать FreeRTOS.
Пример реализации:

#include "FreeRTOS.h"
#include "task.h"

void pj_thread_sleep(unsigned msec) {
    vTaskDelay(pdMS_TO_TICKS(msec));
}
Настройка работы с TCP/IP стеком:
Если вы используете LwIP, подключите его API для работы с сокетами. Например, вместо POSIX вызовов socket(), select(), используйте вызовы LwIP:
#define pj_sock_socket lwip_socket
#define pj_sock_bind lwip_bind
#define pj_sock_connect lwip_connect
Настройка таймеров:
Реализуйте pj_gettickcount() для использования системного времени FreeRTOS:
pj_uint32_t pj_gettickcount(void) {
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}
Шаг 3: Настройка сборки проекта

Создайте проект для библиотеки в STM32CubeIDE:
В STM32CubeIDE выберите File -> New -> STM32 Project.
Настройте процессор (MCU), подключите FreeRTOS через CubeMX.
Добавьте исходники PJSIP:
Скопируйте следующие папки из PJSIP в ваш проект:
pjlib
pjlib-util
pjsip
pjmedia (если используется).
Убедитесь, что вы скопировали только те исходники, которые вам действительно нужны.
Подключите пути к заголовкам:
В Project -> Properties -> C/C++ Build -> Settings -> MCU GCC Compiler -> Includes добавьте:
${workspace_loc:/YourProject/pjlib/include}
${workspace_loc:/YourProject/pjsip/include}
${workspace_loc:/YourProject/pjlib-util/include}
Укажите пути к исходникам для сборки:
В Project -> Properties -> C/C++ General -> Paths and Symbols -> Source Location укажите пути к папкам pjlib, pjlib-util, pjsip.
Шаг 4: Настройка компиляции

Добавьте параметры компиляции:
В MCU GCC Compiler -> Miscellaneous добавьте:
-DPJ_HAS_LWIP=1
-DLWIP_COMPAT_SOCKETS=1
-Dpj_gettickcount=xTaskGetTickCount
Оптимизация памяти:
Включите оптимизацию кода для микроконтроллеров:
Optimization Level: -O2
MCU GCC Linker -> Miscellaneous:
-Wl,--gc-sections
Отключите точку входа main():
Если это библиотека, то в MCU GCC Linker -> General выберите Do not use the standard startup files.
Шаг 5: Сборка как библиотеки

Соберите проект:
После успешной сборки STM32CubeIDE сгенерирует объектные файлы .o.
Создайте статическую библиотеку .a:
Используйте arm-none-eabi-ar для упаковки объектных файлов в библиотеку:
arm-none-eabi-ar rcs libpjsip.a *.o
Подключите библиотеку к другим проектам:
Перенесите libpjsip.a в конечный проект, работающий с FreeRTOS.
Подключите заголовки PJSIP.
Шаг 6: Тестирование

Создайте тестовый проект:
Создайте приложение, которое инициализирует PJSIP:
#include <pjsip.h>
void test_pjsip() {
    pj_status_t status = pj_init();
    if (status != PJ_SUCCESS) {
        // Обработка ошибок
    }
}
Проверьте работу:
Запустите тестовое приложение на STM32 с FreeRTOS. Убедитесь, что PJSIP успешно инициализируется и выполняет базовые операции.
Если вы столкнетесь с проблемами на этапе сборки, адаптации PJSIP или интеграции с FreeRTOS, напишите, и я помогу!