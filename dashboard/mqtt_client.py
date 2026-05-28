import json
import paho.mqtt.client as mqtt


class MQTTClient:
    def __init__(self, broker, port, topic, sensor_data):
        self.broker = broker
        self.port = port
        self.topic = topic
        self.sensor_data = sensor_data

        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message

    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            print("Conectado ao broker MQTT")
            client.subscribe(self.topic)
            print(f"Inscrito no tópico: {self.topic}")
        else:
            print(f"Falha na conexão MQTT. Código: {rc}")

    def on_message(self, client, userdata, message):
        try:
            payload = message.payload.decode("utf-8")
            data = json.loads(payload)

            temperatura = float(data.get("temperatura", 0))
            umidade = float(data.get("umidade", 0))
            movimento = int(data.get("movimento", 0))
            luminosidade = int(data.get("luminosidade", 0))
            gas = int(data.get("gas", 0))
            pressao = float(data.get("pressao", 0))
            altitude = float(data.get("altitude", 0))
            risco = int(data.get("risco", 0))

            self.sensor_data.update(
                temperatura,
                umidade,
                movimento,
                luminosidade,
                gas,
                pressao,
                altitude,
                risco
            )

            print("Dados recebidos:", data)

        except Exception as error:
            print("Erro ao processar mensagem MQTT:", error)

    def start(self):
        self.client.connect(self.broker, self.port, 60)
        self.client.loop_start()

    def stop(self):
        self.client.loop_stop()
        self.client.disconnect()