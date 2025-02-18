Чтобы реализовать шифратор трафика через IPsec ESP с использованием шифра Магма (ГОСТ Р 34.12-2015) и сервера на C++, выполните следующие шаги:

---

### 1. **Подготовка окружения**
#### Установите StrongSwan и зависимости:
```bash
sudo apt-get install strongswan libstrongswan-dev g++ cmake make
```

#### Скачайте или реализуйте шифр Магма:
- Возьмите открытую реализацию ГОСТ Р 34.12-2015 (например, из [репозитория CryptoPro](https://github.com/gost-engine/engine)).
- Создайте файл `magma.c` и `magma.h` с реализацией алгоритма.

---

### 2. **Патчинг StrongSwan для поддержки Магмы**
StrongSwan не поддерживает ГОСТовые алгоритмы по умолчанию. Вам нужно:
1. Создать плагин для Магмы.
2. Зарегистрировать алгоритм в StrongSwan.

#### Пример структуры плагина (`magma_plugin.c`):
```c
#include <library.h>
#include <crypto/crypto_plugin.h>
#include "magma.h" // Ваша реализация Магмы

// Функции шифрования/дешифрования
static void magma_encrypt(...) {
    magma_encrypt_block(key, iv, plain, cipher);
}

static void magma_decrypt(...) {
    magma_decrypt_block(key, iv, cipher, plain);
}

// Регистрация алгоритма
static const crypto_crypter_t crypter = {
    .encrypt = magma_encrypt,
    .decrypt = magma_decrypt,
    ...
};

// Инициализация плагина
plugin_t* magma_plugin_create() {
    return lib->plugins->add_plugin("magma-plugin",
        PLUGIN_DEPENDS(CRYPTO_PLUGIN),
        CRYPTO_CRYPTER("magma", 64, crypter), // 64-битный блок
    );
}
```

#### Сборка плагина:
```bash
gcc -shared -fPIC -o magma_plugin.so magma_plugin.c -I/path/to/strongswan/include
```

#### Настройка StrongSwan:
Добавьте в `/etc/strongswan.conf`:
```bash
charon {
    load = magma_plugin
}
```

---

### 3. **Конфигурация IPsec с Магмой**
В файле `/etc/ipsec.conf` укажите использование Магмы:
```bash
conn myvpn
    authby=secret
    ike=magma-cbc sha256 modp2048
    esp=magma-cbc sha256
    left=192.168.1.1
    leftsubnet=0.0.0.0/0
    right=%any
    rightsubnet=10.0.0.0/24
    auto=add
```

---

### 4. **Сервер на C++ для работы с ESP**
Сервер будет:
1. Перехватывать ESP-пакеты через RAW-сокет.
2. Шифровать/дешифровать их с помощью Магмы.
3. Пересылать трафик через TUN-интерфейс.

#### Пример кода:
```cpp
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "magma.h" // Ваш шифр

// Создание TUN-интерфейса
int create_tun() {
    struct ifreq ifr;
    int fd = open("/dev/net/tun", O_RDWR);
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    strncpy(ifr.ifr_name, "tun0", IFNAMSIZ);
    ioctl(fd, TUNSETIFF, &ifr);
    return fd;
}

// Обработка ESP-пакета
void process_esp_packet(uint8_t* data, size_t len, Magma& cipher) {
    struct iphdr* ip = (struct iphdr*)data;
    if (ip->protocol != IPPROTO_ESP) return;

    // Извлечение ESP-заголовка
    struct esphdr* esp = (struct esphdr*)(data + ip->ihl * 4);
    uint8_t* payload = (uint8_t*)(esp + 1);

    // Дешифровка (пример)
    std::vector<uint8_t> decrypted = cipher.decrypt(payload, len - sizeof(*esp));
    // ... (передача в TUN)
}

int main() {
    int raw_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    int tun_fd = create_tun();

    Magma cipher("secret_key_256bit");

    while (true) {
        uint8_t buffer[4096];
        ssize_t len = recv(raw_sock, buffer, sizeof(buffer), 0);
        if (len <= 0) break;

        process_esp_packet(buffer, len, cipher);
    }

    close(raw_sock);
    close(tun_fd);
    return 0;
}
```

---

### 5. **Настройка маршрутизации**
```bash
# На сервере
sudo ip addr add 10.0.0.1/24 dev tun0
sudo ip link set tun0 up
sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
```

---

### 6. **Компиляция и запуск**
```bash
g++ -std=c++17 server.cpp magma.cpp -o server -lpthread
sudo ./server
```

---

### 7. **Проверка**
1. Подключитесь с клиента через IPsec:
   ```bash
   sudo ipsec up myvpn
   ```
2. Проверьте трафик через `tcpdump`:
   ```bash
   sudo tcpdump -i tun0 -n
   ```

---

### **Важные замечания**
1. **Производительность**: Обработка в пользовательском пространстве медленнее, чем в ядре.
2. **Совместимость**: Требуется одинаковый ключ и настройки на сервере и клиенте.
3. **Безопасность**: Используйте надежные ключи и защищенные каналы для их обмена.

Если задача кажется слишком сложной, рассмотрите использование **TUN/TAP** с собственным шифрованием без IPsec.