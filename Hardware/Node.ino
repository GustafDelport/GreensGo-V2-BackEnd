#include<ArduinoJson.h>
#include<SoftwareSerial.h>
//--------------------------------------------
SoftwareSerial node(D2, D1); //SRX=Dpin-D2; STX-DPin-D1


//Variables
String data;
float temp;
float humi;
float mois;

//--------------------------------------------
//Code

void setup()
{
  Serial.begin(9600); //enable Serial Monitor
  node.begin(9600); //enable Node Port

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
  
  Serial.print(temp);
  Serial.print(",");
  Serial.print(humi);
  Serial.print(",");
  Serial.print(mois);
  Serial.println();
  //--------------------------------------------

  //Posting data to server
  
  //--------------------------------------------
}
//--------------------------------------------
