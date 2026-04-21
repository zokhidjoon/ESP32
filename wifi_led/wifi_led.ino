#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <WiFi.h>
#include <WebServer.h>

// ─── Config ───────────────────────────────────────
const char* ssid     = "NARGIZA";
const char* password = "1122334455";

const int LED_PIN = 13;
bool ledState = false;

WebServer server(80);

// ─── HTML Page ────────────────────────────────────
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>LED Control</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      background: #ffffff;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      height: 100vh;
      font-family: sans-serif;
      gap: 24px;
    }
    p {
      font-size: 13px;
      color: #aaa;
      letter-spacing: 0.08em;
      text-transform: uppercase;
    }
    button {
      width: 120px;
      height: 120px;
      border-radius: 50%;
      border: 2px solid #e0e0e0;
      background: #f0f0f0;
      color: #999;
      font-size: 16px;
      font-weight: 600;
      cursor: pointer;
      letter-spacing: 0.06em;
      transition: all 0.2s;
    }
    button.on {
      background: #1a1a1a;
      border-color: #1a1a1a;
      color: #fff;
    }
  </style>
</head>
<body>
  <p id="label">LED OFF</p>
  <button id="btn" onclick="toggle()">OFF</button>

  <script>
    let state = false;

    async function toggle() {
      const res = await fetch('/toggle');
      const json = await res.json();
      state = json.state;
      document.getElementById('btn').className = state ? 'on' : '';
      document.getElementById('btn').textContent = state ? 'ON' : 'OFF';
      document.getElementById('label').textContent = state ? 'LED ON' : 'LED OFF';
    }

    // Sync state on page load
    fetch('/state')
      .then(r => r.json())
      .then(json => {
        state = json.state;
        document.getElementById('btn').className = state ? 'on' : '';
        document.getElementById('btn').textContent = state ? 'ON' : 'OFF';
        document.getElementById('label').textContent = state ? 'LED ON' : 'LED OFF';
      });
  </script>
</body>
</html>
)rawliteral";

// ─── Routes ───────────────────────────────────────
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleToggle() {
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);
  String json = "{\"state\":" + String(ledState ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

void handleState() {
  String json = "{\"state\":" + String(ledState ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

// ─── Setup ────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());  // <-- open this IP in your browser

  server.on("/",       handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/state",  handleState);
  server.begin();
}

// ─── Loop ─────────────────────────────────────────
void loop() {
  server.handleClient();
}
