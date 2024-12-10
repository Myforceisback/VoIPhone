Для замены BSD-сокетов на стек LwIP в библиотеке PJSIP, необходимо выполнить адаптацию кода, поскольку PJSIP по умолчанию использует POSIX-совместимый интерфейс сокетов, а LwIP предоставляет собственный интерфейс с ограниченной поддержкой POSIX. Вот пошаговое руководство:

Шаг 1: Настройка проекта с LwIP

Добавьте LwIP в проект:
Если вы используете STM32CubeIDE, включите LwIP через STM32CubeMX.
Настройте LwIP в зависимости от вашей среды:
Включите использование API сокетов:
В файле lwipopts.h добавьте или проверьте:
#define LWIP_COMPAT_SOCKETS 1
#define LWIP_SOCKET 1
#define LWIP_DNS 1
Настройте стек в соответствии с требованиями вашего проекта (например, MTU, буферы, очередь пакетов).
Инициализируйте LwIP в вашем проекте:
Включите сетевой интерфейс, используя функции netif_add, netif_set_up и настройте IP-адресацию (DHCP или статический IP).
Пример инициализации:
struct netif gnetif;

void lwip_user_init(void) {
    ip4_addr_t ipaddr, netmask, gw;
    IP4_ADDR(&ipaddr, 192, 168, 1, 100);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 1, 1);

    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
    netif_set_default(&gnetif);
    netif_set_up(&gnetif);
}
Шаг 2: Изменение конфигурации PJSIP

Отключите POSIX-ориентированные механизмы: Создайте или отредактируйте файл config_site.h (обычно находится в pjlib/include/pj):
#define PJ_HAS_BSD_SOCK 0
#define PJ_HAS_LWIP     1

// Отключаем функции, которые могут зависеть от POSIX
#define PJ_SOCK_HAS_GETADDRINFO 0
#define PJ_HAS_SO_ERROR 0
#define PJ_HAS_SELECT 0
Перенаправьте функции сокетов на LwIP: Добавьте переопределения для функций сокетов PJSIP, чтобы использовать аналоги из LwIP:
#define pj_sock_socket       lwip_socket
#define pj_sock_bind         lwip_bind
#define pj_sock_listen       lwip_listen
#define pj_sock_accept       lwip_accept
#define pj_sock_connect      lwip_connect
#define pj_sock_send         lwip_send
#define pj_sock_recv         lwip_recv
#define pj_sock_close        lwip_close
#define pj_sock_setsockopt   lwip_setsockopt
#define pj_sock_getsockopt   lwip_getsockopt
#define pj_sock_getpeername  lwip_getpeername
#define pj_sock_getsockname  lwip_getsockname
Добавьте реализацию времени: PJSIP часто использует функции, такие как gettimeofday. Реализуйте их через таймер LwIP:
pj_uint32_t pj_gettickcount(void) {
    return sys_now(); // Возвращает время в миллисекундах.
}
Шаг 3: Модификация PJSIP

Адаптируйте вызовы функций select: В LwIP вызов select поддерживается, но с ограничениями. Убедитесь, что ваш config_site.h указывает, что вы не используете стандартный POSIX select:
#define PJSIP_DONT_USE_SYS_SELECT 1
Вместо этого используйте встроенный механизм LwIP.
DNS-запросы: Если проект использует DNS, включите поддержку DNS в LwIP:
#define LWIP_DNS 1
#define LWIP_COMPAT_SOCKETS 1
Убедитесь, что функции DNS в PJSIP перенаправлены на LwIP.
Проверьте модули: Некоторые модули PJSIP могут использовать функции, отсутствующие в LwIP (например, getaddrinfo). В этом случае вам потребуется их реализация или замена.
Шаг 4: Сборка проекта

Подключите файлы PJSIP и LwIP в проект: Убедитесь, что пути к заголовочным файлам и библиотекам PJSIP и LwIP добавлены в проект.
Настройте компилятор: Убедитесь, что PJSIP компилируется с учетом вашего нового config_site.h. Если требуется, добавьте флаг:
-Ipath_to_your_config_site.h
Соберите проект.
Шаг 5: Тестирование

Инициализация LwIP и PJSIP: Убедитесь, что стек LwIP полностью инициализирован перед вызовами функций PJSIP.
Тестирование сокетов: Проверьте базовые функции PJSIP (например, создание соединений, отправка/прием сообщений).
Отладка:
Включите отладочные сообщения PJSIP и LwIP для проверки взаимодействия.
Проверьте потребление ресурсов, так как LwIP может потребовать увеличения буферов.
Заключение

Замена BSD-сокетов на LwIP в PJSIP требует перенаправления вызовов сокетов на функции LwIP через макросы в config_site.h. Убедитесь, что все POSIX-зависимости PJSIP (например, select, getaddrinfo) заменены их эквивалентами в LwIP. Если встретите трудности на этапе интеграции, сообщите детали, и я помогу с их решением