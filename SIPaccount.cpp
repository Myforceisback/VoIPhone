Если ты установил Boost через `apt install libboost-all-dev`, то библиотеки и заголовочные файлы уже находятся в системных путях, и тебе не нужно вручную указывать `BOOST_ROOT` или `BOOST_LIBRARYDIR`. Просто используй стандартный `find_package` в `CMakeLists.txt`.

Вот минимальный рабочий пример:

### `CMakeLists.txt`
```cmake
cmake_minimum_required(VERSION 3.10)
project(YourProject)

# Ищем Boost
find_package(Boost REQUIRED COMPONENTS system)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(YourProject main.cpp)
    target_link_libraries(YourProject Boost::system)
else()
    message(FATAL_ERROR "Boost не найден!")
endif()
```

### `main.cpp`
```cpp
#include <boost/asio.hpp>
#include <iostream>

int main() {
    boost::asio::io_context io;
    std::cout << "Boost.Asio работает!" << std::endl;
    return 0;
}
```

### Сборка
1. Создай папку `build` и перейди в неё:
   ```bash
   mkdir build
   cd build
   ```

2. Запусти CMake:
   ```bash
   cmake ..
   ```

3. Собери проект:
   ```bash
   make
   ```

4. Запусти программу:
   ```bash
   ./YourProject
   ```

### Если всё равно ругается
1. Убедись, что Boost установлен:
   ```bash
   dpkg -l | grep libboost
   ```

2. Если Boost установлен, но CMake его не находит, попробуй явно указать версию CMake:
   ```bash
   cmake -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake/Boost ..
   ```

3. Если проблема сохраняется, напиши, я помогу разобраться!