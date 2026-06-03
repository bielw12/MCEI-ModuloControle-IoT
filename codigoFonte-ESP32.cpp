#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// ─── Wi-Fi (Wokwi usa rede virtual) ──────────────────────────────
#define WIFI_SSID     "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// ─── MQTT ─────────────────────────────────────────────────────────
// ALTERAÇÃO 1: broker correto para o HiveMQ público
#define MQTT_BROKER     "broker.hivemq.com"
#define MQTT_PORT       1883          // TCP puro — NÃO é WebSocket
#define MQTT_TOPIC_PUB  "tcc/modulo1/sensores"
#define MQTT_TOPIC_CMD  "tcc/modulo1/cmd"

// ALTERAÇÃO 2: client ID com sufixo aleatório
// ID fixo causa colisão se duas instâncias rodarem ao mesmo tempo
char MQTT_CLIENT_ID[32];

// ─── Pinos ────────────────────────────────────────────────────────
#define DHT_PIN   4
#define LDR_PIN   34
#define VOLT_PIN  35
#define CURR_PIN  32

// ─── Calibração ADC ───────────────────────────────────────────────
#define DHT_TYPE     DHT22
#define ADC_MAX      4095.0
#define ADC_VREF     3.3
#define VOLT_SCALE   4.03
#define ACS_ZERO_ADC 3102
#define ACS_mV_PER_A 185.0
#define PUBLISH_MS   2000

DHT dht(DHT_PIN, DHT_TYPE);
WiFiClient   espClient;
PubSubClient mqtt(espClient);
unsigned long lastPublish = 0;
int leituraId = 0;

void connectWifi();
void connectMqtt();
void mqttCallback(char* topic, byte* payload, unsigned int length);
float lerTensao();
float lerCorrente();
float lerLuminosidade();

void setup() {
  Serial.begin(115200);
  delay(500);

  // Gera ID único — evita colisão de sessão no broker
  randomSeed(esp_random());
  snprintf(MQTT_CLIENT_ID, sizeof(MQTT_CLIENT_ID),
           "esp32_tcc_%08X", (uint32_t)random(0xFFFFFFFF));
  Serial.printf("[MQTT] Client ID: %s\n", MQTT_CLIENT_ID);

  pinMode(LDR_PIN, INPUT);
  pinMode(VOLT_PIN, INPUT);
  pinMode(CURR_PIN, INPUT);
  analogReadResolution(12);
  dht.begin();

  connectWifi();
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
  connectMqtt();
}

void loop() {
  if (!mqtt.connected()) connectMqtt();
  mqtt.loop();

  unsigned long agora = millis();
  if (agora - lastPublish >= PUBLISH_MS) {
    lastPublish = agora;

    float temperatura  = dht.readTemperature();
    float umidade      = dht.readHumidity();
    float tensao       = lerTensao();
    float corrente     = lerCorrente();
    float luminosidade = lerLuminosidade();

    if (isnan(temperatura) || isnan(umidade)) {
      Serial.println("[ERRO] Falha DHT22");
      return;
    }

    leituraId++;
    StaticJsonDocument<256> doc;
    doc["id"]       = leituraId;
    doc["ts"]       = agora;
    doc["temp"]     = round(temperatura  * 10) / 10.0;
    doc["umid"]     = round(umidade      * 10) / 10.0;
    doc["tensao"]   = round(tensao       * 100) / 100.0;
    doc["corrente"] = round(corrente     * 100) / 100.0;
    doc["lux_raw"]  = round(luminosidade * 10) / 10.0;
    doc["modulo"]   = "modulo1";

    char buffer[256];
    serializeJson(doc, buffer);

    bool ok = mqtt.publish(MQTT_TOPIC_PUB, buffer, false);
    Serial.printf(ok ? "[OK #%d] %s\n" : "[ERRO] Falha publish\n",
                 leituraId, buffer);
  }
}

void connectWifi() {
  Serial.printf("[WiFi] Conectando a '%s'", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.printf("\n[WiFi] IP: %s\n", WiFi.localIP().toString().c_str());
}

void connectMqtt() {
  while (!mqtt.connected()) {
    Serial.printf("[MQTT] Conectando ao %s...", MQTT_BROKER);
    if (mqtt.connect(MQTT_CLIENT_ID)) {
      Serial.println(" OK!");
      mqtt.subscribe(MQTT_TOPIC_CMD);
    } else {
      Serial.printf(" falhou (rc=%d). Retry em 3s\n", mqtt.state());
      delay(3000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  String msg;
  for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
  if (msg == "reset") { leituraId = 0; }
}

float lerTensao() {
  int raw = analogRead(VOLT_PIN);
  return (raw / ADC_MAX) * ADC_VREF * VOLT_SCALE;
}

float lerCorrente() {
  long soma = 0;
  for (int i = 0; i < 10; i++) {
    soma += analogRead(CURR_PIN); delayMicroseconds(100);
  }
  float vAdc  = (soma / 10 / ADC_MAX) * ADC_VREF;
  float vZero = (ACS_ZERO_ADC / ADC_MAX) * ADC_VREF;
  return ((vAdc - vZero) * 1000.0) / ACS_mV_PER_A;
}

float lerLuminosidade() {
  return (analogRead(LDR_PIN) / ADC_MAX) * 100.0;
}
