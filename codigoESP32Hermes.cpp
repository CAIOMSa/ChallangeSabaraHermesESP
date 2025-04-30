#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ======= OLED CONFIG =======
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C // Endereço padrão I2C do display

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ======= WIFI CONFIG =======
const char* ssid = "MP2C_ROT";
const char* password = "MP2c2927";

const char* serverBase = "http://192.168.5.161:8000/notificacao/notif?id=1";

String getMessage() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverBase);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        const char* message = doc["message"];
        return String(message);
      } else {
        Serial.println("Erro no parse JSON");
      }
    } else {
      Serial.print("Erro GET: ");
      Serial.println(httpCode);
    }

    http.end();
  }
  return "";
}

void exibeOLED(const String& message) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Mensagem:");
  display.setCursor(0, 20);
  display.println(message);
  display.display();
}

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("Falha ao iniciar o OLED"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Conectando WiFi...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi conectado!");
  display.display();
  delay(1000);
}

void loop() {
  if (Serial.available() > 0) {
    char resposta = Serial.read();

    if (resposta == 'l') {
      String respos = getMessage();
      if (respos != "") {
        exibeOLED(respos);
        delay(10000);

        StaticJsonDocument<100> json;
        exibeOLED("ligado...");
        json["id"] = 1;
        json["mensagem"] = respos;
        serializeJson(json, Serial);
        Serial.println();
      }

    } else if (resposta == 't') {
      exibeOLED("Teste");
    }
  }
}
