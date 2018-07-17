#include <FS.h>
#include "SPIFFS.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Preferences.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

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
WebServer server(80);
WebServer server1(81);
WebServer server2(82);

const char *ssidAP = "uFR";
const char *passwordAP = "password";

//HardwareSerial Serial1(1);
WiFiUDP udp;
IPAddress remoteIp;
uint16_t rPort;

WiFiUDP udpDebug;
IPAddress remoteIpDebug;
uint16_t rPortDebug;

#ifdef DUAL_MODE
//HardwareSerial Serial2(2);
WiFiUDP udp1;
IPAddress remoteIp1;
uint16_t rPort1;
#endif

uint8_t buffer1[bufferSize];
uint8_t length1 = 0;

uint8_t buffer1h[bufferSize];
uint8_t length1h = 0;

uint8_t buffer2[bufferSize];
uint8_t length2 = 0;
uint8_t buffer2h[bufferSize];
uint8_t length2h = 0;

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
String http_username;
String http_password;

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
#ifdef DUAL_MODE
  pinMode(SERIAL2_RESETPIN, OUTPUT);
#endif
  digitalWrite(SERIAL1_RESETPIN, HIGH);
#ifdef DUAL_MODE
  digitalWrite(SERIAL2_RESETPIN, HIGH);
#endif
  delay(100);
  digitalWrite(SERIAL1_RESETPIN, LOW);
#ifdef DUAL_MODE
  digitalWrite(SERIAL2_RESETPIN, LOW);
#endif
  delay(200);
  Serial1.begin(UART_BAUD1, SERIAL_PARAM1, SERIAL1_RXPIN, SERIAL1_TXPIN);
#ifdef DUAL_MODE
  Serial2.begin(UART_BAUD2, SERIAL_PARAM2, SERIAL2_RXPIN, SERIAL2_TXPIN);
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
  http_username = preferences.getString("http_username", "ufr");
  http_password = preferences.getString("http_password", "ufr");
  SERIAL1_UDP_PORT = preferences.getInt("port1", 8881);

#ifdef DUAL_MODE
  SERIAL2_UDP_PORT = preferences.getInt("port2", 8882);
#endif
  preferences.end();
  uint8_t wifiTimeout = 10;
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
  SPIFFS.begin();
  serverStart();


  serverStart1();


  serverStart2();


  ArduinoOTA.setHostname("uFR_Nano");
  ArduinoOTA.setPassword(http_password.c_str());
  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else
      type = "filesystem";

    //Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    //Serial.println("\nEnd");
    digitalWrite(LED_BUILTIN, LOW);
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    //Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
    {
    }
    //Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
    {
    }
    //Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
    {
    }
    //Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
    {
    }
    //Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
    {
    }
    // Serial.println("End Failed");
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

void loop()
{
  server.handleClient();
  server1.handleClient();
  server2.handleClient();
  ArduinoOTA.handle();

  if (transparentMode)
  {
    transparent();
  }
  // else
  // {
  if (!flashMode1)
  {

    int packetSize = udp.parsePacket();

    if (packetSize > 0)
    {
      /*  Serial.println("udp.remotePort()");
        Serial.println(udp.remotePort());
        Serial.println("RECIEVED");
        Serial.println("packetSize");
        Serial.println(packetSize);*/

      sendFrom = 1;
      remoteIp = udp.remoteIP();
      rPort = udp.remotePort();

      /* Serial.println("remoteIP");
        Serial.println(remoteIp);
        Serial.println("remotePort");
        Serial.println(rPort);   */

      udp.read(buffer1, bufferSize);
      if (!debug(buffer1, 1))
      {
        //Serial1.flush();
        length2 = 0;
        Serial1.setTimeout(300);
        Serial1.flush();
        Serial1.write(buffer1, packetSize);
      }
    }


    if (Serial1.available())
    {
      Serial1.setTimeout(5);
      uint8_t b[1];
      while (Serial1.readBytes(b, 1))
      {

        buffer2[length2] = b[0];
        length2++;
      }
      /* Serial.println("length2");
        Serial.println(length2);  */

      int packetSize = udp.parsePacket();
      udp.beginPacket(remoteIp, rPort);
      udp.write(buffer2, length2);
      udp.endPacket();
      sendFrom = 1;
      // Serial.println("SENT");
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
     
      remoteIp1 = udp1.remoteIP();
      rPort1 = udp1.remotePort();
      if (!flashMode2)
      {
        udp1.read(buffer11, bufferSize);
        if (!debug(buffer11, 2))
        {
          length21 = 0;
          Serial2.setTimeout(300);
          Serial2.flush();
          Serial2.write(buffer11, packetSize1);
        }
      }
    }

    if (Serial2.available())
    {
      Serial2.setTimeout(5);
      uint8_t b[1];
      while (Serial2.readBytes(b, 1))
      {

        buffer21[length21] = b[0];
        length21++;
      }
      /* Serial.println("length2");
        Serial.println(length2);  */

      int packetSize1 = udp1.parsePacket();
      udp1.beginPacket(remoteIp1, rPort1);
      udp1.write(buffer21, length21);
      udp1.endPacket();
      sendFrom = 1;
      // Serial.println("SENT");
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
      udp1.read(buffer21, bufferSize);
      udp1.beginPacket(remoteIp1, rPort1);
      udp1.write(reject, 7);
      udp1.endPacket();
    }
  }
#endif
}

void setDefault()
{
  preferences.begin("apsettings", false);
  preferences.clear();
  preferences.end();
}

void serverStart()
{
  MDNS.begin("esp32");

  server.begin();
  serverHandle();
}

void serverStart1()
{
  server1.on("/", serverHandle1);
  server1.begin();
}

void serverStart2()
{
  server2.on("/", serverHandle2);
  server2.begin();
}

int parseString(String str, uint8_t *buff)
{
  str.toUpperCase();
  if (str.length() % 2 != 0)
  {

    return 0;
  }
  else
  {

    int k = 0;
    for (int i = 0; i < str.length(); i += 2)
    {
      /* Serial.println("K");
        Serial.println(k);
        if ((uint8_t)str[i] < 48 || (uint8_t)str[i] > 57 || (uint8_t)str[i + 1] < 48 || (uint8_t)str[i + 1] > 57)
        {
        Serial.println("ovde");
        return 0;
        }
        if ((uint8_t)str[i] < 65 || (uint8_t)str[i] > 70 || (uint8_t)str[i + 1] < 65 || (uint8_t)str[i + 1] > 70)
        {
        Serial.println("ovde1");
        return 0;
        }*/
      buff[k] = strtoul(str.substring(i, i + 2).c_str(), NULL, 16);
      k++;
    }
    return 1;
  }
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
  server.serveStatic("/", SPIFFS, "/www/login.html");
  server.serveStatic("/main", SPIFFS, "/www/main.html");
  server.serveStatic("/jquery.min.js", SPIFFS, "/www/jquery.min.js");
  server.serveStatic("/script.js", SPIFFS, "/www/script.js");
  server.serveStatic("/style.css", SPIFFS, "/www/style.css");
  server.serveStatic("/jquery.dialog.min.css", SPIFFS, "/www/jquery.dialog.min.css");
  server.serveStatic("/jquery.dialog.min.js", SPIFFS, "/www/jquery.dialog.min.js");

  server.on("/toggletransparent", HTTP_GET, []() {
    if (!server.authenticate(http_username.c_str(), http_password.c_str()))
      return server.requestAuthentication();
    preferences.begin("apsettings", false);
    uint8_t t = preferences.getUInt("transparent", 1);
    preferences.putUInt("transparent", !t);
    preferences.end();
    transparentMode = !t;
    server.send(200, "text/plain", String(!t));

  });

  server.on("/changetransparent", HTTP_GET, []() {
    if (!server.authenticate(http_username.c_str(), http_password.c_str()))
      return server.requestAuthentication();
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
    server.send(200, "text/plain", String(d));
    /*  digitalWrite(LED_BUILTIN, LOW);
      ESP.restart();*/

  });

  server.on("/changeap", HTTP_POST, []() {
    if (!server.authenticate(http_username.c_str(), http_password.c_str()))
      return server.requestAuthentication();
    int paramsNr = server.args();
    String ap = "";
    String ps = "";
    for (int i = 0; i < paramsNr; i++)
    {

      if (server.argName(i) == "ssid")
      {
        ap = server.arg(i);
      }
      else if (server.argName(i) == "pass")
      {
        ps = server.arg(i);
      }
    }

    if (ap.length() > 0 && ps.length() > 7)
    {
      preferences.begin("apsettings", false);
      preferences.putString("ssid", ap);
      preferences.putString("password", ps);
      preferences.end();
      server.send(200, "text/plain", "1");
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);

      ESP.restart();
    }
    else
    {
      server.send(200, "text/plain", "0");
    }
  });

  server.on("/changeauth", HTTP_POST, []() {
    if (!server.authenticate(http_username.c_str(), http_password.c_str()))
      return server.requestAuthentication();
    int paramsNr = server.args();
    String httpuser = "";
    String httppass = "";
    for (int i = 0; i < paramsNr; i++)
    {


      if (server.argName(i) == "username")
      {
        httpuser = server.arg(i);
      }
      else if (server.argName(i) == "password")
      {
        httppass = server.arg(i);
      }
    }

    if (httpuser.length() > 0 && httppass.length() > 0)
    {
      preferences.begin("apsettings", false);
      preferences.putString("http_username", httpuser);
      preferences.putString("http_password", httppass);
      preferences.end();
      server.send(200, "text/plain", "1");
      http_username = httpuser;
      http_password = httppass;
    }
    else
    {
      server.send(200, "text/plain", "0");
    }
  });

  server.on("/auth", HTTP_POST, []() {
    int paramsNr = server.args();
    String httpuser = "";
    String httppass = "";
    for (int i = 0; i < paramsNr; i++)
    {


      if (server.argName(i) == "username")
      {
        httpuser = server.arg(i);
      }
      else if (server.argName(i) == "password")
      {
        httppass = server.arg(i);
      }
    }

    preferences.begin("apsettings", false);
    if (preferences.getString("http_username", "ufr") == httpuser && preferences.getString("http_password", "ufr") == httppass)
    {
      server.send(200, "text/plain", "1");
    }
    else
    {
      server.send(403, "text/plain", "1");
    }

    preferences.end();
  });

  server.on("/changesta", HTTP_POST, []() {
    if (!server.authenticate(http_username.c_str(), http_password.c_str()))
      return server.requestAuthentication();
    int paramsNr = server.args();
    String ap = "";
    String ps = "";
    for (int i = 0; i < paramsNr; i++)
    {


      if (server.argName(i) == "ssid")
      {
        ap = server.arg(i);
      }
      else if (server.argName(i) == "pass")
      {
        ps = server.arg(i);
      }
    }

    if (ap.length() > 0 && ps.length() > 7)
    {
      preferences.begin("apsettings", false);
      preferences.putString("ssidSTA", ap);
      preferences.putString("passwordSTA", ps);
      preferences.end();
      WiFi.disconnect(true);
      delay(500);
      WiFi.begin(ap.c_str(), ps.c_str());
      delay(500);
      uint8_t wifiTimeout = 10;
      while (WiFi.status() != WL_CONNECTED && wifiTimeout > 0)
      {
        delay(1000);
        wifiTimeout--;
      }
      if (WiFi.status() != WL_CONNECTED && wifiTimeout <= 0)
      {
        server.send(200, "text/plain", "0");
      }
      else if (WiFi.status() == WL_CONNECTED)
      {
        server.send(200, "text/plain", "1");
      }

      /*digitalWrite(LED_BUILTIN, LOW);
        ESP.restart();*/
    }
    else
    {
      server.send(200, "text/plain", "6");
    }
  });

  server.on("/scan", HTTP_GET, []() {
    if (!server.authenticate(http_username.c_str(), http_password.c_str()))
      return server.requestAuthentication();
    int n = WiFi.scanNetworks();
    String output = "{\"result\":[";
    for (int i = 0; i < n; ++i)
    {

      output += "{  \"ssid\":\"" + WiFi.SSID(i) + "\",\"signal\":" + WiFi.RSSI(i) + ",\"channel\":" + WiFi.channel(i) + ",\"encryption\":\"" + translateEncryptionType(WiFi.encryptionType(i)) + "\"},";
    }

    server.send(200, "application/json", output.substring(0, output.length() - 1) + "]}");
  });

  server.on("/info", HTTP_GET, []() {
    if (!server.authenticate(http_username.c_str(), http_password.c_str()))
      return server.requestAuthentication();
    String output = "{\"status\":\"disconnected\"";
    int n = WiFi.status();

    if (n == 3)
    {
#ifdef DUAL_MODE
      output = "{\"status\":\"connected\",\"ssid\":\"" + WiFi.SSID() + "\",\"ip\":\"" + WiFi.localIP().toString() + "\",\"mac\":\"" + WiFi.macAddress() + "\",\"port1\":" + SERIAL1_UDP_PORT + ",\"port2\":" + SERIAL2_UDP_PORT;
#else
      output = "{\"status\":\"connected\",\"ssid\":\"" + WiFi.SSID() + "\",\"ip\":\"" + WiFi.localIP().toString() + "\",\"mac\":\"" + WiFi.macAddress() + "\",\"port1\":" + SERIAL1_UDP_PORT;

#endif
    }
    preferences.begin("apsettings", false);

    String both = ", \"transparent\":" + String(preferences.getUInt("transparent", 1)) + ", \"device\":" + String(preferences.getUInt("device", 1)) + ", \"ap\":\"" + preferences.getString("ssid", String(ssidAP)).c_str() + "\", \"pass\":\"" + preferences.getString("password", String(passwordAP)).c_str() + "\"";
    preferences.end();

    server.send(200, "application/json", output + both + "}");
  });

  server.on("/setport", HTTP_POST, []() {
    if (!server.authenticate(http_username.c_str(), http_password.c_str()))
      return server.requestAuthentication();
    int paramsNr = server.args();
    int p1;
#ifdef DUAL_MODE
    int p2;
#endif
    for (int i = 0; i < paramsNr; i++)
    {

      if (server.argName(i) == "port1")
      {
        p1 = server.arg(i).toInt();
      }
#ifdef DUAL_MODE
      else if (server.argName(i) == "port2")
      {
        p2 = server.arg(i).toInt();
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
      udp.begin(p1);
#ifdef DUAL_MODE
      preferences.putInt("port2", p2);
      udp1.begin(p2);

#endif
      preferences.end();

      server.send(200, "text/plain", "1");
      /*      digitalWrite(LED_BUILTIN, LOW);
        ESP.restart();*/
    }
    else
    {
      server.send(200, "text/plain", "0");
    }
  });

  server.on("/disconnect", HTTP_GET, []() {
    if (!server.authenticate(http_username.c_str(), http_password.c_str()))
      return server.requestAuthentication();
    preferences.begin("apsettings", false);
    preferences.putString("ssidSTA", "");
    preferences.putString("passwordSTA", "");
    preferences.end();
    WiFi.disconnect(true);
    delay(500);
    server.send(200, "text/plain", "1");
    /*   digitalWrite(LED_BUILTIN, LOW);
      ESP.restart();*/
  });

  server.on("/restart", HTTP_GET, []() {
    if (!server.authenticate(http_username.c_str(), http_password.c_str()))
      return server.requestAuthentication();
    server.send(200, "text/plain", "1");
    digitalWrite(LED_BUILTIN, LOW);

    ESP.restart();
  });

  server.on("/setdefault", HTTP_GET, []() {
    if (!server.authenticate(http_username.c_str(), http_password.c_str()))
      return server.requestAuthentication();
    setDefault();
    server.send(200, "text/plain", "1");
    digitalWrite(LED_BUILTIN, LOW);

    ESP.restart();
  });

  server.on("/alive", HTTP_GET, []() {
    if (!server.authenticate(http_username.c_str(), http_password.c_str()))
      return server.requestAuthentication();
    server.send(200, "text/plain", "1");
    digitalWrite(LED_BUILTIN, LOW);
  });

  server.on("/identify", HTTP_GET, []() {
    if (!server.authenticate(http_username.c_str(), http_password.c_str()))
      return server.requestAuthentication();
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    server.send(200, "text/plain", "1");
  });

}

void serverHandle1()
{

  sendFrom = 1;
  Serial1.setTimeout(300);
  int paramsNr = server1.args();
  String response = "";

  if (paramsNr != 1)
  {
    server1.send(400);
  }
  else
  {

    if (parseString(server1.arg(0), buffer1h))
    {
      Serial1.flush();

      Serial1.write(buffer1h, server1.arg(0).length() / 2);

      //delay(10);


      uint8_t b[1];
      while (Serial1.readBytes(b, 1))
      {
        Serial1.setTimeout(5);
        response += String(b[0], HEX);
      }
      /* Serial.println("length2");
        Serial.println(length2);  */



      sendFrom = 1;
      // Serial.println("SENT");

      server1.send(200, "text/plain", response);
    }
    else
    {
      server1.send(400);
    }
  }

}

void serverHandle2()
{

  sendFrom = 1;
  Serial2.setTimeout(300);
  int paramsNr = server2.args();
  String response = "";

  if (paramsNr != 1)
  {
    server2.send(400);
  }
  else
  {

    if (parseString(server2.arg(0), buffer2h))
    {
      Serial2.flush();

      Serial2.write(buffer2h, server2.arg(0).length() / 2);




      uint8_t b[1];
      while (Serial2.readBytes(b, 1))
      {
        Serial2.setTimeout(5);
        response += String(b[0], HEX);
      }
      /* Serial.println("length2");
        Serial.println(length2);  */



      sendFrom = 1;
      // Serial.println("SENT");

      server2.send(200, "text/plain", response);
    }
    else
    {
      server2.send(400);
    }
  }
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
    if (checkChange(bufferTransparent, out) == false)
    {
      transparentSerial[transparentDevice - 1].write(bufferTransparent, lengthTransparent);
      transparentSerial[transparentDevice - 1].flush();

      lengthTransparent = 0;
    }
    else
    {
      Serial.flush();
      Serial.write(out, 7);

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
      Serial.flush();
      Serial.write(buffer1, length1);

      length1 = 0;
    }
  }
}
