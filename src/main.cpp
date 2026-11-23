#include <Arduino.h>
#include <WiFi.h>
#include "internet.h"

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
const char *mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char *mqtt_client_id = "senai134_esp_kaua";
const char *mqtt_topic_sub = "senai134/DevKp/pub";
const char *mqtt_topic_pub = "senai_duoKP/pub";

WiFiClient espClient;
PubSubClient client(espClient);

void conectaMqtt();
void retornoMqtt(char *topic, byte *payload, unsigned int length);

#define pinDHT 4
#define tipoDHT DHT22
#define pinLDR 32
DHT medidor(pinDHT, tipoDHT);
float tempC_anterior = 0.0;
float umidade_anterior = 0.0;
unsigned long tempoAnterior = 0;
const long intervaloEnvio = 3000;

void setup()
{
  Serial.begin(115200);
  medidor.begin();
  pinMode(pinLDR, INPUT); // entradinha
  conexaoWifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(retornoMqtt);
}

void loop()
{
  reconexaoWifi();
  // aq faz smp ficar conectado
  if (!client.connected())
  {
    conectaMqtt();
  }
  client.loop();
  // le os valor do ldr e dht
  unsigned long tempoAtual = millis();
  if (tempoAtual - tempoAnterior >= intervaloEnvio)
  {
    tempoAnterior = tempoAtual;
    float tempC = medidor.readTemperature();
    float umi = medidor.readHumidity();

    int leituraLDR = analogRead(pinLDR);
    int luminosidade = map(leituraLDR, 0, 4095, 0, 100);

    client.loop();

    // ------------- lê da serial
    if (Serial.available() > 0)
    {
      String textoDigitado = Serial.readStringUntil('\n');
      textoDigitado.trim();

      if (textoDigitado.length() == 0)
      {
        Serial.println("Mensagem vazia");
        return;
      }

      // -------------- montar o JSON
      JsonDocument doc;
      doc["disp"] = "esp-kaua";
      doc["tempC"] = tempC;
      doc["umidade"] = umi;
      doc["luminosidade"] = luminosidade;

      String StringJson;
      serializeJson(doc, StringJson);

      // publica no MQTT
      client.publish(mqtt_topic_pub, StringJson.c_str());

      Serial.print("Enviado: ");
      Serial.println(StringJson);
    }
    tempC_anterior = tempC;
    umidade_anterior = umi;
  }
}
void conectaMqtt()
{
  while (!client.connected())
  {
    Serial.println("Conectando ao MQTT");
    if (client.connect(mqtt_client_id))
    {
      Serial.println("Conectado com Sucesso!");
      client.subscribe(mqtt_topic_sub);
    }
    else
    {
      Serial.println("Não houve Conexão");
      Serial.println(client.state());
      Serial.println("Tentando fazer a reconexão novamente");
      delay(5000);
    }
  }
}
void retornoMqtt(char *topic, byte *payload, unsigned int lenght)
{
  Serial.print("Mensagem recebida em ");
  Serial.print(topic);
  Serial.print(": ");

  String msgRecebida = "";
  for (int i = 0; i < lenght; i++)
  {
    msgRecebida += (char)payload[i];
  }

  Serial.println(msgRecebida);

  // ----------interpretar o JSON
  JsonDocument doc;
  DeserializationError erro = deserializeJson(doc, msgRecebida);
  if (erro)
  {
    Serial.print("erro ao decodificar o JSON: ");
    Serial.println(erro.c_str());
    return;
  }

  //------------------acessar os campos do JSON
  const char *dispositivo = doc["disp"];
  float tempC = doc["temp"];
  float umi = doc["umidade"];
  int lum = doc["luminosidade"];
  Serial.println("dados decodificados");
  Serial.print("esp: ");
  Serial.println(dispositivo);
  Serial.print("Temp: ");
  Serial.println(tempC);
  Serial.print("Umidade: ");
  Serial.println(umi);
  Serial.print("Luminosidade: ");
  Serial.println(lum);

  Serial.println("-------------------------------");
}