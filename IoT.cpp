
#include "DHT.h" // Biblioteca para o sensor DHT11
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define DHTPIN 2 // configuração hardware
#define DHTTYPE DHT11   // DHT 11

const char* ssid = "SSID - REDE";
const char* password = "Senha";
//BROKER
const char* mqtt_server = "test.mosquitto.org"; // mqtt broker
const int mqtt_port=1883;

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHT11 teste!"));
  dht.begin();
  wifi_conexao(); // FUNCAO QUE CONECTA WI-FI
  client.setServer(mqtt_server, mqtt_port); // configurar o servidor para realização do publish para testar o dado no broker
  client.setCallback(mqttCallback);
}

void loop() {
  
  if (!client.connected()) { // confirma conexão
    mqtt_reconnect();
    }
  
  client.loop();  // verificar mensagens recebidas
    
  delay(2000); // inicialização da leitura > 250ms (2s)

  //Leitura sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Checando leituras.
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  //publicação
  
  String temperatura = String(t);
  String umidade = String(h);

     // Publica os dados de temperatura e umidade nos tópicos especificados
   client.publish("mestrado/iot/aluno/michael/temperatura", temperatura.c_str());
   client.publish("mestrado/iot/aluno/michael/umidade", umidade.c_str());
    
   Serial.print("temperatura: ");
    Serial.println(temperatura);
    Serial.print("umidade: ");
    Serial.println(umidade);
}

void wifi_conexao(){
    
    // Conectando a uma rede WiFi
  Serial.println();
  Serial.print("Conectando a rede ");
  Serial.println(ssid);
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço ip: ");
  Serial.println(WiFi.localIP()); // escreve o endereço IP
}

void mqtt_reconnect(){ // caso nao tenha conectado tente reconectar 
  while (!client.connected()) {
    client.connect("Conectado");}
  }

//verificar as mensagens enviadas ao broker MQTT (opcional)
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  Serial.print("Mensagem: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);  // Converte cada byte em caractere para mostrar a mensagem
  }
  Serial.println();
}