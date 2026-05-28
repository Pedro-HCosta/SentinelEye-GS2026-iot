#ifndef CONFIG_H
#define CONFIG_H

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

const char* MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORT = 1883;

const char* TOPIC_TELEMETRIA = "sentineleye/posto01/telemetria";
const char* TOPIC_ALERTA = "sentineleye/posto01/alerta";
const char* TOPIC_STATUS = "sentineleye/posto01/status";

#define DHT_PIN 14
#define PIR_PIN 26
#define LDR_PIN 25
#define MQ2_PIN 12

#define SERVO_PIN 16

#define LED_R 19
#define LED_G 18
#define LED_B 5

#define BUZZER_PIN 22

#define OLED_SCL 4
#define OLED_SDA 2

#endif