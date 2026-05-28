#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DHTTYPE DHT22

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(DHT_PIN, DHTTYPE);
Servo radarServo;

int risco = 0;

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void connectMQTT() {
  while (!client.connected()) {
    String clientId = "ESP32-Sentinel-" + String(random(1000));

    if (client.connect(clientId.c_str())) {
      client.publish(TOPIC_STATUS, "Posto online");
    } else {
      delay(2000);
    }
  }
}

void setupDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void setColor(int r, int g, int b) {
  analogWrite(LED_R, r);
  analogWrite(LED_G, g);
  analogWrite(LED_B, b);
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

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }

  setupDisplay();

  connectWiFi();

  client.setServer(MQTT_BROKER, MQTT_PORT);
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }

  client.loop();

  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();
  int movimento = digitalRead(PIR_PIN);
  int ldr = analogRead(LDR_PIN);
  int gas = analogRead(MQ2_PIN);

  risco = 0;

  if (movimento == HIGH && ldr < 1000) {
    risco = 1;
  }

  if (gas > 2000 || temperatura > 40) {
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
    client.publish(TOPIC_ALERTA, "Movimento suspeito detectado");
  }

  if (risco == 2) {
    setColor(255, 0, 0);
    tone(BUZZER_PIN, 1000);
    radarServo.write(180);
    client.publish(TOPIC_ALERTA, "ALERTA CRITICO");
  }

  String payload = "{";
  payload += "\"temperatura\":" + String(temperatura) + ",";
  payload += "\"umidade\":" + String(umidade) + ",";
  payload += "\"movimento\":" + String(movimento) + ",";
  payload += "\"luminosidade\":" + String(ldr) + ",";
  payload += "\"gas\":" + String(gas) + ",";
  payload += "\"risco\":" + String(risco);
  payload += "}";

  client.publish(TOPIC_TELEMETRIA, payload.c_str());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("SentinelPost");

  display.print("Temp: ");
  display.println(temperatura);

  display.print("Umid: ");
  display.println(umidade);

  display.print("Luz: ");
  display.println(ldr);

  display.print("Gas: ");
  display.println(gas);

  display.print("Risco: ");

  if (risco == 0) {
    display.println("BAIXO");
  }

  if (risco == 1) {
    display.println("MEDIO");
  }

  if (risco == 2) {
    display.println("ALTO");
  }

  display.display();

  delay(2000);
}