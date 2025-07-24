#ifndef WEB_H
#define WEB_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
  <title>ESP32 Panel [0xbdg]</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f2f4f8;
      margin: 0;
      padding: 0;
    }

    .container {
      max-width: 800px;
      margin: 40px auto;
      padding: 30px; 
    }

    h2 {
      text-align: center;
      margin-bottom: 40px;
      color: #333;
    }

    .section {
      margin-bottom: 40px;
      padding-bottom: 30px;
      border-bottom: 1px solid #ddd;
    }

    .section h3 {
      margin-bottom: 15px;
      color: #007bff;
    }

    label {
      display: block;
      margin-top: 12px;
      font-weight: bold;
    }

    input[type="text"] {
      width: 100%;
      padding: 10px;
      margin-top: 5px;
      border-radius: 5px;
      border: 1px solid #ccc;
      box-sizing: border-box;
    }

    input[type="submit"] {
      background-color: #007bff;
      color: white;
      padding: 12px 25px;
      font-size: 15px;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      margin-top: 20px;
    }

    input[type="submit"]:hover {
      background-color: #0056b3;
    }

    .current-value {
      font-size: 0.9em;
      color: #555;
      margin-top: 4px;
      background: #f8f9fa;
      padding: 6px 10px;
      border-left: 4px solid #007bff;
      border-radius: 4px;
    }

    .footer {
      background-color: #333;
      color: white;
      padding: 15px;
    }
  </style>
</head>
<body>
  <div class="container">
    <h2>Konfigurasi ESP32</h2>
    <button onclick="logoutButton()">Logout</button>

    <div class="section">
      <h3>WiFi</h3>
      <form method="POST" action="/save-wifi">
        <label for="botToken">SSID</label>
        <input type="text" id="username" name="ssid" required>
        <div class="current-value">Saat ini: %SSID%</div>

        <label for="chatId">WiFi Password</label>
        <input type="text" id="password" name="password" required>
        <div class="current-value">Saat ini: %PASS%</div>

        <input type="submit" value="Simpan Kredensial">
      </form>
    </div>

    <div class="section">
      <h3>Admin Auth</h3>
      <form method="POST" action="/save-admin">
        <label for="botToken">Admin Username</label>
        <input type="text" id="username" name="username" required>
        <div class="current-value">Saat ini: %USERNAME%</div>

        <label for="chatId">Admin Password</label>
        <input type="text" id="password" name="password" required>
        <div class="current-value">Saat ini: %USER_PASS%</div>

        <input type="submit" value="Simpan Kredensial">
      </form>
    </div>
 
    <div class="section">
      <h3>Telegram</h3>
      <form method="POST" action="/save-tele">
        <label for="botToken">Bot Token</label>
        <input type="text" id="botToken" name="botToken" required>
        <div class="current-value">Saat ini: 123456:ABCDEF-xxx</div>

        <label for="chatId">Chat ID</label>
        <input type="text" id="chatId" name="chatId" required>
        <div class="current-value">Saat ini: 987654321</div>

        <input type="submit" value="Simpan Telegram">
      </form>
    </div>

    <div class="section">
      <h3>Google Spreadsheet</h3>
      <form method="POST" action="/save-spreadsheet">
        <label for="sheetUrl">App Script URL</label>
        <input type="text" id="sheetUrl" name="sheetUrl" placeholder="Ex: https://script.google.com/macros/s/[ID]/exec">
        <div class="current-value">Saat ini: https://docs.google.com/spreadsheets/...</div>

        <label for="sheetApiKey">Sheet</label>
        <input type="text" id="sheetApiKey" name="sheet" placeholder="Ex: Sheet1">
        <div class="current-value">Saat ini: AIzaSyD********</div>

        <input type="submit" value="Simpan Spreadsheet">
      </form>
    </div>

    <div class="section">
      <h3>API Endpoint</h3>
      <form method="POST" action="/save-api">
        <label for="apiUrl">Endpoint URL</label>
        <input type="text" id="apiUrl" name="apiUrl" placeholder="Ex: https://example.com/data/siswa">
        <div class="current-value">Saat ini: https://example.com/api/post</div>

        <input type="submit" value="Simpan API">
      </form>
    </div>

  </div>
  <footer class="footer">
    <p>&copy; 2025 Benjamin Dharma Guntara. All rights reserved.</p>
  </footer>
  <script>
      function logoutButton() {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/logout", true);
        xhr.send();
        setTimeout(function(){ window.open("/logged-out","_self"); }, 1000);
      }
  </script>
</body>
</html>
)rawliteral";

const char logout_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
  <p>Logged out or <a href="/">return to homepage</a>.</p>
  <p><strong>Note:</strong> close all web browser tabs to complete the logout process.</p>
</body>
</html>
)rawliteral";

#endif

