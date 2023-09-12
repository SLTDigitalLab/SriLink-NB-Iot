#include "SriLinkDevBoard.h"
#include <String>

SriLinkDevBoard devboard;

Broker TB_Broker;
bool connected;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  connected=false;
  Serial.println("Start...");

  if(devboard.Init(115200)){
    Serial.println("initialization suceessful...");
  }
  else{
    Serial.println("initializartion fail...");
  }
  Serial.println("waiting for connection");
  long starttime = millis();
  do{
    Serial.print(".");
    delay(5);
  }while((devboard.IS_ATTACH()==false) && ((millis()-starttime)<60000));
  Serial.println(" ");
  Serial.println("connection succesful");

  if(!devboard.GET_IP())
  {
    Serial.println("IP Error");
  }
  delay(100);
  if(devboard.MQTT_SETUP(&TB_Broker, "mqtt.thingsboard.cloud","1883"))
  {
    delay(200);
    if(devboard.MQTT_CONNECT(&TB_Broker, "iiot-dev-0001","pramitha1","pramitha1"))
    {
        Serial.println("MQTT Connect successful");
        connected = true;

    }
    else
    {
      Serial.println("MQTT Connect fail");
    }
  }


}

void loop() {
   if(connected){
    String data = "{'Temperature':" + String(120) + ",'Vibration':" + String(0227) + "}";
    Serial.println("String data");
    Serial.println(data);    
    uint8_t val = devboard.MQTT_PUB(&TB_Broker, "v1/devices/me/telemetry", data, data.length(), 0, false, false, false);

    Serial.println("msg send - ");
    Serial.print(val);
  }
  delay(3000);
  // put your main code here, to run repeatedly:

}
