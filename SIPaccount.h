Чтобы собрать проект с PJSIP в STM32CubeIDE в виде статической библиотеки (.a), нужно немного изменить подход. Вот пошаговый план для достижения этой цели:

1. Создайте новый проект для библиотеки
Создайте новый проект в STM32CubeIDE:
В меню выберите File -> New -> STM32 Project.
Настройте MCU и периферию с помощью CubeMX.
Выберите "Empty" в разделе выбора шаблона для минимального кода.
Настройте проект как библиотеку:
Откройте Project -> Properties -> C/C++ Build -> Settings.
В разделе MCU GCC Linker -> General выберите Do not use the standard startup files (т. е., не добавляйте точку входа, так как это библиотека).
В разделе MCU GCC Archiver -> Archive Format установите формат lib<name>.a.
2. Добавьте PJSIP в проект
2.1. Подключение библиотек PJSIP

Соберите PJSIP вручную для ARM:
Соберите PJSIP с помощью arm-none-eabi-gcc (см. предыдущие инструкции).
Получите статические библиотеки (libpjsip.a, libpjlib.a, и т.д.).
Добавьте PJSIP в проект:
Переместите собранные библиотеки и заголовки PJSIP в папку проекта STM32CubeIDE. Например:
Заголовки: YourProject/include/pjsip.
Библиотеки: YourProject/libs.
Подключите пути к заголовкам и библиотекам:
Project -> Properties -> C/C++ Build -> Settings -> MCU GCC Compiler -> Includes:
${workspace_loc:/YourProject/include/pjsip}
Project -> Properties -> C/C++ Build -> Settings -> MCU GCC Linker -> Libraries:
В поле Libraries добавьте pjsip, pjlib, pjmedia, и т.д.
В поле Library search path укажите путь к YourProject/libs.
2.2. Добавление конфигурации для STM32

Создайте файл config_site.h для минимальной конфигурации PJSIP:

#define PJ_CONFIG_MINIMAL_SIZE 1
#include <pj/config_site_sample.h>

#define PJ_HAS_STDIO 0
#define PJ_LOG_MAX_LEVEL 3
#define PJ_ENABLE_EXTRA_CHECK 0
#define PJSIP_MAX_URL_SIZE 512

// Для работы с LwIP
#define PJSIP_DONT_USE_SYS_SELECT 1
#define PJ_HAS_HIGH_RES_TIMER 0
Добавьте путь к config_site.h в список заголовков.

3. Адаптируйте проект
Удалите точку входа main():
Поскольку это библиотека, точка входа в код не нужна. Убедитесь, что файл main.c не используется.
Создайте свои модули:
В файлах библиотеки (.c или .cpp) создайте функции, которые будут использоваться конечными пользователями библиотеки. Например:
#include <pjsip.h>

void init_pjsip() {
    // Пример: Инициализация PJSIP
    pj_status_t status = pj_init();
    if (status != PJ_SUCCESS) {
        // Обработка ошибки
    }
}
Подключите необходимые библиотеки STM32:
Если библиотека будет использовать HAL, LwIP или FreeRTOS, убедитесь, что все пути к заголовкам и конфигурации добавлены.
4. Настройка линковки
Отключите точку входа, если она включена:
В MCU GCC Linker -> General проверьте, что отсутствует точка входа (_start или main).
Убедитесь, что статические библиотеки PJSIP включены в сборку:
Проверяйте, чтобы в итоговую библиотеку .a попадали только модули, которые требуются пользователю.
5. Сборка библиотеки
Соберите проект:
Нажмите Project -> Build Project.
В папке Debug или Release должен появиться файл .a. Например:
libYourProject.a
Проверьте состав библиотеки:
Используйте команду arm-none-eabi-ar для проверки:
arm-none-eabi-ar -t Debug/libYourProject.a
6. Использование библиотеки
Чтобы использовать собранную библиотеку в других проектах:

Подключите файл libYourProject.a в новый проект.
Убедитесь, что пути к заголовкам и зависимостям (например, HAL, LwIP, PJSIP) указаны корректно.
В проекте вызывайте функции, реализованные в библиотеке.
