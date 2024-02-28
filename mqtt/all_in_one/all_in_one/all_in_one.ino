#include <ESP8266WiFi.h> 
#include <PubSubClient.h>

#define pinBotao1 12  //D6
#define pinLED1 2  //GP2 ESP-01

//WiFi
const char* SSID = "Bianca";                // SSID / nome da rede WiFi que deseja se conectar
const char* PASSWORD = "991014596";   // Senha da rede WiFi que deseja se conectar
WiFiClient wifiClient;                        
 
//MQTT Server
const char* BROKER_MQTT = "mqtt.eclipseprojects.io"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;                      // Porta do Broker MQTT


#define ID_MQTT  "richter_ID01"            //Informe um ID unico e seu. Caso sejam usados IDs repetidos a ultima conexão irá sobrepor a anterior. 
#define TOPIC_PUBLISH "richter_teste_01"    //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
#define TOPIC_SUBSCRIBE "richter_teste_02"
PubSubClient MQTT(wifiClient);        // Instancia o Cliente MQTT passando o objeto espClient

//Declaração das Funções
void mantemConexoes();  //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();     //Faz conexão com WiFi
void conectaMQTT();     //Faz conexão com Broker MQTT
void EnviaValorOn();
void EnviaValorOff();
void recebePacote(char* topic, byte* payload, unsigned int length);

void setup() {
  pinMode(pinBotao1, INPUT_PULLUP);         
  pinMode(pinLED1, OUTPUT);  

  Serial.begin(115200);

  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);  
  MQTT.setCallback(recebePacote);  
}

void loop() {
  mantemConexoes();
  EnviaValorOn();
  MQTT.loop();
  delay(5000);
  EnviaValorOff();
  MQTT.loop();
  delay(5000);
}

void mantemConexoes() {
    if (!MQTT.connected()) {
       conectaMQTT(); 
    }
    
    conectaWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void conectaWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
     return;
  }
        
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI  
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 
}

void conectaMQTT() { 
    while (!MQTT.connected()) {
        Serial.print("Conectando ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado ao Broker com sucesso!");
            MQTT.subscribe(TOPIC_SUBSCRIBE);
        } 
        else {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 10s");
            delay(10000);
        }
    }
}

void EnviaValorOn() {

  MQTT.publish(TOPIC_PUBLISH, "1");
  Serial.println("Payload enviado, valor 1");

}

void EnviaValorOff() {

  MQTT.publish(TOPIC_PUBLISH, "0");
  Serial.println("Payload enviado, valor 0");

}


void recebePacote(char* topic, byte* payload, unsigned int length) 
{
    String msg;

    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }

    Serial.println("Mensagem: " + msg);

    if (msg == "Desligar") {
       digitalWrite(pinLED1, HIGH);
    }

    if (msg == "Ligar") {
       digitalWrite(pinLED1, LOW);
    }
}
