// -----------------------------------------------------------
//  Projeto: Monitoramento de Temperatura e Umidade via MQTT
//  Dispositivo: ESP32
//  Sensor: DHT22
//  Autor: Felipe
//  Funções principais:
//    - Ler temperatura e umidade
//    - Enviar dados via MQTT
//    - Enviar alerta somente 1 vez quando ultrapassar limite
// -----------------------------------------------------------

// Bibliotecas necessárias
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"

// -----------------------------------------------------------
// CONFIGURAÇÕES DO SENSOR
// -----------------------------------------------------------
const int DHT_PIN = 15;
DHTesp dht;

// -----------------------------------------------------------
// CONFIGURAÇÕES DE REDE (Wokwi não precisa mudar nada)
// -----------------------------------------------------------
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// -----------------------------------------------------------
// CONFIGURAÇÕES DO BROKER MQTT
// -----------------------------------------------------------
const char* mqtt_server = "mqtt-wokwi.c3.wokwi.com";
WiFiClient espClient;
PubSubClient client(espClient);

// Tópicos usados no seu projeto
const char* TOPICO_LEITURAS = "sensor/leituras";
const char* TOPICO_ALERTAS = "sensor/alertas";

// -----------------------------------------------------------
// LIMITES (você pode alterar manualmente)
// -----------------------------------------------------------
float LIMITE_TEMP = 30.0;
float LIMITE_UMID = 60.0;

// Controle para evitar alertas repetidos
bool alertaTempAtivo = false;
bool alertaUmidAtivo = false;

// -----------------------------------------------------------
// Conecta ao WiFi
// -----------------------------------------------------------
void setupWifi() {
  delay(10);
  Serial.println("\nConectando ao WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
}

// -----------------------------------------------------------
// Callback MQTT (caso você adicione configuração remota)
// -----------------------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);
}

// -----------------------------------------------------------
// Conecta ao broker MQTT
// -----------------------------------------------------------
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");

    if (client.connect("esp32-client-felipe")) {
      Serial.println(" conectado!");
    } else {
      Serial.print(" falhou. Erro = ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// -----------------------------------------------------------
// SETUP
// -----------------------------------------------------------
void setup() {
  Serial.begin(115200);

  dht.setup(DHT_PIN, DHTesp::DHT22);

  setupWifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// -----------------------------------------------------------
// LOOP PRINCIPAL
// -----------------------------------------------------------
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Ler dados do DHT22
  TempAndHumidity data = dht.getTempAndHumidity();
  float temperatura = data.temperature;
  float umidade = data.humidity;

  // ---------------------------------------------
  // ENVIO DAS LEITURAS (MQTT)
  // ---------------------------------------------
  String payload_leituras = 
    String("{\"temperatura\":") + temperatura +
    ",\"umidade\":" + umidade +
    ",\"timestamp\":" + String(millis()) +
    "}";

  client.publish(TOPICO_LEITURAS, payload_leituras.c_str());
  Serial.println("Leitura enviada: " + payload_leituras);

  // ---------------------------------------------
  // LÓGICA DE ALERTA DE TEMPERATURA
  // ---------------------------------------------
  if (temperatura > LIMITE_TEMP) {
    if (!alertaTempAtivo) {
      // Só envia se NÃO estava em alerta antes
      String alerta = 
        String("{\"tipo\":\"temperatura_alta\",\"valor\":") + temperatura +
        ",\"limite\":" + LIMITE_TEMP +
        "}";

      client.publish(TOPICO_ALERTAS, alerta.c_str());
      Serial.println("Alerta enviado: " + alerta);

      alertaTempAtivo = true;
    }
  } else {
    alertaTempAtivo = false;  // reset do alerta
  }

  // ---------------------------------------------
  // LÓGICA DE ALERTA DE UMIDADE
  // ---------------------------------------------
  if (umidade > LIMITE_UMID) {
    if (!alertaUmidAtivo) {
      String alerta = 
        String("{\"tipo\":\"umidade_alta\",\"valor\":") + umidade +
        ",\"limite\":" + LIMITE_UMID +
        "}";

      client.publish(TOPICO_ALERTAS, alerta.c_str());
      Serial.println("Alerta enviado: " + alerta);

      alertaUmidAtivo = true;
    }
  } else {
    alertaUmidAtivo = false;
  }

  delay(2000);  // intervalo de leitura
}
