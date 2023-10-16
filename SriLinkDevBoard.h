/*
Author - Pramitha Muthukudaarachchi, navindu Madanayaka
Copyrights - Sri Lanka Telecom PLC
*/

#ifndef SriLinkDevBoard_h
#define SriLinkDevBoard_h

#include "Arduino.h"

struct Broker{
  String addr;
  String port = "1883";
  uint8_t version;
  uint8_t clientId;
  String username = "0";
  String password = "0";
  uint8_t mqttId;
};

struct NetStat{
  String IP;
  String APN;
  String oper_long;
  String oper_short;
  String oper_num;
  String AcT;
};

#define PWR_PIN 12
#define RST_PIN 4

class SriLinkDevBoard{
    public:

        bool Init(unsigned long buad_rate);
        //void enableNBIOT();
        bool AT_TEST();
        bool PWRDOWN();
        bool SET_APN(String APNNAME);
        bool IS_ATTACH();
        bool GET_IP();
        bool MQTT_SETUP(Broker *broker, String server,String port);
        bool MQTT_CONNECT(Broker *broker, String clientid);
        bool MQTT_CONNECT(Broker *broker, String clientid, String Username);
        bool MQTT_CONNECT(Broker *broker, String clientid, String Username, String password);
        uint8_t MQTT_OUTPUT_FORMAT(bool hex);
        uint8_t MQTT_PUB(Broker *broker, String topic, String msg, uint16_t msg_len, uint8_t qos, bool retained, bool dup, bool stream_hex);
        uint8_t MQTTSUB(Broker *broker, String topic, uint8_t qos);
        uint8_t MQTTUNSUB(Broker *broker, String topic);
        bool PowerDown();


        bool SEND_AT_CMD_RAW(char *at_command, unsigned int timeout, String* response);
        
    private:
        uint8_t SENDATCMD(const char* at_command, unsigned int timeout, const char* expected_answer1, const char* expected_answer2);
        NetStat _NetStat;
        
        //int _pin;    
};
#endif