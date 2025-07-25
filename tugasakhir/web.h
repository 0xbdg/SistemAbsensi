#ifndef WEB_H
#define WEB_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
  <title>Panel Konfigurasi</title>
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
    <h2>Konfigurasi Alat</h2>

    <div class="section">
      <h3>Device</h3>
      <form id="host" method="POST" action="/save-device">
        <label for="deviceName">Nama</label>
        <input type="text" id="device" name="device" value="%DEVICE%" pattern="^[a-zA-Z0-9]([a-zA-Z0-9\-]*[a-zA-Z0-9])?$" title="Hanya huruf, angka, dan tanda minus (-). Tidak diawali/diakhiri tanda minus. Tidak boleh spasi atau titik." required>
        <div class="current-value">Saat ini: %DEVICE%</div>

        <input type="submit" value="Simpan perangkat">
      </form>
    </div>

    <div class="section">
      <h3>WiFi</h3>
      <form method="POST" action="/save-wifi">
        <label for="botToken">SSID</label>
        <input type="text" id="username" name="ssid" value="%SSID%" required>
        <div class="current-value">Saat ini: %SSID%</div>

        <label for="chatId">WiFi Password</label>
        <input type="text" id="password" name="password" value="%PASS%" required>
        <div class="current-value">Saat ini: %PASS%</div>

        <input type="submit" value="Simpan Kredensial">
      </form>
    </div>

    <div class="section">
      <h3>Admin Auth</h3>
      <form method="POST" action="/save-admin">
        <label for="botToken">Admin Username</label>
        <input type="text" id="username" name="username" value="%USERNAME%" required>
        <div class="current-value">Saat ini: %USERNAME%</div>

        <label for="chatId">Admin Password</label>
        <input type="text" id="password" name="password" value="%USER_PASS%" required>
        <div class="current-value">Saat ini: %USER_PASS%</div>

        <input type="submit" value="Simpan Kredensial">
      </form>
    </div>
 
    <div class="section">
      <h3>Telegram</h3>
      <form method="POST" action="/save-telegram">
        <label for="botToken">Bot Token</label>
        <input type="text" id="botToken" name="botToken" value="%BOT_TOKEN%" required>
        <div class="current-value">Saat ini: %BOT_TOKEN%</div>

        <label for="chatId">Chat ID</label>
        <input type="text" id="chatId" name="chatId" value="%CHAT_ID%" required>
        <div class="current-value">Saat ini: %CHAT_ID%</div>

        <input type="submit" value="Simpan Telegram">
      </form>
    </div>

    <div class="section">
      <h3>Google Spreadsheet</h3>
      <form method="POST" action="/save-spreadsheet">
        <label for="sheetUrl">App Script URL</label>
        <input type="text" id="sheetUrl" name="sheetUrl" value="%SHEET_URL%" placeholder="Ex: https://script.google.com/macros/s/[ID]/exec" required>
        <div class="current-value">Saat ini: %SHEET_URL%</div>

        <label for="sheetApiKey">Sheet</label>
        <input type="text" id="sheetApiKey" name="sheetName" value="%SHEET_NAME%" placeholder="Ex: Sheet1" required>
        <div class="current-value">Saat ini: %SHEET_NAME%</div>

        <input type="submit" value="Simpan Spreadsheet">
      </form>
    </div>

    <div class="section">
      <h3>API Endpoint</h3>
      <form method="POST" action="/save-api">
        <label for="apiUrl">Endpoint URL</label>
        <input type="text" id="apiUrl" name="apiUrl" value="%API_ENDPOINT%" placeholder="Ex: https://example.com/data/siswa" required>
        <div class="current-value">Saat ini: %API_ENDPOINT%</div>

        <input type="submit" value="Simpan API">
      </form>
    </div>

  </div>
  <footer class="footer">
    <p>&copy; 2025 Benjamin Dharma Guntara. All rights reserved.</p>
  </footer>
  <script>
        const form = document.getElementById('host');
        const input = document.getElementById('device');

        form.addEventListener('submit', function (e) {
          const value = input.value;

          if (/--/.test(value) || /\s/.test(value) || /\./.test(value)) {
            e.preventDefault(); 
            return;
          }
        });
  </script>
</body>
</html>
)rawliteral";

#endif

