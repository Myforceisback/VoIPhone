Чтобы связать StrongSwan с вашим сервером на C++ и реализовать шифрование полезной нагрузки через ESP, потребуется интеграция с интерфейсом VICI от StrongSwan для управления сессиями IKEv2 и ручная обработка ESP-пакетов. Вот пошаговое руководство и пример кода:

---

### 1. Настройка StrongSwan
Сначала настройте StrongSwan для обработки IKEv2. Пример конфигурации (`/etc/ipsec.conf`):
```bash
conn myvpn
    auto=add
    left=your_server_ip
    leftsubnet=0.0.0.0/0
    leftcert=server-cert.pem
    right=%any
    rightsubnet=10.0.0.0/24
    rightcert=client-cert.pem
    ike=aes256-sha256-modp2048!
    esp=aes256-sha256!
    keyexchange=ikev2
    authby=rsa-sha256
```

---

### 2. Установка зависимостей
- Установите StrongSwan и библиотеки для разработки:
  ```bash
  sudo apt-get install strongswan libstrongswan-dev
  ```
- Для работы с VICI потребуется библиотека `libvici`.

---

### 3. Пример кода сервера на C++ с Boost.Asio и VICI
Этот код демонстрирует, как:
- Слушать входящие подключения через Boost.Asio.
- Взаимодействовать с StrongSwan через VICI для получения параметров сессии.
- Шифровать данные с использованием ESP.

```cpp
#include <boost/asio.hpp>
#include <libvici.h>
#include <openssl/evp.h>
#include <iostream>
#include <memory>

using boost::asio::ip::tcp;

// Конфигурация ESP (алгоритмы из конфига StrongSwan)
constexpr auto ESP_CIPHER = EVP_aes_256_cbc();
constexpr auto ESP_HMAC = EVP_sha256();
constexpr size_t SPI_SIZE = 4; // SPI (32 бита)

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, const std::vector<uint8_t>& enc_key, const std::vector<uint8_t>& hmac_key)
        : socket_(std::move(socket)), enc_key_(enc_key), hmac_key_(hmac_key) {}

    void start() {
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(raw_data_),
            [this, self](boost::system::error_code ec, size_t length) {
                if (!ec) {
                    // Дешифровка ESP
                    std::vector<uint8_t> decrypted = decrypt_esp(raw_data_, length);
                    
                    // Обработка данных (например, передача роутеру)
                    process_data(decrypted);

                    do_read();
                }
            });
    }

    void send_encrypted(const std::vector<uint8_t>& data) {
        // Шифрование данных в ESP-пакет
        std::vector<uint8_t> esp_packet = encrypt_esp(data);
        
        // Отправка через Boost.Asio
        boost::asio::write(socket_, boost::asio::buffer(esp_packet));
    }

    std::vector<uint8_t> decrypt_esp(const uint8_t* data, size_t len) {
        // Упрощенная реализация дешифровки ESP
        EVP_CIPHER_CTX* cipher_ctx = EVP_CIPHER_CTX_new();
        EVP_DecryptInit_ex(cipher_ctx, ESP_CIPHER, nullptr, enc_key_.data(), nullptr);

        std::vector<uint8_t> decrypted(len);
        int out_len;
        EVP_DecryptUpdate(cipher_ctx, decrypted.data(), &out_len, data, len);
        EVP_CIPHER_CTX_free(cipher_ctx);

        decrypted.resize(out_len);
        return decrypted;
    }

    std::vector<uint8_t> encrypt_esp(const std::vector<uint8_t>& data) {
        // Упрощенная реализация шифровки ESP
        EVP_CIPHER_CTX* cipher_ctx = EVP_CIPHER_CTX_new();
        EVP_EncryptInit_ex(cipher_ctx, ESP_CIPHER, nullptr, enc_key_.data(), nullptr);

        std::vector<uint8_t> encrypted(data.size() + EVP_CIPHER_block_size(ESP_CIPHER));
        int out_len;
        EVP_EncryptUpdate(cipher_ctx, encrypted.data(), &out_len, data.data(), data.size());
        EVP_CIPHER_CTX_free(cipher_ctx);

        encrypted.resize(out_len);
        return encrypted;
    }

    tcp::socket socket_;
    std::vector<uint8_t> enc_key_;  // Ключ шифрования из SA
    std::vector<uint8_t> hmac_key_; // Ключ HMAC
    uint8_t raw_data_[4096];
};

// Функция для получения ключей из StrongSwan через VICI
std::pair<std::vector<uint8_t>, std::vector<uint8_t>> get_sa_keys() {
    vici_conn_t conn = vici_connect();
    vici_req_t req = vici_begin("list-sas");
    vici_response_t res = vici_submit(req, conn);
    
    // Парсинг ответа (примерно)
    // В реальности нужно извлечь enc_key и hmac_key из JSON-ответа
    std::vector<uint8_t> enc_key(32, 0xAA); // Пример ключа
    std::vector<uint8_t> hmac_key(32, 0xBB);
    
    vici_disconnect(conn);
    return {enc_key, hmac_key};
}

class Server {
public:
    Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        // Получение ключей из StrongSwan
        auto [enc_key, hmac_key] = get_sa_keys();
        enc_key_ = enc_key;
        hmac_key_ = hmac_key;
        
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                auto session = std::make_shared<Session>(std::move(socket), enc_key_, hmac_key_);
                session->start();
            }
            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    std::vector<uint8_t> enc_key_;
    std::vector<uint8_t> hmac_key_;
};

int main() {
    boost::asio::io_context io_context;
    Server server(io_context, 5000);
    io_context.run();
    return 0;
}
```

---

### 4. Компиляция и запуск
1. Скомпилируйте код с зависимостями:
   ```bash
   g++ -std=c++17 server.cpp -o server -lboost_system -lssl -lcrypto -lvici
   ```
2. Запустите StrongSwan:
   ```bash
   sudo ipsec start
   ```
3. Запустите сервер:
   ```bash
   ./server
   ```

---

### 5. Пояснения
- **VICI-интеграция**: Код использует библиотеку `libvici` для запроса параметров сессии (например, ключей шифрования) из StrongSwan.
- **Шифрование ESP**: Для шифрования/дешифрования используется OpenSSL. В реальности нужно учитывать структуру ESP-пакетов (SPI, номера последовательности и HMAC).
- **Boost.Asio**: Обрабатывает сетевые соединения асинхронно.

---

### 6. Важные замечания
- Реальный код должен обрабатывать **SPI (Security Parameters Index)** и **последовательность пакетов** для ESP.
- Для HMAC требуется дополнительная обработка.
- Используйте `libipsec` из StrongSwan, если нужно реализовать ESP в пользовательском пространстве.

Этот пример упрощен для демонстрации концепции. В реальном проекте потребуется глубокая интеграция с StrongSwan и точная реализация протокола ESP.