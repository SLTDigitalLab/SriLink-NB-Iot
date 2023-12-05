#ifndef SriLinkDevBoard_h
#include "SriLinkDevBoard.h"
#endif

bool SriLinkDevBoard::Init(unsigned long buad_rate)
{
  // port config and buad rate set 
  pinMode(PWR_PIN, OUTPUT);
  Serial2.begin(buad_rate);
  // PWR on pin signal
  digitalWrite(PWR_PIN, HIGH);
  delay(600);
  digitalWrite(PWR_PIN, LOW);

  // disable echo mode.
  Serial2.write("ATE0\r\n"); // Turn off echo mode
  while(Serial2.available()) Serial2.read();
  delay(100);
  return AT_TEST();
}

bool SriLinkDevBoard::PWRDOWN()
{
  
  uint8_t answer = SENDATCMD("AT+CPOWD=1\r\n", 2000, "NORMAL POWER DOWN", "error");

  if (answer == 1)
  {
    digitalWrite(PWR_PIN, LOW);   
    return true;
  }
  return false;
  
}

bool SriLinkDevBoard::SET_APN(String APNNAME)
{
  uint8_t answer = SENDATCMD("AT+CFUN=0", 2000, "OK", "ERROR");
  if (answer == 1)
  {
    String atCommand = "AT*MCGDEFCONT=\"IP\",\"" + APNNAME + "\"\r\n";
    char charArray[atCommand.length()];
    atCommand.toCharArray(charArray, atCommand.length());
    answer = SENDATCMD(charArray, 4000, "OK", "ERROR");
    if (answer == 1)
    {
      uint8_t answer = SENDATCMD("AT+CFUN=1", 2000, "OK", "ERROR");
      if (answer == 1)
      {
        return true;
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }
  else
  {
    // Serial.println("AP  set  Error");
    return false;
  }
}

bool SriLinkDevBoard ::IS_ATTACH()
{
  String response;
  SEND_AT_CMD_RAW("AT+CGREG?\r\n",2000,&response);
  uint8_t answer = SENDATCMD("AT+CGREG?\r\n", 2000, "OK", "ERROR");
  if (answer)
  {
    //Serial.println(response[9]);
    if((response[8]=='0') && (response[10]=='1')) return true;
  }
  return false;
}

bool SriLinkDevBoard ::GET_IP()
{
  String response;
  uint8_t answer = SEND_AT_CMD_RAW("AT+CGCONTRDP\r\n",2000,&response);

  //if (response =="ERROR")
  //response = +CGCONTRDP: 1,5,"nbiot","10.106.221.48.255.255.255.0"
  //_NetStat.IP = 
  // uint8_t answer = SENDATCMD("AT+CGCONTRDP?\r\n", 2000, "OK", "ERROR");
  Serial.println(response);
  if (answer == 1)
  {
    return true;
  }
  else 
  {
    return false;
  }
}

bool SriLinkDevBoard ::MQTT_SETUP(Broker *broker, String server, String port)
{
  String atCommand = "AT+CMQNEW=" + server + "," + port + ",12000,1024\r\n";

  broker->addr = server;
  broker->port = port;

  char charArray[atCommand.length()];
  atCommand.toCharArray(charArray, atCommand.length());
  String response;
  bool ok = SEND_AT_CMD_RAW(charArray, 60000, &response);
  Serial.println("Setup response");
  Serial.println(response);
  if (ok)
  {
    broker->mqttId = uint8_t(response.substring(8).toInt());
    return true;
  }
  return false;
  // uint8_t answer = SENDATCMD(charArray, 4000, "OK", "ERROR");

  // if (answer == 1)
  // {
  //   Serial.println("success");
  //   return true;
  // }
  // else if (answer == 2)
  // {
  //   return false;
  // }
  // else
  // {
  //   return false;
  // }
}

bool SriLinkDevBoard::MQTT_CONNECT(Broker *broker, String clientid, String Username, String password)
{
  String atCommand = "AT+CMQCON="+String(broker->mqttId)+",3,\""+ clientid + "\",600,0,0,\""+Username+"\",\""+password+"\"\r\n";
  char charArray[atCommand.length()];
  atCommand.toCharArray(charArray, atCommand.length());
  //Serial.println(atCommand);
  uint8_t answer = SENDATCMD(charArray, 60000, "OK", "ERROR");
  //String response;
  //bool answer = SEND_AT_CMD_RAW(charArray, 30000, &response);
  //Serial.println(response);
  if (answer==1)
  {
    return true;
  }
  else
  {
    return false;
  }
 
}

bool SriLinkDevBoard::MQTT_CONNECT(Broker *broker, String clientid){
  return MQTT_CONNECT(broker, clientid, "", "");
}

bool SriLinkDevBoard::MQTT_CONNECT(Broker *broker, String clientid, String Username){
  return MQTT_CONNECT(broker, clientid, Username, "");
}

bool SriLinkDevBoard::MQTT_DISCONNECT(Broker *broker){
  String atCommand = "AT+CMQDISCON="+String(broker->mqttId);
  char charArray[atCommand.length()];
  atCommand.toCharArray(charArray, atCommand.length());
  //Serial.println(atCommand);
  uint8_t answer = SENDATCMD(charArray, 60000, "OK", "ERROR");
  //String response;
  //bool answer = SEND_AT_CMD_RAW(charArray, 30000, &response);
  //Serial.println(response);
  if (answer==1)
  {
    return true;
  }
  else
  {
    return false;
  }
}

uint8_t SriLinkDevBoard::MQTT_OUTPUT_FORMAT(bool Ishex)
{
  String atCommand = "AT+CREVHEX?\r\n";
  char charArray[atCommand.length()];
  atCommand.toCharArray(charArray, atCommand.length());
  uint8_t answer = SENDATCMD(charArray, 4000, "1", "0");

  if (answer == 1 && !Ishex)
  {
    String atCommand = "AT+CREVHEX=0\r\n";
    char charArray[atCommand.length()];
    atCommand.toCharArray(charArray, atCommand.length());
    uint8_t answer = SENDATCMD(charArray, 4000, "OK", "ERROR");
    if (answer == 2)
      return 0xf0; /* Error: 0xf0: Can't set CREVHEX = 0 */
  }
  else if (answer == 2 && Ishex)
  {
    String atCommand = "AT+CREVHEX=1\r\n";
    char charArray[atCommand.length()];
    atCommand.toCharArray(charArray, atCommand.length());
    uint8_t answer = SENDATCMD(charArray, 4000, "OK", "ERROR");
    if (answer == 2)
      return 0xf1; /* Error: 0xf1: Can't set CREVHEX = 1*/
  }
  return 0x01;
}

uint8_t SriLinkDevBoard::MQTT_PUB(Broker *broker, String topic, String msg, uint16_t msg_len, uint8_t qos, bool retained, bool dup, bool stream_hex)
{
  uint8_t format_ok = MQTT_OUTPUT_FORMAT(stream_hex);
  if (format_ok != 0x01)
    return format_ok;

  /* Input validation */
  if (qos < 0 || qos > 2)
    return 0xe0; /* QoS must be 0, 1, 2 */
  if (msg_len < 2 || msg_len > 1000)
    return 0xe1; /* msg_len must be in between 2 and 1000 */
  if (stream_hex && !(msg_len % 2))
    return 0xe2; /* when stream mode is set to hex, msg_len must be odd */

  String _ret = "0";//retained ? '1':'0';
  String _dup = "0";//dup ? '1':'0';

  String atCommand = "AT+CMQPUB=" + String(broker->mqttId) + ",\"" + topic + "\"," + qos + "," + _ret + "," + _dup + "," + String(msg_len) + ",\"" + msg + "\"\r\n";
  char charArray[atCommand.length()];
  Serial.println("pub msg");
  Serial.println(atCommand);
  atCommand.toCharArray(charArray, atCommand.length());
  uint8_t answer = SENDATCMD(charArray, 4000, "OK", "ERROR");
  return answer == 1 ? 0x01 : answer;
}

uint8_t SriLinkDevBoard::MQTTSUB(Broker *broker, String topic, uint8_t qos)
{
  /* Input validation */
  if (qos < 0 || qos > 2)
    return 0xe0; /* QoS must be 0, 1, 2 */

  String atCommand = "AT+CMQSUB=" + String(broker->mqttId) + ",\"" + topic + "\"," + qos + "\r\n";
  char charArray[atCommand.length()];
  atCommand.toCharArray(charArray, atCommand.length());
  uint8_t answer = SENDATCMD(charArray, 4000, "OK", "ERROR");
  return answer == 1 ? 0x01 : answer;
}

uint8_t SriLinkDevBoard::MQTTUNSUB(Broker *broker, String topic)
{
  String atCommand = "AT+CMQUNSUB=" + String(broker->mqttId) + "," + topic + "\r\n";
  char charArray[atCommand.length()];
  atCommand.toCharArray(charArray, atCommand.length());
  uint8_t answer = SENDATCMD(charArray, 4000, "OK", "ERROR");
  return answer == 1 ? 0x01 : answer;
}

bool SriLinkDevBoard::SEND_AT_CMD_RAW(char *at_command, unsigned int timeout, String *resp)
{
  uint8_t x = 0;
  char response[100] = {0}; // uint8 responce[100] -
  unsigned long previous;
  bool buffer_start = false;
  bool buffer_end = false;

  while (Serial2.available() > 0)
    Serial2.read(); // Clean the input buffer

  // delay(100);

  Serial2.write(at_command); // Send the AT command

  // delay(00);
  previous = millis();

  do
  {
    if (Serial2.available())
    {
      char tem1 = Serial2.read();
      if (tem1 = '\r')
      {
        char tem2 = Serial2.read();
        if (tem1 = '\n')
        {
          buffer_start = true;
        }
      }
    }
    delay(10);
  } while (!buffer_start && ((millis() - previous) < timeout));
  if (!buffer_start)
  {
    return false;
  }

  do
  {
    if (Serial2.available())
    {
      response[x] = Serial2.read();
      x++;
    }
  
    if (x > 1)
    {
      char tempArray[2] = {0};
      tempArray[0] = response[x-2];
      tempArray[1] = response[x-1];
      if (strstr(tempArray, "\r\n") != NULL)
      {
        buffer_end = true;
      }
    }
  } while (!buffer_end && ((millis() - previous) < timeout));

  char tempStr[100] = {0};
  for(uint8_t i = 0; i < x - 2; i++) {
    tempStr[i] = response[i];
  }
  *resp = String(tempStr);
  return true;
}


uint8_t SriLinkDevBoard::SENDATCMD(const char* at_command, unsigned int timeout, const char *expected_answer1, const char *expected_answer2)
{

  uint8_t x = 0, answer = 0;
  char response[100] = {0};

  unsigned long previous;

  while (Serial2.available() > 0)
    Serial2.read(); // Clean the input buffer

  delay(100);

  Serial2.write(at_command); // Send the AT command

  delay(1000);

  previous = millis();
  // this loop waits for the answer
  do
  {
    // if there are data in the UART input buffer, reads it and checks for the asnwer
    if (Serial2.available() != 0)
    {
      response[x] = Serial2.read();

      if (expected_answer1 != "")
      {
        // check if the desired answer   is in the response of the module
        if (strstr(response, expected_answer1) != NULL)
        {
          answer = 1;
          delay(100);
        }
        else if (expected_answer2 != "")
        {
          if (strstr(response, expected_answer2) != NULL)
          {
            answer = 2;
            delay(100);
          }
        }
      }
      else
      {
        // Serial.print(response);
      }
      x++;
      delay(10);
    }

  } while ((answer == 0) && ((millis() - previous) < timeout));

  // if (answer == 0) {
  //   Serial.println("AT response Time Out");
  // }
  return answer;
}

bool SriLinkDevBoard::AT_TEST()
{
  uint8_t answer = SENDATCMD("AT\r\n", 2000, "OK", "ERROR");
  if (answer == 1)
  {
    return true;
  }
  else
  {
    return false;
  }
}


bool SriLinkDevBoard::PowerDown(){
  uint8_t answer = SENDATCMD("AT+CPOWD=1", 2000, "OK", "ERROR");
  if (answer == 1)
  {
    return true;
  }
  else
  {
    return false;
  }
}