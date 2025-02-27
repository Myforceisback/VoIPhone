В Boost.Asio есть встроенная поддержка пула потоков через класс `boost::asio::thread_pool`. Этот класс упрощает создание и управление пулом потоков, которые могут выполнять задачи, связанные с `io_context`. Вместо того чтобы вручную создавать потоки и вызывать `io_context.run()`, можно использовать `thread_pool`, который автоматически управляет потоками.

Вот пример использования `boost::asio::thread_pool`:

### Пример кода:

```cpp
#include <boost/asio.hpp>
#include <iostream>

int main() {
    // Создаем пул потоков с количеством потоков, равным количеству ядер процессора
    boost::asio::thread_pool pool(std::thread::hardware_concurrency());

    // Запускаем задачи в пуле потоков
    for (int i = 0; i < 10; ++i) {
        boost::asio::post(pool, [i]() {
            std::cout << "Task " << i << " is running on thread " 
                      << std::this_thread::get_id() << std::endl;
        });
    }

    // Ожидаем завершения всех задач в пуле
    pool.join();

    return 0;
}
```

### Пояснение:
1. **`boost::asio::thread_pool`**:
   - Это готовый пул потоков, который автоматически создает и управляет потоками.
   - Количество потоков можно указать в конструкторе. В примере используется `std::thread::hardware_concurrency()`, чтобы создать столько потоков, сколько ядер у процессора.

2. **`boost::asio::post`**:
   - Функция `post` добавляет задачу в пул потоков для выполнения.
   - Задачи выполняются асинхронно в одном из потоков пула.

3. **`pool.join()`**:
   - Блокирует выполнение текущего потока до тех пор, пока все задачи в пуле не будут завершены.
   - После вызова `join()` пул потоков завершает свою работу.

### Преимущества использования `thread_pool`:
- Упрощает управление потоками: не нужно вручную создавать и управлять потоками.
- Автоматически распределяет задачи между потоками.
- Позволяет легко масштабировать количество потоков.

### Если нужно использовать `io_context` с `thread_pool`:
Если вы хотите использовать `io_context` вместе с `thread_pool`, вы можете связать их следующим образом:

```cpp
#include <boost/asio.hpp>
#include <iostream>

int main() {
    // Создаем io_context
    boost::asio::io_context io_context;

    // Создаем пул потоков
    boost::asio::thread_pool pool(std::thread::hardware_concurrency());

    // Запускаем io_context.run() в пуле потоков
    boost::asio::post(pool, [&io_context]() {
        io_context.run();
    });

    // Добавляем асинхронные операции в io_context
    boost::asio::post(io_context, []() {
        std::cout << "Async operation 1 is running on thread " 
                  << std::this_thread::get_id() << std::endl;
    });

    boost::asio::post(io_context, []() {
        std::cout << "Async operation 2 is running on thread " 
                  << std::this_thread::get_id() << std::endl;
    });

    // Ожидаем завершения всех задач в пуле
    pool.join();

    return 0;
}
```

### Пояснение:
- В этом примере `io_context.run()` запускается в одном из потоков пула.
- Асинхронные операции добавляются в `io_context`, и они выполняются в потоке, который вызвал `run()`.

Этот подход позволяет использовать `io_context` вместе с пулом потоков для выполнения асинхронных операций.