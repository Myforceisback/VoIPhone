Для создания IP-шифратора на C++ с использованием IPsec ESP и алгоритма "Магма" (ГОСТ 28147-89) выполните следующие шаги:

---

### 1. Настройка окружения
- Установите необходимые библиотеки:
  ```bash
  sudo apt install build-essential libpcap-dev
  ```

---

### 2. Реализация захвата и отправки пакетов (RAW-сокеты)
```cpp
#include <iostream>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/ether.h>

// Callback-функция для обработки пакетов
void packet_handler(u_char *user, const struct pcap_pkthdr *hdr, const u_char *data) {
    struct iphdr *ip_header = (struct iphdr*)(data + sizeof(struct ethhdr));
    // Здесь будет обработка IP-пакета
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_live("eth0", BUFSIZ, 1, 1000, errbuf);
    
    if (handle == nullptr) {
        std::cerr << "Error opening interface: " << errbuf << std::endl;
        return 1;
    }
    
    pcap_loop(handle, 0, packet_handler, nullptr);
    pcap_close(handle);
    return 0;
}
```

---

### 3. Реализация алгоритма "Магма" (ГОСТ 28147-89)
```cpp
#include <cstdint>
#include <vector>

class MagmaCipher {
    uint32_t key[8]; // 256-битный ключ

    // S-блоки ГОСТ
    const uint8_t sbox[8][16] = { /* ... */ };

    uint32_t f(uint32_t block, uint32_t key) {
        // Реализация функции раунда
        return block;
    }

public:
    MagmaCipher(const uint8_t* key_data) {
        // Инициализация ключа
    }

    void encrypt(uint8_t* data, size_t len) {
        // Шифрование данных
    }

    void decrypt(uint8_t* data, size_t len) {
        // Дешифрование
    }
};
```

---

### 4. Реализация IPsec ESP
#### Структура ESP-заголовка:
```cpp
struct esp_header {
    uint32_t spi;   // Security Parameters Index
    uint32_t seq;   // Sequence Number
    uint8_t iv[8];  // Initialization Vector
    // Далее следует зашифрованная нагрузка
};
```

#### Шифрование пакета:
```cpp
void encrypt_packet(uint8_t* packet, size_t len, MagmaCipher& cipher) {
    // Добавление ESP-заголовка
    struct esp_header esp;
    esp.spi = htonl(0x12345678);
    esp.seq = htonl(1);
    
    // Генерация IV
    // ...
    
    // Шифрование данных
    cipher.encrypt(packet + sizeof(esp_header), len - sizeof(esp_header));
}
```

---

### 5. Туннелирование через RAW-сокеты
```cpp
#include <sys/socket.h>
#include <netinet/ip.h>

int create_raw_socket() {
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    int one = 1;
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one));
    return sock;
}

void send_encrypted_packet(int sock, struct sockaddr_in* dest, uint8_t* data, size_t len) {
    sendto(sock, data, len, 0, (struct sockaddr*)dest, sizeof(*dest));
}
```

---

### 6. Сборка проекта
Компиляция:
```bash
g++ -o ipsec_encryptor main.cpp -lpcap
```

---

### 7. Настройка сети
- Включите форвардинг пакетов:
  ```bash
  sudo sysctl -w net.ipv4.ip_forward=1
  ```
- Настройте iptables для перенаправления трафика:
  ```bash
  sudo iptables -A INPUT -i eth0 -j NFQUEUE --queue-num 0
  ```

---

### Важные замечания:
1. **Ключевое управление**: Используйте предустановленные ключи для упрощения.
2. **Производительность**: Для обработки больших объемов данных используйте многопоточность.
3. **Безопасность**: 
   - Всегда проверяйте целостность пакетов (HMAC).
   - Используйте защищенные каналы для обмена ключами.
4. **Тестирование**: Проверяйте в изолированной сети перед развертыванием.

---

### Пример конфигурации туннеля:
```
Локальный шифратор:
IP: 192.168.1.100
Удаленный шифратор:
IP: 192.168.1.200
SPI: 0x12345678
Ключ: 00112233...FF (32 байта)
```

Это базовая реализация. Для промышленного использования потребуется:
- Поддержка IKEv2 для обмена ключами
- Обработка фрагментации пакетов
- Механизмы защиты от повторов (anti-replay)