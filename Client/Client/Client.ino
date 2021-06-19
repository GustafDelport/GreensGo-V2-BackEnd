#include <WebSocketClient.h>
#include <ESP8266WiFi.h>

boolean handshakeFailed=0;
String data= "";
char path[] = "/NodeOne";
const char* ssid     = "Delport-WiFi 2.4GHz";
const char* password = "Adp!001G";

char* host = "192.168.1.102";
const int espport= 3000;
  
WebSocketClient webSocketClient;

unsigned long previousMillis = 0;
unsigned long currentMillis;
unsigned long interval=10000;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  //pinMode(readPin, INPUT);     // Initialize the LED_BUILTIN pin as an output
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
    currentMillis = millis(); 
    webSocketClient.getData(data);
      
      if (data.length() > 0) {
        if (abs((int)currentMillis - (int)previousMillis) >= interval) {
          previousMillis = currentMillis;
          data= (String) analogRead(A0); //read adc values, this will give random value, since no sensor is connected. 
          Serial.println(data);
          webSocketClient.sendData(data);//send sensor data to websocket server
        }
      }
      else{
      
      }
    delay(5);
  }
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
