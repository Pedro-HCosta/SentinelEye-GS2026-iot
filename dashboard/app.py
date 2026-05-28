import time
import pandas as pd
import streamlit as st

from sensor_data import SensorData
from mqtt_client import MQTTClient

MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT = 1883
MQTT_TOPIC = "sentineleye/posto01/telemetria"

st.set_page_config(
    page_title="SentinelPost | IoT Monitoring",
    page_icon="🛰️",
    layout="wide"
)

st.markdown("""
<style>
.stApp {
    background: radial-gradient(circle at top, #10263f 0%, #06111f 45%, #020812 100%);
    color: #e5f4ff;
}

.block-container {
    padding-top: 2rem;
}

[data-testid="stMetric"] {
    background: rgba(9, 28, 48, 0.92);
    border: 1px solid rgba(0, 195, 255, 0.25);
    border-radius: 16px;
    padding: 18px;
    box-shadow: 0 0 18px rgba(0, 195, 255, 0.08);
}

.sentinel-header {
    background: linear-gradient(90deg, rgba(0, 195, 255, 0.20), rgba(24, 255, 178, 0.08));
    border: 1px solid rgba(0, 195, 255, 0.35);
    border-radius: 20px;
    padding: 24px;
    margin-bottom: 24px;
}

.sentinel-title {
    font-size: 34px;
    font-weight: 800;
    color: #d8f3ff;
}

.sentinel-subtitle {
    color: #9edfff;
    font-size: 16px;
}

.section-title {
    margin-top: 28px;
    margin-bottom: 12px;
    color: #d8f3ff;
    font-size: 22px;
    font-weight: 700;
}

.status-card {
    border-radius: 18px;
    padding: 22px;
    margin-bottom: 24px;
    font-size: 18px;
    font-weight: 600;
}

.status-low {
    background: rgba(18, 120, 75, 0.25);
    border: 1px solid rgba(63, 255, 169, 0.45);
    color: #b9ffd9;
}

.status-medium {
    background: rgba(176, 126, 0, 0.25);
    border: 1px solid rgba(255, 203, 74, 0.55);
    color: #ffe4a3;
}

.status-high {
    background: rgba(180, 32, 32, 0.30);
    border: 1px solid rgba(255, 86, 86, 0.65);
    color: #ffc4c4;
}
</style>
""", unsafe_allow_html=True)

if "sensor_data" not in st.session_state:
    st.session_state.sensor_data = SensorData()

if "mqtt_started" not in st.session_state:
    mqtt_client = MQTTClient(
        MQTT_BROKER,
        MQTT_PORT,
        MQTT_TOPIC,
        st.session_state.sensor_data
    )

    mqtt_client.start()

    st.session_state.mqtt_client = mqtt_client
    st.session_state.mqtt_started = True

st.markdown("""
<div class="sentinel-header">
    <div class="sentinel-title">SentinelPost — Estação IoT de Vigilância Ambiental</div>
    <div class="sentinel-subtitle">
        Monitoramento de borda com ESP32, sensores ambientais e alertas via MQTT.
    </div>
</div>
""", unsafe_allow_html=True)

data = st.session_state.sensor_data.to_dict()
status = st.session_state.sensor_data.get_status()

col1, col2, col3, col4 = st.columns(4)

with col1:
    st.metric("Temperatura", f"{data['temperatura']} °C" if data["temperatura"] is not None else "Aguardando")

with col2:
    st.metric("Umidade", f"{data['umidade']} %" if data["umidade"] is not None else "Aguardando")

with col3:
    st.metric("Luminosidade", f"{data['luminosidade']} %" if data["luminosidade"] is not None else "Aguardando")

with col4:
    st.metric("Gás/Fumaça", f"{data['gas']} %" if data["gas"] is not None else "Aguardando")

col5, col6, col7, col8 = st.columns(4)

with col5:
    movimento = "Detectado" if data["movimento"] == 1 else "Não detectado"
    st.metric("Movimento", movimento if data["movimento"] is not None else "Aguardando")

with col6:
    st.metric("Pressão", f"{data['pressao']} hPa" if data["pressao"] is not None else "Aguardando")

with col7:
    st.metric("Altitude", f"{data['altitude']} m" if data["altitude"] is not None else "Aguardando")

with col8:
    st.metric("Nível de risco", data["risco_texto"] if data["risco_texto"] is not None else "Aguardando")

st.markdown('<div class="section-title">Status operacional</div>', unsafe_allow_html=True)

if data["risco"] == 2:
    css_class = "status-card status-high"
elif data["risco"] == 1:
    css_class = "status-card status-medium"
else:
    css_class = "status-card status-low"

st.markdown(f'<div class="{css_class}">{status}</div>', unsafe_allow_html=True)

st.markdown('<div class="section-title">Última atualização</div>', unsafe_allow_html=True)
st.write(data["ultima_atualizacao"] if data["ultima_atualizacao"] else "Nenhum dado recebido ainda.")

history = st.session_state.sensor_data.history

if history:
    df = pd.DataFrame(history)
    df["timestamp"] = pd.to_datetime(df["timestamp"])
    df = df.set_index("timestamp")

    st.markdown('<div class="section-title">Análise temporal dos sensores</div>', unsafe_allow_html=True)

    tab1, tab2, tab3, tab4 = st.tabs([
        "Ambiente",
        "Risco",
        "Atmosfera",
        "Histórico"
    ])

    with tab1:
        col_a, col_b = st.columns(2)

        with col_a:
            st.subheader("Temperatura")
            st.line_chart(df["temperatura"])

            st.subheader("Umidade")
            st.line_chart(df["umidade"])

        with col_b:
            st.subheader("Luminosidade")
            st.line_chart(df["luminosidade"])

            st.subheader("Gás/Fumaça")
            st.line_chart(df["gas"])

    with tab2:
        st.subheader("Evolução do nível de risco")
        st.line_chart(df["risco"])

        st.subheader("Movimento detectado")
        st.bar_chart(df["movimento"])

    with tab3:
        st.subheader("Pressão atmosférica")
        st.line_chart(df["pressao"])

        st.subheader("Altitude aproximada")
        st.line_chart(df["altitude"])

    with tab4:
        st.dataframe(df, use_container_width=True)

else:
    st.warning("Aguardando dados do ESP32 via MQTT...")

time.sleep(2)
st.rerun()