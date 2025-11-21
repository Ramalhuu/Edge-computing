// Bibliotecas necessárias
#include <WiFi.h>           // Conexão Wi-Fi
#include <PubSubClient.h>   // Comunicação MQTT
#include <DHTesp.h>         // Leitura do sensor DHT22
#include <ArduinoJson.h>    // Criação e leitura de JSON

// Credenciais do WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Dados do servidor MQTT
const char* mqtt_server = "44.223.43.74";
const int   mqtt_port   = 1883;
const char* mqtt_user   = "";
const char* mqtt_pass   = "";

// Tópicos MQTT utilizados
const char* TOPICO_SENSORES = "esp32/sensores";
const char* TOPICO_COMANDOS = "esp32/comandos";
const char* TOPICO_ALERTAS  = "esp32/alertas";

// Objetos principais de comunicação
WiFiClient espClient;       // Cliente Wi-Fi
PubSubClient client(espClient); // Cliente MQTT
DHTesp dht;                 // Objeto do sensor DHT

// Definição dos pinos utilizados no ESP32
const int pinDHT         = 14;
const int pinLDR         = 34;
const int pinPIR         = 27;
const int pinLED         = 26;
const int pinLEDInclusao = 32;
const int pinBuzzer      = 25;
const int pinRele        = 33;

// Controle do modo de inclusão
bool modoInclusao = false;

// Variáveis para detecção de ausência prolongada
unsigned long ultimoMovimento = 0;
bool alertaMovimentoEnviado = false;
const unsigned long TEMPO_SEM_MOVIMENTO = 600000; // 10 minutos

// Flags para evitar repetição de alertas
bool alertaTempHumEnviado = false;
bool alertaLuzEnviado = false;

// Função que ativa um alerta visual/sonoro
void ativarAlerta() {
  digitalWrite(pinLED, HIGH);
  tone(pinBuzzer, 1200, 400);
  delay(200);
  tone(pinBuzzer, 900, 400);
  delay(200);
  digitalWrite(pinLED, LOW);
}

// Ativa LED do modo inclusão
void ativarInclusao() {
  digitalWrite(pinLEDInclusao, HIGH);
}

// Desativa LED do modo inclusão
void desativarInclusao() {
  digitalWrite(pinLEDInclusao, LOW);
}

// Função chamada sempre que chega uma mensagem MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  // Converte o payload recebido em string
  String msg = "";
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  // Prepara JSON para leitura
  StaticJsonDocument<200> doc;
  if (deserializeJson(doc, msg)) return; // Se der erro, ignora

  // Comando para LED
  if (doc.containsKey("led")) {
    String cmd = doc["led"];
    digitalWrite(pinLED, cmd == "on" ? HIGH : LOW);
  }

  // Comando para buzzer
  if (doc.containsKey("buzzer")) {
    String cmd = doc["buzzer"];
    if (cmd == "on") tone(pinBuzzer, 1000, 500);
  }

  // Comando para relé
  if (doc.containsKey("rele")) {
    String cmd = doc["rele"];
    digitalWrite(pinRele, cmd == "on" ? HIGH : LOW);
  }

  // Comando para modo inclusão
  if (doc.containsKey("inclusao")) {
    String cmd = doc["inclusao"];
    modoInclusao = (cmd == "on");
    modoInclusao ? ativarInclusao() : desativarInclusao();
  }
}

// Reconecta ao MQTT caso caia
void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client12345")) {
      client.subscribe(TOPICO_COMANDOS); // Reinscreve no tópico de comandos
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Configuração dos pinos
  pinMode(pinLED, OUTPUT);
  pinMode(pinLEDInclusao, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinRele, OUTPUT);
  pinMode(pinPIR, INPUT);

  // Inicia o DHT22
  dht.setup(pinDHT, DHTesp::DHT22);

  // Conexão ao Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi conectado!");

  // Configuração do servidor MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  // Mantém conexão MQTT ativa
  if (!client.connected()) reconnect();
  client.loop();

  // Leitura dos sensores
  TempAndHumidity dados = dht.getTempAndHumidity();
  float temp = dados.temperature;
  float hum = dados.humidity;
  int lux = analogRead(pinLDR);
  int presence = digitalRead(pinPIR);

  // Se há movimento, reseta o contador de ausência
  if (presence == HIGH) {
    ultimoMovimento = millis();
    alertaMovimentoEnviado = false;
  }

  // Envia alerta de ausência prolongada
  if (presence == LOW && (millis() - ultimoMovimento > TEMPO_SEM_MOVIMENTO)) {
    if (!alertaMovimentoEnviado) {
      ativarAlerta();
      StaticJsonDocument<200> alerta;
      alerta["mensagem"] = "Ausência prolongada detectada — faça pausa ou ajuste postura.";
      String alertaJSON;
      serializeJson(alerta, alertaJSON);
      client.publish(TOPICO_ALERTAS, alertaJSON.c_str());
      alertaMovimentoEnviado = true;
    }
  }

  // Alerta de iluminação baixa
  if (lux < 500) {
    if (!alertaLuzEnviado) {
      ativarAlerta();
      StaticJsonDocument<200> alerta;
      alerta["mensagem"] = "Iluminação insuficiente! Ajuste a luz para evitar fadiga visual.";
      String alertaJSON;
      serializeJson(alerta, alertaJSON);
      client.publish(TOPICO_ALERTAS, alertaJSON.c_str());
      alertaLuzEnviado = true;
    }
  } else {
    alertaLuzEnviado = false;
  }

  // Alerta de temperatura quente ou umidade baixa
  if (temp > 28 || hum < 35) {
    if (!alertaTempHumEnviado) {
      ativarAlerta();
      StaticJsonDocument<200> alerta;
      alerta["mensagem"] = "Ambiente quente ou muito seco! Faça pausa, beba água ou ajuste ventilação.";
      String alertaJSON;
      serializeJson(alerta, alertaJSON);
      client.publish(TOPICO_ALERTAS, alertaJSON.c_str());
      alertaTempHumEnviado = true;
    }
  } else {
    alertaTempHumEnviado = false;
  }

  // Mantém LED de inclusão ligado enquanto ativo
  if (modoInclusao) ativarInclusao();

  // Envia dados dos sensores via MQTT
  StaticJsonDocument<256> doc;
  doc["temp"] = temp;
  doc["hum"] = hum;
  doc["lux"] = lux;
  doc["presence"] = presence;

  String json;
  serializeJson(doc, json);

  client.publish(TOPICO_SENSORES, json.c_str());
  Serial.println(json);

  delay(2000); // Intervalo entre leituras/envio
}
