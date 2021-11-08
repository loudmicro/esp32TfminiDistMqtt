#include <TFMPlus.h>
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

//TFmini tfmini;
TFMPlus tfmP;


const char *mqtt_server = "192.168.0.100"; //the ip address of the mqtt server
const int mqtt_port = 1883;
const char *mqtt_user = "homeassistant";
const char *mqtt_pass = "whateveryourpasswordis"; 
const char *root_topic_subscribe = "dist/1";
const char *root_topic_publish = "dist/1"; //the topic you'll be suscribing in homeassistant


const char* ssid = "yourWifi";
const char* password =  "yourPass";

WiFiClient espClient;
PubSubClient client(espClient);
char msg[25];
long count=0;

void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();


void setup() {
    Serial.begin( 115200);   // Intialize terminal serial port
    delay(20);               // Give port time to initalize
   
    Serial2.begin( 115200);  // Initialize TFMPLus device serial port.
    delay(20);               // Give port time to initalize
    tfmP.begin( &Serial2);   // Initialize device library object and...

    setup_wifi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);


    delay(500);  // added to allow the System Rest enough time to complete

}

int16_t tfDist = 0;
int16_t tfFlux = 0;
int16_t tfTemp = 0;
void loop()
{
    if (!client.connected()) {
      reconnect();
    }

  
    delay(500);
   
    if( tfmP.getData( tfDist, tfFlux, tfTemp)) // Get data from the device.
    {
      Serial.print( "Dist: ");   // display distance in log
      Serial.println(tfDist);   // display distance in log

    }

    if (client.connected()){
      String str = String(tfDist);
      str.toCharArray(msg,25);
      client.publish(root_topic_publish,msg); //publish the topic
      count++;
      delay(300);
    }
    client.loop();

}

void setup_wifi(){
  delay(10);
  // Nos conectamos a nuestra red Wifi
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {

  while (!client.connected()) {
    Serial.print("Intentando conexión Mqtt...");
    // Creamos un cliente ID
    String clientId = "IOTICOS_H_W_";
    clientId += String(random(0xffff), HEX);
    // Intentamos conectar
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      Serial.println("Conectado!");
      // Nos suscribimos
      if(client.subscribe(root_topic_subscribe)){
        Serial.println("Suscripcion ok");
      }else{
        Serial.println("fallo Suscripciión");
      }
    } else {
      Serial.print("falló :( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length){
  String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  Serial.println("");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Mensaje -> " + incoming);

}
