Вот реализация сервера с использованием **Boost.Asio**, **Raw Socket** и **пула потоков** для обработки входящих пакетов:  

### **Ключевые моменты реализации**  
- **Raw Socket** (AF_PACKET на Linux) для приема всех входящих пакетов.  
- **Boost.Asio** для асинхронной обработки и распределения работы.  
- **Пул потоков** с использованием `std::thread` и `boost::asio::thread_pool`.  

### **Код сервера**
```cpp
#include <iostream>
#include <vector>
#include <thread>
#include <boost/asio.hpp>
#include <linux/if_packet.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <unistd.h>

class RawSocketServer {
public:
    RawSocketServer(boost::asio::thread_pool &pool, const std::string &interface)
        : io_context_(pool.get_executor()), socket_fd_(-1) {
        // Создаем raw-сокет
        socket_fd_ = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        if (socket_fd_ < 0) {
            perror("socket");
            throw std::runtime_error("Failed to create raw socket");
        }

        // Привязываем сокет к сетевому интерфейсу
        struct sockaddr_ll sa{};
        sa.sll_family = AF_PACKET;
        sa.sll_protocol = htons(ETH_P_ALL);
        sa.sll_ifindex = if_nametoindex(interface.c_str());
        if (sa.sll_ifindex == 0) {
            perror("if_nametoindex");
            throw std::runtime_error("Failed to get interface index");
        }

        if (bind(socket_fd_, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
            perror("bind");
            throw std::runtime_error("Failed to bind socket");
        }

        std::cout << "Listening on interface: " << interface << std::endl;
    }

    ~RawSocketServer() {
        if (socket_fd_ >= 0) {
            close(socket_fd_);
        }
    }

    void start() {
        for (int i = 0; i < 4; ++i) { // 4 потока в пуле
            boost::asio::post(io_context_, [this] { receive_packets(); });
        }
    }

private:
    void receive_packets() {
        constexpr size_t BUFFER_SIZE = 65536;
        std::vector<uint8_t> buffer(BUFFER_SIZE);

        while (true) {
            ssize_t received_bytes = recvfrom(socket_fd_, buffer.data(), BUFFER_SIZE, 0, nullptr, nullptr);
            if (received_bytes > 0) {
                process_packet(buffer.data(), received_bytes);
            }
        }
    }

    void process_packet(const uint8_t *data, ssize_t length) {
        // Разбираем Ethernet-заголовок
        auto *eth_header = (struct ether_header *)data;
        std::cout << "Packet received: " 
                  << "Src MAC: " << ether_ntoa((struct ether_addr *)eth_header->ether_shost) 
                  << " -> Dst MAC: " << ether_ntoa((struct ether_addr *)eth_header->ether_dhost) 
                  << " Type: " << ntohs(eth_header->ether_type) << std::endl;
    }

    boost::asio::io_context &io_context_;
    int socket_fd_;
};

int main() {
    try {
        boost::asio::thread_pool pool(4); // Пул из 4 потоков
        RawSocketServer server(pool, "eth0"); // Указываем интерфейс

        server.start();
        pool.join(); // Ожидание завершения работы потоков
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    return 0;
}
```

### **Объяснение кода**
1. **Создание raw-сокета**:
   - Используется `socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))` для перехвата всех пакетов на уровне Ethernet.
   - Привязывается к заданному интерфейсу (`eth0`).
   
2. **Пул потоков (`boost::asio::thread_pool`)**:
   - Создается пул из **4 потоков**.
   - В каждом потоке вызывается `receive_packets()` для обработки входящих пакетов.

3. **Обработка пакетов**:
   - Выводит **MAC-адреса** источника и назначения, а также **EtherType**.

### **Запуск**
```sh
sudo ./server
```
> **Важно**: Raw-сокеты требуют **root-прав**.