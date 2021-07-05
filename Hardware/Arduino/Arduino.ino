#include <ArduinoJson.h>
#include <dht.h>
#include <SoftwareSerial.h>

//Instances
dht DHT;
SoftwareSerial node(2, 3); //RX | TX

//---------------------------------------------------------------------------

//Pin declirations
#define DHT11_PIN 7
#define MOIS_PIN A0

//---------------------------------------------------------------------------

//Variables
float temp;
float humi;
float mois;
//---------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  node.begin(9600);
  delay(1000);

  Serial.println("Program Started");
}

void loop() {
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();
  
  //Temp and Humidity
  //--------------------------------------------------------------------------
  float chk = DHT.read11(DHT11_PIN);
  
  temp = DHT.temperature;
  humi = DHT.humidity;

  data["temp"] = temp;
  data["humi"] = humi;
  //--------------------------------------------------------------------------

  //Moisture levels and decisions
  //# the sensor value description
  //# 0  ~300     dry soil
  //# 300~700     humid soil
  //# 700~950     in water

  mois = analogRead(MOIS_PIN);
  data["mois"] = mois;
  
  //---------------------------------------------------------------------------

  //Sending data to ESP for server transfer
  //--------------------------------------------------------------------------

  data.printTo(node);
  data.printTo(Serial);
  Serial.println();
  jsonBuffer.clear();
  //--------------------------------------------------------------------------
  delay(5000);
}
