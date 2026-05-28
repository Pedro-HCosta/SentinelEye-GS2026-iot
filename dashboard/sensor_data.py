from datetime import datetime


class SensorData:
    def __init__(self):
        self.temperature = None
        self.humidity = None
        self.movement = None
        self.luminosity = None
        self.gas = None
        self.pressure = None
        self.altitude = None
        self.risk = None
        self.last_update = None
        self.history = []

    def update(self, temperature, humidity, movement, luminosity, gas, pressure, altitude, risk):
        self.temperature = temperature
        self.humidity = humidity
        self.movement = movement
        self.luminosity = luminosity
        self.gas = gas
        self.pressure = pressure
        self.altitude = altitude
        self.risk = risk
        self.last_update = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        self.history.append({
            "timestamp": self.last_update,
            "temperatura": temperature,
            "umidade": humidity,
            "movimento": movement,
            "luminosidade": luminosity,
            "gas": gas,
            "pressao": pressure,
            "altitude": altitude,
            "risco": risk,
            "risco_texto": self.get_risk_text()
        })

        if len(self.history) > 100:
            self.history.pop(0)

    def get_risk_text(self):
        if self.risk == 0:
            return "Baixo"
        if self.risk == 1:
            return "Médio"
        if self.risk == 2:
            return "Alto"
        return None

    def to_dict(self):
        return {
            "temperatura": self.temperature,
            "umidade": self.humidity,
            "movimento": self.movement,
            "luminosidade": self.luminosity,
            "gas": self.gas,
            "pressao": self.pressure,
            "altitude": self.altitude,
            "risco": self.risk,
            "risco_texto": self.get_risk_text(),
            "ultima_atualizacao": self.last_update
        }

    def get_status(self):
        alerts = []

        if self.risk == 2:
            alerts.append("ALERTA CRÍTICO")
        elif self.risk == 1:
            alerts.append("Atenção: possível atividade suspeita")
        elif self.risk == 0:
            alerts.append("Área monitorada dentro do padrão")

        if self.movement == 1:
            alerts.append("Movimento detectado")

        if self.temperature is not None:
            if self.temperature >= 40:
                alerts.append("Temperatura crítica")
            elif self.temperature >= 30:
                alerts.append("Temperatura elevada")

        if self.humidity is not None:
            if self.humidity >= 75:
                alerts.append("Umidade elevada")
            elif self.humidity <= 30:
                alerts.append("Umidade baixa")

        if self.luminosity is not None:
            if self.luminosity <= 30:
                alerts.append("Baixa luminosidade")

        if self.gas is not None:
            if self.gas >= 60:
                alerts.append("Fumaça ou gás detectado")

        if self.pressure is not None:
            if self.pressure < 980:
                alerts.append("Pressão atmosférica baixa")
            elif self.pressure > 1030:
                alerts.append("Pressão atmosférica alta")

        return " | ".join(alerts)