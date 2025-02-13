Привет! Для создания HTML-страницы регистрации с использованием Drogon Framework на C++ и последующего перенаправления пользователя на страницу настроек, вам нужно выполнить несколько шагов. Вот пример того, как это можно сделать:

### 1. Установка Drogon Framework
Если вы еще не установили Drogon, вы можете сделать это с помощью CMake:

```bash
git clone https://github.com/drogonframework/drogon
cd drogon
mkdir build
cd build
cmake ..
make && sudo make install
```

### 2. Создание проекта
Создайте новый проект и добавьте необходимые файлы:

```bash
mkdir my_drogon_app
cd my_drogon_app
```

### 3. Создание HTML-страницы регистрации
Создайте файл `register.html` в папке `views`:

```html
<!-- views/register.html -->
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Регистрация</title>
</head>
<body>
    <h1>Регистрация</h1>
    <form action="/register" method="post">
        <label for="login">Логин:</label>
        <input type="text" id="login" name="login" required><br><br>
        <label for="password">Пароль:</label>
        <input type="password" id="password" name="password" required><br><br>
        <button type="submit">Зарегистрироваться</button>
    </form>
</body>
</html>
```

### 4. Создание HTML-страницы настроек
Создайте файл `settings.html` в папке `views`:

```html
<!-- views/settings.html -->
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Настройки</title>
</head>
<body>
    <h1>Настройки</h1>
    <p>Добро пожаловать в настройки!</p>
</body>
</html>
```

### 5. Создание контроллера для обработки запросов
Создайте файл `RegisterController.cc`:

```cpp
#include <drogon/drogon.h>

using namespace drogon;

class RegisterController : public HttpController<RegisterController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RegisterController::showRegisterPage, "/register", Get);
    ADD_METHOD_TO(RegisterController::handleRegister, "/register", Post);
    METHOD_LIST_END

    void showRegisterPage(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
    {
        auto resp = HttpResponse::newHttpViewResponse("register");
        callback(resp);
    }

    void handleRegister(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
    {
        auto json = req->getJsonObject();
        std::string login = (*json)["login"].asString();
        std::string password = (*json)["password"].asString();

        // Здесь должна быть проверка логина и пароля в базе данных
        // Например, если проверка прошла успешно:
        if (login == "user" && password == "password")
        {
            auto resp = HttpResponse::newRedirectionResponse("/settings");
            callback(resp);
        }
        else
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k403Forbidden);
            resp->setBody("Неверный логин или пароль");
            callback(resp);
        }
    }
};

class SettingsController : public HttpController<SettingsController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(SettingsController::showSettingsPage, "/settings", Get);
    METHOD_LIST_END

    void showSettingsPage(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
    {
        auto resp = HttpResponse::newHttpViewResponse("settings");
        callback(resp);
    }
};

int main()
{
    app().registerController(std::make_shared<RegisterController>());
    app().registerController(std::make_shared<SettingsController>());
    app().setDocumentRoot("./views");
    app().addListener("0.0.0.0", 8080);
    app().run();
    return 0;
}
```

### 6. Сборка и запуск проекта
Создайте `CMakeLists.txt` для сборки проекта:

```cmake
cmake_minimum_required(VERSION 3.10)
project(my_drogon_app)

set(CMAKE_CXX_STANDARD 14)

find_package(Drogon REQUIRED)

add_executable(my_drogon_app RegisterController.cc)

target_link_libraries(my_drogon_app PRIVATE Drogon::Drogon)
```

Теперь соберите и запустите проект:

```bash
mkdir build
cd build
cmake ..
make
./my_drogon_app
```

### 7. Проверка работы
Откройте браузер и перейдите по адресу `http://localhost:8080/register`. Вы увидите страницу регистрации. После ввода логина и пароля и нажатия на кнопку "Зарегистрироваться", если логин и пароль верны, вы будете перенаправлены на страницу настроек.

### 8. Работа с базой данных
Для проверки логина и пароля в базе данных, вам нужно будет использовать ORM Drogon или напрямую работать с SQL-запросами. Это уже зависит от вашей конкретной реализации базы данных.

Надеюсь, это поможет вам начать работу с Drogon Framework! Если у вас есть дополнительные вопросы, не стесняйтесь задавать.