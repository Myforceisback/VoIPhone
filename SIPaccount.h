Конечно! Вот пример простого веб-интерфейса на HTML, который включает страницу регистрации и основную страницу с настройками сети, времени и списком подключенных клиентов.

### 1. Страница регистрации (`register.html`)

```html
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Регистрация</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }
        .container {
            background-color: #fff;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            width: 300px;
        }
        h2 {
            text-align: center;
            margin-bottom: 20px;
        }
        label {
            display: block;
            margin-bottom: 5px;
            font-weight: bold;
        }
        input[type="text"], input[type="password"] {
            width: 100%;
            padding: 10px;
            margin-bottom: 15px;
            border: 1px solid #ccc;
            border-radius: 4px;
        }
        input[type="submit"] {
            width: 100%;
            padding: 10px;
            background-color: #28a745;
            color: #fff;
            border: none;
            border-radius: 4px;
            cursor: pointer;
        }
        input[type="submit"]:hover {
            background-color: #218838;
        }
    </style>
</head>
<body>
    <div class="container">
        <h2>Регистрация</h2>
        <form action="main.html" method="get">
            <label for="username">Имя пользователя:</label>
            <input type="text" id="username" name="username" required>

            <label for="password">Пароль:</label>
            <input type="password" id="password" name="password" required>

            <input type="submit" value="Зарегистрироваться">
        </form>
    </div>
</body>
</html>
```

### 2. Основная страница (`main.html`)

```html
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Основная страница</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            margin: 0;
            padding: 0;
        }
        .container {
            max-width: 800px;
            margin: 50px auto;
            padding: 20px;
            background-color: #fff;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        h1 {
            text-align: center;
            margin-bottom: 20px;
        }
        .section {
            margin-bottom: 30px;
        }
        .section h2 {
            margin-bottom: 10px;
            font-size: 1.5em;
        }
        .section label {
            display: block;
            margin-bottom: 5px;
            font-weight: bold;
        }
        .section input[type="text"], .section input[type="time"] {
            width: 100%;
            padding: 10px;
            margin-bottom: 15px;
            border: 1px solid #ccc;
            border-radius: 4px;
        }
        .section button {
            padding: 10px 20px;
            background-color: #007bff;
            color: #fff;
            border: none;
            border-radius: 4px;
            cursor: pointer;
        }
        .section button:hover {
            background-color: #0056b3;
        }
        .client-list {
            list-style-type: none;
            padding: 0;
        }
        .client-list li {
            padding: 10px;
            border-bottom: 1px solid #ccc;
        }
        .client-list li:last-child {
            border-bottom: none;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Основная страница</h1>

        <div class="section">
            <h2>Настройка сети</h2>
            <label for="ip-address">IP-адрес:</label>
            <input type="text" id="ip-address" name="ip-address" placeholder="192.168.1.1">

            <label for="subnet-mask">Маска подсети:</label>
            <input type="text" id="subnet-mask" name="subnet-mask" placeholder="255.255.255.0">

            <label for="gateway">Шлюз:</label>
            <input type="text" id="gateway" name="gateway" placeholder="192.168.1.254">

            <button onclick="saveNetworkSettings()">Сохранить настройки сети</button>
        </div>

        <div class="section">
            <h2>Настройка времени</h2>
            <label for="time">Текущее время:</label>
            <input type="time" id="time" name="time">

            <button onclick="saveTimeSettings()">Сохранить время</button>
        </div>

        <div class="section">
            <h2>Подключенные клиенты</h2>
            <ul class="client-list" id="client-list">
                <li>Клиент 1</li>
                <li>Клиент 2</li>
                <li>Клиент 3</li>
            </ul>
        </div>
    </div>

    <script>
        function saveNetworkSettings() {
            alert('Настройки сети сохранены!');
        }

        function saveTimeSettings() {
            alert('Время сохранено!');
        }
    </script>
</body>
</html>
```

### Описание:

1. **Страница регистрации (`register.html`)**:
   - Простая форма регистрации с полями для ввода имени пользователя и пароля.
   - После заполнения формы пользователь перенаправляется на основную страницу (`main.html`).

2. **Основная страница (`main.html`)**:
   - **Настройка сети**: Поля для ввода IP-адреса, маски подсети и шлюза. Кнопка для сохранения настроек.
   - **Настройка времени**: Поле для выбора времени и кнопка для сохранения.
   - **Список подключенных клиентов**: Простой список клиентов, который можно расширить динамически с помощью JavaScript.

### Дальнейшие улучшения:
- Добавьте валидацию форм на стороне клиента и сервера.
- Используйте JavaScript для динамического обновления списка подключенных клиентов.
- Подключите backend (например, на Node.js, Python, PHP) для обработки данных и хранения их в базе данных.

Этот пример является базовым и может быть расширен в зависимости от ваших требований.