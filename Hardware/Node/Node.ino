#include<ArduinoJson.h>
#include<SoftwareSerial.h>
#include <WebSocketClient.h>
#include <ESP8266WiFi.h>
//--------------------------------------------

//Variables
String passData;
float temp;
float humi;
float mois;
int lightVal;
boolean handshakeFailed=0;

char path[] = "/NodeOne";
const char* ssid     = "Delport-WiFi 2.4GHz";
const char* password = "Adp!001G";

//192.168.1.102:3000
//51.195.104.141:6579

char* host = "51.195.104.141";
const int espport= 6579;

//--------------------------------------------

//Instances
WebSocketClient webSocketClient;
SoftwareSerial node(D2, D1); //SRX=Dpin-D2; STX-DPin-D1
WiFiClient client;

//--------------------------------------------
//Code

void setup()
{
  Serial.begin(9600); //enable Serial Monitor
  node.begin(9600); //enable Node Port
  
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
  
  while(!Serial) continue;
}
  
void loop()
{
  //Recieving data from Arduino
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject(node);
  
  if (data == JsonObject::invalid())
  {
    jsonBuffer.clear();
    return;
  }
  
  temp = data["temp"];
  humi = data["humi"];
  mois = data["mois"];
  lightVal = data["light"];

  //Display data in serial to test we remove this later
  Serial.print(temp);
  Serial.print(",");
  Serial.print(humi);
  Serial.print(",");
  Serial.print(mois);
  Serial.print(",");
  Serial.print(lightVal);
  Serial.println();
  
  //--------------------------------------------

  //Posting data to server
  
  if (client.connected()) {    
    //webSocketClient.getData(data);
          
    passData = (String)temp + "," + (String)humi + "," + (String)mois + "," + (String)lightVal;
    webSocketClient.sendData(passData);//send sensor data to websocket server
  }
  //--------------------------------------------
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
//--------------------------------------------
