#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP085.h>

#include "config.h"

#define DHTTYPE DHT22

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(DHT_PIN, DHTTYPE);

Servo radarServo;

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

Adafruit_BMP085 bmp;

int risco = 0;

void connectWiFi() {

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println("\nWiFi conectado");
}

void connectMQTT() {

  while (!client.connected()) {

    String clientId =
      "ESP32-SentinelPost-" +
      String(random(1000, 9999));

    if (client.connect(clientId.c_str())) {

      Serial.println("MQTT conectado");

      client.publish(
        TOPIC_STATUS,
        "SentinelPost online"
      );

    } else {

      Serial.print("Falha MQTT. Codigo: ");

      Serial.println(client.state());

      delay(2000);
    }
  }
}

void setColor(int r, int g, int b) {

  analogWrite(LED_R, r);
  analogWrite(LED_G, g);
  analogWrite(LED_B, b);
}

String riscoTexto(int risco) {

  if (risco == 0) return "BAIXO";

  if (risco == 1) return "MEDIO";

  return "ALTO";
}

void setup() {

  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  dht.begin();

  radarServo.attach(SERVO_PIN);

  radarServo.write(90);

  Wire.begin(
    OLED_SDA,
    OLED_SCL
  );

  if (!display.begin(
    SSD1306_SWITCHCAPVCC,
    0x3C
  )) {

    Serial.println("Falha OLED");

    while (true);
  }

  if (!bmp.begin()) {

    Serial.println("BMP180 nao encontrado");
  }

  display.clearDisplay();

  display.setTextSize(1);

  display.setTextColor(SSD1306_WHITE);

  connectWiFi();

  client.setServer(
    MQTT_BROKER,
    MQTT_PORT
  );
}

void loop() {

  if (!client.connected()) {

    connectMQTT();
  }

  client.loop();

  float temperatura =
    dht.readTemperature();

  float umidade =
    dht.readHumidity();

  if (isnan(temperatura)) {

    temperatura = 0;
  }

  if (isnan(umidade)) {

    umidade = 0;
  }

  int movimento =
    digitalRead(PIR_PIN);

  int leituraLdr =
    analogRead(LDR_PIN);

  int luminosidade =
    map(leituraLdr, 0, 4095, 100, 0);

  int leituraGas =
    analogRead(MQ2_PIN);

  int gas =
    map(leituraGas, 0, 4095, 0, 100);

  float pressao =
    bmp.readPressure() / 100.0;

  float altitude =
    bmp.readAltitude();

  risco = 0;

  if (
    movimento == HIGH &&
    luminosidade < 30
  ) {

    risco = 1;
  }

  if (
    gas > 60 ||
    temperatura > 40
  ) {

    risco = 2;
  }

  if (risco == 0) {

    setColor(0, 255, 0);

    noTone(BUZZER_PIN);

    radarServo.write(90);
  }

  if (risco == 1) {

    setColor(255, 255, 0);

    noTone(BUZZER_PIN);

    radarServo.write(45);

    client.publish(
      TOPIC_ALERTA,
      "Movimento suspeito detectado"
    );
  }

  if (risco == 2) {

    setColor(255, 0, 0);

    tone(BUZZER_PIN, 1000);

    radarServo.write(180);

    client.publish(
      TOPIC_ALERTA,
      "ALERTA CRITICO"
    );
  }

  String payload = "{";

  payload +=
    "\"temperatura\":" +
    String(temperatura, 1) + ",";

  payload +=
    "\"umidade\":" +
    String(umidade, 1) + ",";

  payload +=
    "\"movimento\":" +
    String(movimento) + ",";

  payload +=
    "\"luminosidade\":" +
    String(luminosidade) + ",";

  payload +=
    "\"gas\":" +
    String(gas) + ",";

  payload +=
    "\"pressao\":" +
    String(pressao, 2) + ",";

  payload +=
    "\"altitude\":" +
    String(altitude, 2) + ",";

  payload +=
    "\"risco\":" +
    String(risco);

  payload += "}";

  client.publish(
    TOPIC_TELEMETRIA,
    payload.c_str()
  );

  Serial.println(payload);

  display.clearDisplay();

  display.setCursor(0, 0);

  display.println("SentinelPost");

  display.print("Temp: ");
  display.print(temperatura);
  display.println(" C");

  display.print("Umid: ");
  display.print(umidade);
  display.println(" %");

  display.print("Luz: ");
  display.print(luminosidade);
  display.println(" %");

  display.print("Gas: ");
  display.print(gas);
  display.println(" %");

  display.print("Press: ");
  display.println(pressao);

  display.print("Risco: ");
  display.println(riscoTexto(risco));

  display.display();

  delay(2000);
}