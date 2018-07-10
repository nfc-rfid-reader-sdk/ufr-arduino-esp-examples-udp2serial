#include <esp_wifi.h>
#include <FS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Preferences.h>
#include "frontend.h"

#define DUAL_MODE //Uncomment for DUAL_MODE UFR mode

#define UART_BAUD 115200
#define DEBUG_PORT 8880
#define bufferSize 300
#define timeout 5

#define UART_BAUD1 115200
#define SERIAL_PARAM1 SERIAL_8N1
#define SERIAL1_RXPIN 16
#define SERIAL1_TXPIN 17
#define SERIAL1_RESETPIN 5
int SERIAL1_UDP_PORT;

#ifdef DUAL_MODE
#define UART_BAUD2 115200
#define SERIAL_PARAM2 SERIAL_8N1
#define SERIAL2_RXPIN 15
#define SERIAL2_TXPIN 4
#define SERIAL2_RESETPIN 18
int SERIAL2_UDP_PORT;
#endif

#define SET_DEFAULTPIN 13
const char *ssid = "";
const char *pw = "";

AsyncWebServer server(80);

const char *ssidAP = "uFR";
const char *passwordAP = "password";

HardwareSerial Serial1(1);
WiFiUDP udp;
IPAddress remoteIp;
uint16_t rPort;

WiFiUDP udpDebug;
IPAddress remoteIpDebug;
uint16_t rPortDebug;

#ifdef DUAL_MODE
HardwareSerial Serial2(2);
WiFiUDP udp1;
IPAddress remoteIp1;
uint16_t rPort1;
#endif

uint8_t buffer1[bufferSize];
uint8_t length1 = 0;
uint8_t buffer2[bufferSize];
uint8_t length2 = 0;

uint8_t bufferTransparent[bufferSize];
uint8_t lengthTransparent = 0;

uint8_t bufferDebug[bufferSize];
uint8_t lengthDebug = 0;
uint8_t buffer2Debug[bufferSize];
uint8_t length2Debug = 0;

bool flashMode1 = false;
bool flashing1 = false;
bool flashMode2 = false;
bool flashing2 = false;

uint8_t transparentMode;
uint8_t transparentDevice;
uint8_t sendFrom = 0;

Preferences preferences;

#ifdef DUAL_MODE
uint8_t buffer11[bufferSize];
uint8_t length11 = 0;
uint8_t buffer21[bufferSize];
uint8_t length21 = 0;
#endif
int LED_BUILTIN = 2;
#ifdef __cplusplus
extern "C"
{
#endif

uint8_t temprature_sens_read();

#ifdef __cplusplus
}
#endif

void setDefault()
{
  preferences.begin("apsettings", false);
  preferences.clear();
  preferences.end();
}

void serverStart()
{
  server.begin();
}

void serverHandle()
{
  /*server.on("/toggleflashmode1", HTTP_GET, [](AsyncWebServerRequest * request) {
    flashMode1 = !flashMode1;
    request->send(200, "text/plain", String(flashMode1));
    });

    server.on("/toggleflashmode2", HTTP_GET, [](AsyncWebServerRequest * request) {
    flashMode2 = !flashMode2;
    request->send(200, "text/plain", String(flashMode2));
    });*/

  server.on("/toggletransparent", HTTP_GET, [](AsyncWebServerRequest * request) {
    preferences.begin("apsettings", false);
    uint8_t t = preferences.getUInt("transparent", 1);
    preferences.putUInt("transparent", !t);
    preferences.end();
    request->send(200, "text/plain", String(!t));
    digitalWrite(LED_BUILTIN, LOW);

    ESP.restart();
  });

  server.on("/changetransparent", HTTP_GET, [](AsyncWebServerRequest * request) {
    preferences.begin("apsettings", false);
    uint8_t d = preferences.getUInt("device", 1);
#ifdef DUAL_MODE
    if (d == 1)
    {
      d = 2;
      preferences.putUInt("device", d);
    }
    else
    {
      d = 1;
      preferences.putUInt("device", d);
    }
#else
    preferences.putUInt("device", 1);
#endif
    preferences.end();
    request->send(200, "text/plain", String(d));
    digitalWrite(LED_BUILTIN, LOW);

    ESP.restart();
  });

  server.on("/changeap", HTTP_POST, [](AsyncWebServerRequest * request) {
    int paramsNr = request->params();
    String ap = "";
    String ps = "";
    for (int i = 0; i < paramsNr; i++)
    {

      AsyncWebParameter *p = request->getParam(i);
      if (p->name() == "ssid")
      {
        ap = p->value();
      }
      else if (p->name() == "pass")
      {
        ps = p->value();
      }
    }

    if (ap.length() > 0 && ps.length() > 7)
    {
      preferences.begin("apsettings", false);
      preferences.putString("ssid", ap);
      preferences.putString("password", ps);
      preferences.end();
      request->send(200, "text/plain", "1");
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);

      ESP.restart();
    }
    else
    {
      request->send(200, "text/plain", "0");
    }
  });

  server.on("/changesta", HTTP_POST, [](AsyncWebServerRequest * request) {
    int paramsNr = request->params();
    String ap = "";
    String ps = "";
    for (int i = 0; i < paramsNr; i++)
    {

      AsyncWebParameter *p = request->getParam(i);
      if (p->name() == "ssid")
      {
        ap = p->value();
      }
      else if (p->name() == "pass")
      {
        ps = p->value();
      }
    }

    if (ap.length() > 0 && ps.length() > 7)
    {
      preferences.begin("apsettings", false);
      preferences.putString("ssidSTA", ap);
      preferences.putString("passwordSTA", ps);
      preferences.end();
      request->send(200, "text/plain", "1");
      digitalWrite(LED_BUILTIN, LOW);

      ESP.restart();
    }
    else
    {
      request->send(200, "text/plain", "0");
    }
  });

  server.on("/scan", HTTP_GET, [](AsyncWebServerRequest * request) {
    int n = WiFi.scanNetworks();
    String output = "[";
    for (int i = 0; i < n; ++i)
    {

      output += "{ \"ssid\":\"" + WiFi.SSID(i) + "\",\"signal\":" + WiFi.RSSI(i) + ",\"encryption\":\"" + translateEncryptionType(WiFi.encryptionType(i)) + "\"},";
    }
    request->send(200, "application/json", output.substring(0, output.length() - 1) + "]");
  });

  server.on("/info", HTTP_GET, [](AsyncWebServerRequest * request) {
    String output = "[{\"status\":\"disconnected\"";
    int n = WiFi.status();

    if (n == 3)
    {
#ifdef DUAL_MODE
      output = "[{\"status\":\"connected\",\"ssid\":\"" + WiFi.SSID() + "\",\"ip\":\"" + WiFi.localIP().toString() + "\",\"mac\":\"" + WiFi.macAddress() + "\",\"port1\":" + SERIAL1_UDP_PORT + ",\"port2\":" + SERIAL2_UDP_PORT;
#else
      output = "[{\"status\":\"connected\",\"ssid\":\"" + WiFi.SSID() + "\",\"ip\":\"" + WiFi.localIP().toString() + "\",\"mac\":\"" + WiFi.macAddress() + "\",\"port1\":" + SERIAL1_UDP_PORT;

#endif
    }
    preferences.begin("apsettings", false);

    String both = ", \"transparent\":" + String(preferences.getUInt("transparent", 1)) + ", \"device\":" + String(preferences.getUInt("device", 1)) + ", \"ap\":\"" + preferences.getString("ssid", String(ssidAP)).c_str() + "\", \"pass\":\"" + preferences.getString("password", String(passwordAP)).c_str() + "\"";
    preferences.end();
    request->send(200, "application/json", output + both + "}]");
  });

  server.on("/setport", HTTP_POST, [](AsyncWebServerRequest * request) {
    int paramsNr = request->params();
    int p1;
#ifdef DUAL_MODE
    int p2;
#endif
    for (int i = 0; i < paramsNr; i++)
    {
      AsyncWebParameter *p = request->getParam(i);
      if (p->name() == "port1")
      {
        p1 = p->value().toInt();
      }
#ifdef DUAL_MODE
      else if (p->name() == "port2")
      {
        p2 = p->value().toInt();
      }
#endif
    }
#ifdef DUAL_MODE
    if (p1 > 0 && p1 < 65536 && p2 > 0 && p2 < 65536 && p1 != p2)

#else
    if (p1 > 0 && p1 < 65536)
#endif
    {
      preferences.begin("apsettings", false);
      preferences.putInt("port1", p1);
#ifdef DUAL_MODE
      preferences.putInt("port2", p2);
#endif
      preferences.end();
      request->send(200, "text/plain", "1");
      digitalWrite(LED_BUILTIN, LOW);

      ESP.restart();
    }
    else
    {
      request->send(200, "text/plain", "0");
    }
  });

  server.on("/disconnect", HTTP_GET, [](AsyncWebServerRequest * request) {
    preferences.begin("apsettings", false);
    preferences.putString("ssidSTA", "");
    preferences.putString("passwordSTA", "");
    preferences.end();
    request->send(200, "text/plain", "1");
    digitalWrite(LED_BUILTIN, LOW);

    ESP.restart();
  });

  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "1");
    digitalWrite(LED_BUILTIN, LOW);

    ESP.restart();
  });

  server.on("/setdefault", HTTP_GET, [](AsyncWebServerRequest * request) {
    setDefault();
    request->send(200, "text/plain", "1");
    digitalWrite(LED_BUILTIN, LOW);

    ESP.restart();
  });

  server.on("/alive", HTTP_GET, [](AsyncWebServerRequest * request) {

    request->send(200, "text/plain", "1");
    digitalWrite(LED_BUILTIN, LOW);

  });

  server.on("/identify", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    request->send(200, "text/plain", "1");


  });
}

String translateEncryptionType(wifi_auth_mode_t encryptionType)
{

  switch (encryptionType)
  {
    case (WIFI_AUTH_OPEN):
      return "Open";
    case (WIFI_AUTH_WEP):
      return "WEP";
    case (WIFI_AUTH_WPA_PSK):
      return "WPA_PSK";
    case (WIFI_AUTH_WPA2_PSK):
      return "WPA2_PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
      return "WPA_WPA2_PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
      return "WPA2_ENTERPRISE";
  }
}

void setup()
{
  delay(500);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SET_DEFAULTPIN, OUTPUT);
  if (digitalRead(SET_DEFAULTPIN) == HIGH)
  {
    setDefault();
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(3000);
    digitalWrite(LED_BUILTIN, LOW);
    ESP.restart();
  }
  Serial.begin(115200);

  pinMode(SERIAL1_RESETPIN, OUTPUT);
  pinMode(18, INPUT);
    digitalWrite(SERIAL1_RESETPIN, HIGH);
  delay(100);
  digitalWrite(SERIAL1_RESETPIN, LOW);
  delay(200);
  Serial1.begin(UART_BAUD1, SERIAL_PARAM1, SERIAL1_RXPIN, SERIAL1_TXPIN);

#ifdef DUAL_MODE
  digitalWrite(SERIAL2_RESETPIN, HIGH);
  delay(10);
  digitalWrite(SERIAL2_RESETPIN, LOW);
  delay(200);
  Serial2.begin(UART_BAUD2, SERIAL_PARAM2, SERIAL2_RXPIN, SERIAL2_TXPIN);
  pinMode(SERIAL2_RESETPIN, OUTPUT);
  pinMode(19, INPUT);
#endif

  preferences.begin("apsettings", false);

  WiFi.disconnect(true);
  delay(500);
  WiFi.mode(WIFI_AP_STA);
  WiFi.persistent(false);
  WiFi.softAP(preferences.getString("ssid", String(ssidAP)).c_str(), preferences.getString("password", String(passwordAP)).c_str());
  WiFi.begin(preferences.getString("ssidSTA", "").c_str(), preferences.getString("passwordSTA", "").c_str());

  transparentMode = preferences.getUInt("transparent", 1);
  transparentDevice = preferences.getUInt("device", 1);
  SERIAL1_UDP_PORT = preferences.getInt("port1", 8881);



#ifdef DUAL_MODE
  SERIAL2_UDP_PORT = preferences.getInt("port2", 8882);
#endif
  preferences.end();
  uint8_t wifiTimeout = 30;
  while (WiFi.status() != WL_CONNECTED && wifiTimeout > 0)
  {
    delay(1000);
    wifiTimeout--;
  }

  if (WiFi.status() != WL_CONNECTED && wifiTimeout == 0)
  {
    WiFi.mode(WIFI_AP);
  }


  udp.begin(SERIAL1_UDP_PORT);
#ifdef DUAL_MODE

  udp1.begin(SERIAL2_UDP_PORT);
#endif

  serverStart();
  serverHandle();

  ArduinoOTA.setHostname("uFR_Nano");

  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else
      type = "filesystem";

    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });

  ArduinoOTA.begin();

  delay(300);

  if (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

bool debug(uint8_t *in, int num)
{

  uint8_t out[7];

  if (in[0] == 0x55 && in[1] == 0xFE && in[2] == 0xAA && in[3] == 0x00 && in[5] == 0x00)
  {
    if (in[4] == 0x01 && in[6] == ((in[0] ^ in[1] ^ in[2] ^ in[3] ^ in[4] ^ in[5]) + 0x07))
    {
      if (num == 1)
      {
        digitalWrite(SERIAL1_RESETPIN, HIGH);
        delay(10);
        digitalWrite(SERIAL1_RESETPIN, LOW);
        flashMode1 = false;
      }
      else if (num == 2)
      {
#ifdef DUAL_MODE
        digitalWrite(SERIAL2_RESETPIN, HIGH);
        delay(10);
        digitalWrite(SERIAL2_RESETPIN, LOW);
        flashMode2 = false;
#endif
      }

      out[0] = 0xDE;
      out[1] = 0xFE;
      out[2] = 0xED;
      out[3] = 0x00;
      out[4] = 0x00;
      out[5] = 0x00;
      out[6] = (out[0] ^ out[1] ^ out[2] ^ out[3] ^ out[4] ^ out[5]) + 0x07;
      if (num == 1)
      {
        udp.beginPacket(remoteIp, rPort);
        udp.write(out, 7);
        udp.endPacket();
      }
      else if (num == 2)
      {
#ifdef DUAL_MODE
        udp1.beginPacket(remoteIp1, rPort1);
        udp1.write(out, 7);
        udp1.endPacket();
#endif
      }
    }
    else if (in[4] == 0x02 && in[6] == ((in[0] ^ in[1] ^ in[2] ^ in[3] ^ in[4] ^ in[5]) + 0x07))
    {

      out[0] = 0xDE;
      out[1] = 0xFE;
      out[2] = 0xED;
      out[3] = 0x00;
      out[4] = (temprature_sens_read() - 32) / 1.8;
      out[5] = 0x00;
      out[6] = (out[0] ^ out[1] ^ out[2] ^ out[3] ^ out[4] ^ out[5]) + 0x07;
      if (num == 1)
      {
        udp.beginPacket(remoteIp, rPort);
        udp.write(out, 7);
        udp.endPacket();
      }
      else if (num == 2)
      {
#ifdef DUAL_MODE
        udp1.beginPacket(remoteIp1, rPort1);
        udp1.write(out, 7);
        udp1.endPacket();
#endif
      }
    }
   
    return true;
  }
  else
  {
    return false;
  }
}

void flash(uint8_t num, uint32_t packetSize)
{
  if (num == 1)
  {
    /*udp.beginPacket(remoteIp, rPort);
      udp.write(flashBuffer1, packetSize);
      udp.endPacket();
      free(flashBuffer1);

      flashMode1 = false;*/
  }
  else if (num == 2)
  {
#ifdef DUAL_MODE

    /* udp1.beginPacket(remoteIp1, rPort1);
      udp1.write(flashBuffer2, packetSize);
      udp1.endPacket();
      free(flashBuffer2);
      flashMode2 = false;*/

#endif
  }
}

bool checkChange(uint8_t *in, uint8_t *out)
{
 if (in[0] == 0x55 && in[1] == 0xFE && in[2] == 0xAA && in[3] == 0x00 && in[5] == 0x00 && in[4] == 0x03 && in[6] == ((in[0] ^ in[1] ^ in[2] ^ in[3] ^ in[4] ^ in[5]) + 0x07))
    {

      out[0] = 0xDE;
      out[1] = 0xFE;
      out[2] = 0xED;
      out[3] = 0x00;
      out[4] = 0x00;
      out[5] = 0x00;
      out[6] = (out[0] ^ out[1] ^ out[2] ^ out[3] ^ out[4] ^ out[5]) + 0x07;
     preferences.begin("apsettings", false);
    uint8_t d = preferences.getUInt("device", 1);
#ifdef DUAL_MODE
    if (d == 1)
    {
      d = 2;
      preferences.putUInt("device", d);
    }
    else
    {
      d = 1;
      preferences.putUInt("device", d);
    }
#else
    preferences.putUInt("device", 1);
#endif
    preferences.end();
    transparentDevice = d;
    return true;
    }
    else
    {
    return false;   
    }
}

void transparent()
{
#ifdef DUAL_MODE
  HardwareSerial transparentSerial[2] = {Serial1, Serial2};
#else
  HardwareSerial transparentSerial[1] = {Serial1};
#endif

  if (Serial.available())
  {
    sendFrom = 0;
    while (1)
    {
      if (Serial.available())
      {
        bufferTransparent[lengthTransparent] = Serial.read();
        if (lengthTransparent < bufferSize - 1)
        {
          lengthTransparent++;
        }
      }
      else
      {
        delay(timeout);
        if (!Serial.available())
        {
          break;
        }
      }
    }
    uint8_t out[7];
    if(checkChange(bufferTransparent, out)==false)
    {
    transparentSerial[transparentDevice - 1].write(bufferTransparent, lengthTransparent);
    lengthTransparent = 0;
    }
    else
    {
      Serial.write(out, 7);
      Serial.flush();
          lengthTransparent = 0;

    }
  }
  if (sendFrom == 0)
  {
    if (transparentSerial[transparentDevice - 1].available())
    {
      while (1)
      {
        if (transparentSerial[transparentDevice - 1].available())
        {
          buffer1[length1] = transparentSerial[transparentDevice - 1].read();
          if (length1 < bufferSize - 1)
          {
            length1++;
          }
        }
        else
        {
          delay(timeout);
          if (!transparentSerial[transparentDevice - 1].available())
          {
            break;
          }
        }
      }

      Serial.write(buffer1, length1);
      length1 = 0;
    }
  }
}

void loop()
{
 
  ArduinoOTA.handle();

  if (transparentMode)
  {
    transparent();
  }
  // else
  // {
  if (!flashMode1)
  {
    udp.flush();
    delay(10);
    int packetSize = udp.parsePacket();
         
    if (packetSize > 0)
    {
     Serial.println("udp.remotePort()");
    Serial.println(udp.remotePort());
    
         Serial.println("RECIEVED");

          Serial.println("packetSize");
          Serial.println(packetSize);

      sendFrom = 1;
      remoteIp = udp.remoteIP();
      rPort = udp.remotePort();
     
       Serial.println("remoteIP");
          Serial.println(remoteIp);
           Serial.println("remotePort");
          Serial.println(rPort);         
   


       
      udp.read(buffer1, bufferSize);
      if (!debug(buffer1, 1))
      {
          Serial1.write(buffer1, packetSize);
      }
    
    }
    if (sendFrom == 1)
    {

    
      if (Serial1.available())
      {
        while (1)
        {
          if (Serial1.available())
          {
            buffer2[length2] = Serial1.read();

            if (length2 < bufferSize - 1)
            {
              length2++;
            }
          }
          else
          {
            delay(timeout);
            if (!Serial1.available())
            {
              break;
            }
          }
        }
             Serial.println("length2");
          Serial.println(length2);      
        int packetSize = udp.parsePacket();
        udp.beginPacket(remoteIp, rPort);
        udp.write(buffer2, length2);
        udp.endPacket();
        length2 = 0;
         Serial.println("SENT");
        
    }
    
    }

    
  }
  else
  {
    uint8_t reject[7] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    int packetSize = udp.parsePacket();
    if (packetSize > 0)
    {
      remoteIp = udp.remoteIP();
      rPort = udp.remotePort();
      udp.read(buffer1, bufferSize);
      udp.beginPacket(remoteIp, rPort);
      udp.write(reject, 7);
      udp.endPacket();
    }
  }

#ifdef DUAL_MODE
  if (!flashMode2)
  {
    int packetSize1 = udp1.parsePacket();
    if (packetSize1 > 0)
    {

      sendFrom = 1;
      remoteIp1 = udp1.remoteIP();
      rPort1 = udp1.remotePort();
      if (!flashMode2)
      {
        udp1.read(buffer2, bufferSize);
        if (!debug(buffer2, 2))
        {
          Serial2.write(buffer2, packetSize1);
        }
      }
      /* else
        {
         flashBuffer2 = (uint8_t*)malloc(64000);
         udp.read(flashBuffer2, 64000);
         flash(2, packetSize1);
        }*/
    }
    if (sendFrom == 1)
    {
      if (Serial2.available())
      {
        while (1)
        {
          if (Serial2.available())
          {
            buffer21[length21] = Serial2.read();
            if (length21 < bufferSize - 1)
            {
              length21++;
            }
          }
          else
          {
            delay(timeout);
            if (!Serial2.available())
            {
              break;
            }
          }
        }

        udp1.beginPacket(remoteIp1, rPort1);
        udp1.write(buffer21, length21);
        udp1.endPacket();
        length21 = 0;
      }
    }
  }
  else
  {
    uint8_t reject[7] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    int packetSize1 = udp1.parsePacket();
    if (packetSize1 > 0)
    {
      remoteIp1 = udp1.remoteIP();
      rPort1 = udp1.remotePort();
      udp1.read(buffer2, bufferSize);
      udp1.beginPacket(remoteIp1, rPort1);
      udp1.write(reject, 7);
      udp1.endPacket();
    }
  }
#endif
  
}

