#include <DHT.h>
#include <WebSocketClient.h>
#include <ESP8266WiFi.h>

boolean handshakeFailed=0;

String data= "";
float temp;
float humd;

char path[] = "/NodeOne";
const char* ssid     = "Delport-WiFi 2.4GHz";
const char* password = "Adp!001G";

char* host = "192.168.1.102";
const int espport= 3000;
  
WebSocketClient webSocketClient;

#define DHTTYPE DHT11
uint8_t DHTPin = D2;

DHT dht(DHTPin,DHTTYPE);

WiFiClient client;

void setup() {
  Serial.begin(115200);
  
  delay(10);
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
  
  wsconnect();
}
void loop() {
  if (client.connected()) {    
    //webSocketClient.getData(data);
    
    temp = dht.readTemperature();
    humd = dht.readHumidity();
          
    data= (String)temp + "," + (String)humd;
          
    Serial.println("Tempreture: " + (String)temp + " Humidity: " + (String)humd);
    webSocketClient.sendData(data);//send sensor data to websocket server
    
    delay(30000);
  }
  //do else here and call wsconnect
}

void wsconnect(){
  // Connect to the websocket server
  if (client.connect(host, espport)) {
    Serial.println("Connected");
  } 
  else {
    Serial.println("Connection failed.");
    delay(1000);  
   
    if(handshakeFailed){
      handshakeFailed=0;
      ESP.restart();
    }
    handshakeFailed=1;
  }
  
  // Handshake with the server
  webSocketClient.path = path;
  webSocketClient.host = host;
  
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } 
  else {
    Serial.println("Handshake failed.");
    delay(4000);  
   
    if(handshakeFailed){
      handshakeFailed=0;
      ESP.restart();
    }
    handshakeFailed=1;
  }
}
