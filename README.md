# SentinelEye GS2026
Posto de monitoramento de borda para detecção de atividades suspeitas e riscos ambientais em áreas vulneráveis e de fronteira.

## Objetivo
O projeto foi desenvolvido como proposta para a Global Solution 2026, cujo tema é a economia espacial e seu impacto em problemas reais aqui na Terra.

Inspirado na lógica de monitoramento contínuo dos satélites de previsão climática e vigilância ambiental, o SentinelEye atua como o nó físico de campo: enquanto satélites como o Copernicus (ESA) e o Landsat (NASA) identificam anomalias em escala macro, o posto de borda confirma e detalha o evento no solo com dados precisos de temperatura, fumaça, luminosidade e presença humana.

A solução simula um sensor de fronteira físico — ESP32 com sensores ambientais, comunicação MQTT e dashboard em tempo real — capaz de detectar movimentações suspeitas e condições de risco em regiões remotas e de difícil acesso.

## Estrutura do Projeto
```
sentineleye-gs2026-iot/
│
├── esp32/
│   ├── sketch.ino  
│   ├── libraries.txt
│   └── diagram.json
│
├── dashboard/
│   ├── app.py
│   ├── mqtt_client.py
│   ├── sensor_data.py
│   └── requirements.txt
│
└── README.md
```

## Módulo IoT
O módulo IoT simula um posto de monitoramento de borda utilizando ESP32 e sensores físicos.

- ESP32 DevKit V1
- DHT22
- LDR
- MQ-2
- PIR
- BMP180
- MQTT
- Dashboard em Streamlit

O sistema coleta:
- temperatura e umidade;
- luminosidade (detecção de operações noturnas);
- presença de fumaça e gases;
- movimentação suspeita;
- pressão atmosférica;

e transmite os dados em tempo real para o dashboard utilizando MQTT.

## Atuadores
- LED RGB para indicação visual de risco
- Buzzer para alerta sonoro
- Servo Motor para simulação de varredura
- Display OLED para exibição local de dados

## Níveis de Risco
- baixo — LED verde, operação normal
- médio — LED amarelo, movimentação suspeita em baixa luminosidade
- alto — LED vermelho, buzzer ativado, fumaça/gás detectado ou temperatura crítica

## Tópicos MQTT
- `sentineleye/posto01/telemetria`
- `sentineleye/posto01/alerta`
- `sentineleye/posto01/status`

## Tecnologias utilizadas
- Python
- Streamlit
- MQTT
- HiveMQ Broker
- ESP32
- DHT22
- MQ-2
- PIR
- BMP180
- LDR
- Arduino IDE
- Wokwi

# Como executar

## 1. Criar ambiente virtual
```
python -m venv venv
```

## 2. Ativar ambiente virtual
```
venv\Scripts\activate
```

## 3. Instalar dependências
```
pip install -r requirements.txt
```

## Executando o dashboard
```
cd dashboard
streamlit run app.py
```

## Simulação no Wokwi
O projeto pode ser simulado no Wokwi utilizando os arquivos `sketch.ino`, `diagram.json` e `libraries.txt`.

## Configuração Wi-Fi
No arquivo `sketch.ino`:
```
const char* WIFI_SSID     = "SUA_REDE";
const char* WIFI_PASSWORD = "SUA_SENHA";
```

## Estrutura de funcionamento
```
ESP32 + Sensores de borda
         ↓
       MQTT
         ↓
  Dashboard IoT
         ↓
Dados ambientais + Alertas
```

## Objetivos futuros
- Integração com dados abertos de satélite (Copernicus ESA / Landsat NASA)
- Cruzamento de alertas do posto com anomalias identificadas orbitalmente
- Suporte a múltiplos postos simultâneos
- Banco de dados histórico de ocorrências e coordenadas
- Dashboard geoespacial com mapa de alertas
- Modelos de IA para predição de risco com base em histórico
- Automação de respostas e acionamento remoto

## Integrantes
Pedro Henrique dos Santos Costa    — RM562156\
Eduardo Augusto de Oliveira Souza  — RM565269\
Fellipe Costa de Oliveira          — RM564673\
Felype Ferreira Maschio            — RM563009\
Gustavo Vieira de Matos            — RM563304

## Contexto Acadêmico
Projeto desenvolvido para a disciplina de IoT da FIAP como parte da Global Solution 2026, com foco em monitoramento ambiental de borda, prevenção de riscos e integração com ecossistemas de vigilância inteligente.