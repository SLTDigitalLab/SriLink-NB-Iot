#include "SriLinkDevBoard.h"

SriLinkDevBoard devboard;

Broker TB_Broker;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
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
  }while((devboard.IS_ATTACH()==false) && ((millis()-starttime)<5000));
  Serial.println(" ");
  Serial.println("connection succesful");

  if(!devboard.GET_IP())
  {
    Serial.println("IP Error");
  }
  bool connected=false;
  if(devboard.MQTT_SETUP(TB_Broker, "mqtt.thingsboard.cloud","1883")
  {
    delay(200);
    if(devboard.MQTT_CONNECT(TB_Broker, "mn123")
    {
        Serial.println("MQTT Connect successful");
        connected = true;
    }
  }

  




}

void loop() {
  if(connected){
    String data = "{"Temperature":" + string(120) + ", "Vibration":" + string(0227) + "}";
    uint8_t val = MQTT_PUB(TB_Broker, "v1/devices/me/telemetry", data, data.length(), 0, false, false, true);

    serial.println("msg send - ");
    serial.print(val);
  }
  delay(3000);
  // put your main code here, to run repeatedly:

}
