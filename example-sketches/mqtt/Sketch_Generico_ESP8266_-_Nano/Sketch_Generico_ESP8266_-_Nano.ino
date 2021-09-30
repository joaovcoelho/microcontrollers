#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DEBUG
long now = millis();
long lastMeasure = 0;

//informações da rede WIFI
const char* ssid = "NOME DO SEU WIFI"; //SSID da rede WIFI
const char* password = "SUA SENHA"; //senha da rede wifi

//informações do broker MQTT
const char* mqttServer = "ENDEREÇO IP";
const int mqttPort = PORTA; //colocar o número da porta sem aspas
const char* mqttUser = "NOME DO USUÁRIO MQTT NO BROKER";
const char* mqttPassword = "SENHA";
const char* mqttTopicSub = "TOPICO PARA RECEBER";
const char* mqttTopicPub = "TOPICO PARA ENVIAR";  


WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef DEBUG
    Serial.println("Conectando ao WiFi..");
#endif
  }
#ifdef DEBUG
  Serial.println("Conectado na rede WiFi");
#endif
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
#ifdef DEBUG
    Serial.println("Conectando ao Broker MQTT...");
#endif
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
#ifdef DEBUG
      Serial.println("Conectado");
#endif
    } else {
#ifdef DEBUG
      Serial.print("falha estado  ");
      Serial.print(client.state());
#endif
      delay(2000);
    }
  }
  client.subscribe(mqttTopicSub);
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String strMSG = String((char*)payload);
#ifdef DEBUG
  Serial.print("Mensagem chegou do tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem:");
  Serial.print(strMSG);
  Serial.println();
  Serial.println("-----------------------");
#endif
}

void reconnect() {                            
  while (!client.connected()) {
#ifdef DEBUG
    Serial.print("Tentando conectar ao servidor MQTT");
#endif
    bool conectado = strlen(mqttUser) > 0 ?
                     client.connect("ESP8266Client", mqttUser, mqttPassword) :
                     client.connect("ESP8266Client");
    if (conectado) {
#ifdef DEBUG
      Serial.println("Conectado!");
#endif
  
      client.subscribe(mqttTopicSub, 1); 
    } else {
#ifdef DEBUG
      Serial.println("Falha durante a conexão.Code: ");
      Serial.println( String(client.state()).c_str());
      Serial.println("Tentando novamente em 5 s");
#endif
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop())
    client.connect("ESP8266Client");
  now = millis();
  if (now - lastMeasure > 5000) {      
    lastMeasure = now;
  }

// COLOCAR PARTE DO SEU CÓDIGO NESTE TRECHO ABAIXO

client.publish(mqttTopicPub, "Enviado de uma nova ESP"); // Envia essas mensagens para o topico escolhido
client.publish(mqttTopicPub, "Dado recebido pelo servidor!"); // Envia essas mensagens para o topico escolhido

}
