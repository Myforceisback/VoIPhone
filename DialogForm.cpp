Добавление заголовка ESP в IP-пакет вручную требует изменения заголовка IP перед отправкой через RAW-сокеты. Общий порядок действий:  

1. **Создание IP-заголовка**  
2. **Формирование заголовка ESP**  
3. **Шифрование полезной нагрузки**  
4. **Добавление ESP-заголовка в IP-пакет**  
5. **Отправка через RAW-сокет**  

---

### **1. Формат IP-заголовка с ESP**
ESP-заголовок вставляется между IP-заголовком и полезной нагрузкой.  

**IPv4 до ESP:**  
```
[ IP Header ] [ ESP Header ] [ Encrypted Payload ] [ ESP Trailer ] [ ESP Auth ]
```
**IPv6:**  
ESP является отдельным заголовком (Next Header указывает на ESP).  

---

### **2. Код на C++ для формирования IP + ESP**
```cpp
#include <iostream>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/if_ether.h>

#define BUFFER_SIZE 4096

struct esp_header {
    uint32_t spi;
    uint32_t seq;
    uint8_t encrypted_data[BUFFER_SIZE]; // Полезная нагрузка
} __attribute__((packed));

int main() {
    int raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (raw_sock < 0) {
        perror("RAW socket failed");
        return 1;
    }

    struct sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("192.168.1.2");

    char packet[BUFFER_SIZE];
    struct iphdr *ip = (struct iphdr*)packet;
    struct esp_header *esp = (struct esp_header*)(packet + sizeof(struct iphdr));

    // Заполняем IP-заголовок
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct esp_header));
    ip->id = htons(54321);
    ip->frag_off = 0;
    ip->ttl = 64;
    ip->protocol = IPPROTO_ESP; // Указываем, что следующий заголовок - ESP
    ip->saddr = inet_addr("192.168.1.1");
    ip->daddr = inet_addr("192.168.1.2");
    ip->check = 0; // Чек-сумму нужно вычислять

    // Заполняем заголовок ESP
    esp->spi = htonl(0x100); // SPI - Security Parameter Index
    esp->seq = htonl(1); // Порядковый номер пакета

    // Шифруем полезную нагрузку
    memset(esp->encrypted_data, 0, BUFFER_SIZE); // Здесь вызовите шифрование Магма

    // Отправляем пакет
    if (sendto(raw_sock, packet, sizeof(struct iphdr) + sizeof(struct esp_header), 0,
               (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("sendto failed");
    }

    close(raw_sock);
    return 0;
}
```

---

### **3. Объяснение кода**
- **IP-заголовок**: указывает `IPPROTO_ESP` (50), указывая, что следующий заголовок — ESP.  
- **ESP-заголовок**: содержит `SPI` и `Sequence Number`, затем идут зашифрованные данные.  
- **Отправка через RAW-сокет**: передача IP-пакета напрямую.  

Этот код создает IP-пакет с ESP-заголовком и отправляет его. Вам нужно добавить шифрование полезной нагрузки Магмой.  

Если требуется работа с IPsec SA/SP, используйте `ip xfrm` или `PF_KEY`.