Использование raw socket для отправки пакетов IKEv2 требует работы на низком уровне сетевого стека. Это подразумевает ручное создание IP-заголовка и UDP-заголовка (если используется UDP-инкапсуляция, что часто применяется для IKEv2). В этом примере я покажу, как отправить IKEv2 пакет через raw socket на Linux.

### Пример: Отправка IKEv2 пакета через raw socket

```cpp
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

// Структура для IKEv2 заголовка
struct IKEv2Header {
    uint8_t initiatorSPI[8];
    uint8_t responderSPI[8];
    uint8_t nextPayload;
    uint8_t version;
    uint8_t exchangeType;
    uint8_t flags;
    uint32_t messageID;
    uint32_t length;
};

// Функция для вычисления контрольной суммы (упрощённая версия)
uint16_t checksum(const void* data, size_t length) {
    const uint16_t* ptr = reinterpret_cast<const uint16_t*>(data);
    uint32_t sum = 0;
    for (size_t i = 0; i < length / 2; ++i) {
        sum += ptr[i];
    }
    if (length % 2) {
        sum += reinterpret_cast<const uint8_t*>(data)[length - 1];
    }
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return static_cast<uint16_t>(~sum);
}

// Функция для создания и отправки IKEv2 пакета
void sendIKEv2Packet(const std::string& destIP, const std::string& srcIP, uint16_t destPort, uint16_t srcPort) {
    // Создаём raw socket
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) {
        perror("Socket creation failed");
        return;
    }

    // Включаем ручное заполнение IP-заголовка
    int one = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) {
        perror("setsockopt failed");
        close(sock);
        return;
    }

    // Создаём буфер для пакета
    char packet[4096];
    memset(packet, 0, sizeof(packet));

    // Указатели на заголовки
    struct iphdr* ipHeader = reinterpret_cast<struct iphdr*>(packet);
    struct udphdr* udpHeader = reinterpret_cast<struct udphdr*>(packet + sizeof(struct iphdr));
    IKEv2Header* ikeHeader = reinterpret_cast<IKEv2Header*>(packet + sizeof(struct iphdr) + sizeof(struct udphdr));

    // Заполняем IKEv2 заголовок
    memset(ikeHeader->initiatorSPI, 0x11, sizeof(ikeHeader->initiatorSPI));
    memset(ikeHeader->responderSPI, 0x22, sizeof(ikeHeader->responderSPI));
    ikeHeader->nextPayload = 33; // Пример: SA payload
    ikeHeader->version = 0x20;  // IKEv2
    ikeHeader->exchangeType = 34; // Пример: IKE_SA_INIT
    ikeHeader->flags = 0x08;     // Initiator bit set
    ikeHeader->messageID = htonl(1);
    ikeHeader->length = htonl(sizeof(IKEv2Header));

    // Заполняем UDP заголовок
    udpHeader->source = htons(srcPort);
    udpHeader->dest = htons(destPort);
    udpHeader->len = htons(sizeof(struct udphdr) + sizeof(IKEv2Header));
    udpHeader->check = 0; // Контрольная сумма будет вычислена позже

    // Заполняем IP заголовок
    ipHeader->ihl = 5;
    ipHeader->version = 4;
    ipHeader->tos = 0;
    ipHeader->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(IKEv2Header));
    ipHeader->id = htons(54321);
    ipHeader->frag_off = 0;
    ipHeader->ttl = 64;
    ipHeader->protocol = IPPROTO_UDP;
    ipHeader->check = 0; // Контрольная сумма будет вычислена позже
    inet_pton(AF_INET, srcIP.c_str(), &ipHeader->saddr);
    inet_pton(AF_INET, destIP.c_str(), &ipHeader->daddr);

    // Вычисляем контрольную сумму IP заголовка
    ipHeader->check = checksum(ipHeader, sizeof(struct iphdr));

    // Вычисляем контрольную сумму UDP заголовка (псевдозаголовок)
    struct {
        uint32_t srcAddr;
        uint32_t destAddr;
        uint8_t zero;
        uint8_t protocol;
        uint16_t length;
    } pseudoHeader;
    pseudoHeader.srcAddr = ipHeader->saddr;
    pseudoHeader.destAddr = ipHeader->daddr;
    pseudoHeader.zero = 0;
    pseudoHeader.protocol = IPPROTO_UDP;
    pseudoHeader.length = udpHeader->len;

    char pseudoPacket[sizeof(pseudoHeader) + sizeof(struct udphdr) + sizeof(IKEv2Header)];
    memcpy(pseudoPacket, &pseudoHeader, sizeof(pseudoHeader));
    memcpy(pseudoPacket + sizeof(pseudoHeader), udpHeader, sizeof(struct udphdr) + sizeof(IKEv2Header));
    udpHeader->check = checksum(pseudoPacket, sizeof(pseudoPacket));

    // Отправляем пакет
    struct sockaddr_in destAddr;
    memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    inet_pton(AF_INET, destIP.c_str(), &destAddr.sin_addr);

    if (sendto(sock, packet, ntohs(ipHeader->tot_len), 0, reinterpret_cast<struct sockaddr*>(&destAddr), sizeof(destAddr)) < 0) {
        perror("Sendto failed");
    } else {
        std::cout << "IKEv2 packet sent successfully!" << std::endl;
    }

    close(sock);
}

int main() {
    // Параметры для отправки
    std::string destIP = "192.168.1.1"; // Адрес назначения
    std::string srcIP = "192.168.1.2";  // Адрес источника
    uint16_t destPort = 500;           // Порт назначения (стандартный для IKEv2)
    uint16_t srcPort = 4500;           // Порт источника

    // Отправляем IKEv2 пакет
    sendIKEv2Packet(destIP, srcIP, destPort, srcPort);

    return 0;
}
```

### Пояснения:
1. **Raw socket**: Создаётся сокет с использованием `SOCK_RAW`, что позволяет вручную заполнять заголовки IP и UDP.
2. **IP заголовок**: Заполняется вручную, включая контрольную сумму.
3. **UDP заголовок**: Заполняется вручную, включая контрольную сумму, которая требует использования псевдозаголовка.
4. **IKEv2 заголовок**: Заполняется вручную, как в предыдущем примере.
5. **Отправка пакета**: Используется `sendto` для отправки пакета на указанный адрес.

### Важные моменты:
- **Права администратора**: Для работы с raw socket требуются права root.
- **Контрольные суммы**: Вычисляются вручную для IP и UDP заголовков.
- **Псевдозаголовок UDP**: Используется для вычисления контрольной суммы UDP.

### Запуск:
1. Скомпилируйте программу:
   ```bash
   g++ -o ikev2_send ikev2_send.cpp -lssl -lcrypto
   ```
2. Запустите с правами root:
   ```bash
   sudo ./ikev2_send
   ```

Этот пример демонстрирует базовую отправку IKEv2 пакета через raw socket. Для полной реализации IKEv2 потребуется добавить поддержку всех типов payload, обработку ответов и другие функции.