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

  //set APN to mobitel NBIOT. APN - "nbiot"
  if(!devboard.SET_APN("nbiot")){
    Serial.println("APN SET error");
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
  if(devboard.MQTT_SETUP(&TB_Broker, "test.mosquitto.org","1883"))
  {
    delay(200);
    if(devboard.MQTT_CONNECT(&TB_Broker, "iiot-dev-0001"))
    {
        Serial.println("MQTT Connect successful");
        connected = true;

    }
    else
    {
      Serial.println("MQTT Connect fail");
    }
  }

  if(connected)
  {
    uint8_t answer = devboard.MQTTSUB(&TB_Broker,"iiot/dev/#", 0);
    if (answer=1){
      Serial.println("subscribe successfull");
    }
    else {
      Serial.println("subscribe fail");
    }
  }

}

void loop() {
  if(Serial2.available()){
    Serial.write(Serial2.read());
  }
  


  if(connected){
    String data = "{'Temperature':" + String(120) + ",'Vibration':" + String(0227) + "}";
    Serial.println("String data");
    Serial.println(data);    
    uint8_t val = devboard.MQTT_PUB(&TB_Broker, "v1/devices/me/telemetry", data, data.length(), 0, false, false, false);

    Serial.println("msg send - ");
    Serial.print(val);
  }

}
