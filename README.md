# ESP32 Monitor Ambiental via MQTT (DHT22 + LDR)

Projeto em ESP32 para leitura de **temperatura**, **umidade** e **luminosidade**, com envio dos dados em formato **JSON** através do protocolo **MQTT** e recepção de mensagens de outros dispositivos.

---

## ⚙️ Funcionalidades

- **Leitura de Sensores:** Coleta temperatura (°C) e umidade (%) com o sensor **DHT22**, além da luminosidade em porcentagem (0% a 100%) usando um **LDR**.
- **Publicação via MQTT:** Envia os dados estruturados em JSON para o broker `broker.emqx.io` sob demanda (ao digitar qualquer comando no Monitor Serial dentro do ciclo de 3 segundos).
- **Recepção e Decodificação:** Escuta um tópico MQTT dedicado, recebe payloads JSON de outros ESPs e exibe os dados decodificados no Monitor Serial.
- **Reconexão Automática:** Mantém a conexão Wi-Fi e MQTT ativa continuamente.

---

## 🔌 Pinagem (Hardware)

| Componente | Pino no ESP32 | Tipo |
| :--- | :--- | :--- |
| **Sensor DHT22** | GPIO `4` | Digital |
| **Sensor LDR** | GPIO `32` | Analógico (`0–4095` mapeado para `0–100%`) |

---

## 📡 Configuração MQTT

| Parâmetro | Valor |
| :--- | :--- |
| **Broker** | `broker.emqx.io` |
| **Porta** | `1883` |
| **Client ID** | `senai134_esp_kaua` |
| **Subscribe (Recebe)** | `senai134/DevKp/pub` |
| **Publish (Envia)** | `senai_duoKP/pub` |

---

## 📦 Formato do JSON Enviado

```json
{
  "disp": "esp-kaua",
  "tempC": 25.4,
  "umidade": 60.2,
  "luminosidade": 78
}
