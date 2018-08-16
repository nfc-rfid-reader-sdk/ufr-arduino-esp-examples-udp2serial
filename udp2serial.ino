#include <FS.h>
#include "SPIFFS.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Preferences.h>
#include <WiFiClient.h>

#include <WebServer.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <ESP8266EthernetWebServer.h>
#include <HTTPClient.h>
#include "config.h"
#include "files.h"

void setup()
{

  delay(500);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SET_DEFAULTPIN, INPUT);
  pinMode(WIRED_STATE, INPUT);
  digitalWrite(LED_BUILTIN, LOW);
  if (digitalRead(SET_DEFAULTPIN) == HIGH)
  {
    setDefault();
  }

  if (digitalRead(WIRED_STATE) == HIGH)
  {
    wireless = 1;
  }
  else
  {
    wireless = 0;
  }

  serialSetup();

  //otaSetup();

  if (wireless)
  {
    wifiSetup();

  }
  else
  {
    ethernetSetup();

  }


  xTaskCreatePinnedToCore(core1, "Core1", 100000, NULL, 1, NULL, 0);

  xTaskCreatePinnedToCore(core2, "Core2", 10000, NULL, 1, NULL, 1);

}

void core1(void *p)
{
  while (1)
  {
    if (wireless)
    {
      core1Wifi();
    }
    else
    {
      core1Ethernet();
    }
  }
}

void core1Wifi()
{
  if (master)
  {
    core1WifiMaster();
  }
  else
  {
    core1WifiSlave();
  }

}

void core1WifiMaster()
{
  server1.handleClient();

  if (transparentMode == 1 && transparentDevice == 1)
  {
    transparent();
  }

  int packetSize = udpWifi.parsePacket();
  if (packetSize > 0)
  {
    sendFrom = 1;
    remoteIp = udpWifi.remoteIP();
    rPort = udpWifi.remotePort();

    udpWifi.read(buffer1, bufferSize);
    if (!debug(buffer1, 1))
    {
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

    if (sendFrom == 1)
    {
      int packetSize = udpWifi.parsePacket();
      udpWifi.flush();

      udpWifi.beginPacket(remoteIp, rPort);
      udpWifi.write(buffer2, length2);
      udpWifi.endPacket();
      sendFrom1 = 10;
    }
  }

  vTaskDelay(1);
}
String tmp = "";

void core1WifiSlave()
{

  uint8_t cardID[10];

  uint8_t length = 0;
  uint8_t code = getCardID1(cardID, &length);
  if (code == 0)
  {

    String buff = "";
    for (int i = 0; i < length; i++)
    {
      if (unsigned(cardID[i]) < 16)
      {
        buff += "0";
      }
      buff += String(unsigned(cardID[i]), HEX) + ":";
    }
    buff = buff.substring(0, buff.length() - 1);
    buff.toUpperCase();
    if (!buff.equals(tmp))
    {
      HTTPClient http;

      http.begin(host);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      int httpCode = http.POST("UID=" + buff + "&SN=" + serialNumber1);
      if (httpCode == 200)
      {
        String payload = http.getString();
        uint8_t light = String(payload.charAt(0)).toInt();
        uint8_t beep = String(payload.charAt(2)).toInt();
        uiSignal1(light, beep);

        vTaskDelay(pdMS_TO_TICKS(10));
        tmp = buff;
        http.end();
      }
      else
      {
        uiSignal1(4, 4);
        vTaskDelay(pdMS_TO_TICKS(10));
        tmp = buff;
        http.end();
      }
    }
  }
  else
  {
    tmp = "";
  }

  vTaskDelay(pdMS_TO_TICKS(50));

}

void core1Ethernet()
{
  if (master)
  {
    core1EthernetMaster();
  }
}

void core1EthernetMaster()
{
  ethernetServer1.handleClient();

  if (transparentMode == 1 && transparentDevice == 1)
  {
    transparent();
  }
  int packetSize = udpEthernet.parsePacket();
  if (packetSize > 0)
  {
    sendFrom = 1;
    remoteIp = udpEthernet.remoteIP();
    rPort = udpEthernet.remotePort();
    udpEthernet.read(buffer1, bufferSize);
    if (!debug(buffer1, 1))
    {
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

    if (sendFrom == 1)
    {
      int packetSize = udpEthernet.parsePacket();

      udpEthernet.flush();
      udpEthernet.beginPacket(remoteIp.toString().c_str(), rPort);
      udpEthernet.write(buffer2, length2);
      udpEthernet.endPacket();
      sendFrom1 = 10;
    }
  }
  vTaskDelay(1);
}

void core2(void *p)
{
  while (1)
  {
    if (wireless)
    {
      core2Wifi();
    }
    else
    {
      core2Ethernet();
    }
  }
}

void core2Wifi()
{
  if (master)
  {
    core2WifiMaster();
  }
  else
  {
    core2WifiSlave();
  }
}

void core2WifiMaster()
{
  server2.handleClient();

  if (transparentMode == 1 && transparentDevice == 2)
  {
    transparent();
  }
#ifdef DUAL_MODE

  int packetSize1 = udpWifi1.parsePacket();
  if (packetSize1 > 0)
  {
    sendFrom1 = 1;
    remoteIp1 = udpWifi1.remoteIP();
    rPort1 = udpWifi1.remotePort();

    udpWifi1.read(buffer11, bufferSize);
    if (!debug(buffer11, 2))
    {
      length21 = 0;
      Serial2.setTimeout(300);
      Serial2.flush();
      Serial2.write(buffer11, packetSize1);
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

    if (sendFrom1 == 1)
    {
      int packetSize1 = udpWifi1.parsePacket();
      udpWifi1.flush();

      udpWifi1.beginPacket(remoteIp1, rPort1);
      udpWifi1.write(buffer21, length21);
      udpWifi1.endPacket();
      sendFrom1 = 10;
    }
  }

#endif
  vTaskDelay(1);
}

String tmp1 = "";

void core2WifiSlave()
{

  uint8_t cardID[10];

  uint8_t length = 0;
  uint8_t code = getCardID2(cardID, &length);
  if (code == 0)
  {

    String buff = "";
    for (int i = 0; i < length; i++)
    {
      if (unsigned(cardID[i]) < 16)
      {
        buff += "0";
      }
      buff += String(unsigned(cardID[i]), HEX) + ":";
    }
    buff = buff.substring(0, buff.length() - 1);
    buff.toUpperCase();
    if (!buff.equals(tmp1))
    {
      HTTPClient http;

      http.begin(host);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      int httpCode = http.POST("UID=" + buff + "&SN=" + serialNumber2);
      if (httpCode == 200)
      {
        String payload = http.getString();
        uint8_t light = String(payload.charAt(0)).toInt();
        uint8_t beep = String(payload.charAt(2)).toInt();
        uiSignal2(light, beep);

        vTaskDelay(pdMS_TO_TICKS(10));
        tmp1 = buff;
        http.end();
      }
      else
      {
        uiSignal2(4, 4);
        vTaskDelay(pdMS_TO_TICKS(10));
        tmp1 = buff;
        http.end();
      }
    }
  }
  else
  {
    tmp1 = "";
  }

  vTaskDelay(pdMS_TO_TICKS(50));

}

void core2Ethernet()
{
  if (master)
  {
    core2EthernetMaster();
  }
}

void core2EthernetMaster()
{
  ethernetServer2.handleClient();

  if (transparentMode == 1 && transparentDevice == 2)
  {
    transparent();
  }
#ifdef DUAL_MODE

  int packetSize1 = udpEthernet1.parsePacket();
  if (packetSize1 > 0)
  {
    sendFrom1 = 1;
    remoteIp1 = udpEthernet1.remoteIP();
    rPort1 = udpEthernet1.remotePort();

    udpEthernet1.read(buffer11, bufferSize);
    if (!debug(buffer11, 2))
    {
      length21 = 0;
      Serial2.setTimeout(300);
      Serial2.flush();
      Serial2.write(buffer11, packetSize1);
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

    if (sendFrom1 == 1)
    {
      int packetSize1 = udpEthernet1.parsePacket();
      //udpEthernet1.flush();

      udpEthernet1.beginPacket(remoteIp1.toString().c_str(), rPort1);
      udpEthernet1.write(buffer21, length21);
      udpEthernet1.endPacket();
      sendFrom1 = 10;
    }
  }

#endif
  vTaskDelay(1);
}

void loop()
{

  if (wireless)
  {
    server.handleClient();
    //ArduinoOTA.handle();
  }
  else
  {
    ethernetServer.handleClient();
  }

  if (digitalRead(WIRED_STATE) != wireless)
  {
    digitalWrite(LED_BUILTIN, LOW);
    ESP.restart();
  }

  if (digitalRead(SET_DEFAULTPIN) == HIGH)
  {

    ESP.restart();
  }


}

void setDefault()
{
  delay(3000);
  if (digitalRead(SET_DEFAULTPIN) == HIGH)
  {
    preferences.begin("apsettings", false);
    preferences.clear();
    preferences.end();
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    ESP.restart();
  }
}

void serverStart()
{
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

void ethernetServerStart()
{

  ethernetServer.begin();
  ethernetServerHandle();
}

void ethernetServerStart1()
{
  ethernetServer1.on("/", ethernetServerHandle1);
  ethernetServer1.begin();
}

void ethernetServerStart2()
{
  ethernetServer2.on("/", ethernetServerHandle2);
  ethernetServer2.begin();
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
      buff[k] = strtoul(str.substring(i, i + 2).c_str(), NULL, 16);
      k++;
    }
    return 1;
  }
}

String modeToString()
{
  if (wireless)
  {
    return "wifi";
  }
  else
  {
    return "ethernet";
  }
}

int parseString1(String str, uint8_t *buff)
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
      buff[k] = strtoul(str.substring(i, i + 2).c_str(), NULL, 16);
      k++;
    }
    return 1;
  }
}
void serverHandle()
{

  /* server.on("/", HTTP_GET, []() {
     server.send(200, "text/html", String(login_html));
    });
    server.on("/fader.js", HTTP_GET, []() {
     server.send(200, "text/javascript", String(fader_js));
    });
    server.on("/nanoajax.js", HTTP_GET, []() {
     server.send(200, "text/javascript", String(nanoajax_js));
    });
    server.on("/domready.js", HTTP_GET, []() {
     server.send(200, "text/javascript", String(domready_js));
    });
    server.on("/login.js", HTTP_GET, []() {
     server.send(200, "text/javascript", String(login_js));
    });
    server.on("/login.css", HTTP_GET, []() {
     server.send(200, "text/css", String(login_css));
    });
    server.on("/main", HTTP_GET, []() {
     server.send(200, "text/html", String(main_html));
    });
    server.on("/main.css", HTTP_GET, []() {
     server.send(200, "text/css", String(main_css));
    });
    server.on("/main1.css", HTTP_GET, []() {
     server.send(200, "text/css", String(main1_css));
    });
    server.on("/main2.css", HTTP_GET, []() {
     server.send(200, "text/css", String(main2_css));
    });
    server.on("/dom.js", HTTP_GET, []() {
     server.send(200, "text/javascript", String(dom_js));
    });

    server.on("/main.js", HTTP_GET, []() {
     server.send(200, "text/javascript", String(main_js));
    });
    server.on("/main1.js", HTTP_GET, []() {
     server.send(200, "text/javascript", String(main1_js));
    });
    server.on("/main2.js", HTTP_GET, []() {
     server.send(200, "text/javascript", String(main2_js));
    });
    server.on("/mainwifionly.js", HTTP_GET, []() {
     server.send(200, "text/javascript", String(mainwifionly_js));
    });*/

  server.serveStatic("/", SPIFFS, "/www/login.html");
  /* server.serveStatic("/domready.js", SPIFFS, "/www/domready.js");
    server.serveStatic("/nanoajax.js", SPIFFS, "/www/nanoajax.js");*/

  server.serveStatic("/main", SPIFFS, "/www/main.html");
  server.serveStatic("/jquery.min.js", SPIFFS, "/www/jquery.min.js");
  server.serveStatic("/script.js", SPIFFS, "/www/script.js");
  server.serveStatic("/style.css", SPIFFS, "/www/style.css");
  server.serveStatic("/jquery.dialog.min.css", SPIFFS, "/www/jquery.dialog.min.css");
  server.serveStatic("/jquery.dialog.min.js", SPIFFS, "/www/jquery.dialog.min.js");


   server.on("/togglemode", HTTP_GET, []() {
     if (!server.authenticate(http_username.c_str(), http_password.c_str()))
       return server.requestAuthentication();
     preferences.begin("apsettings", false);
     uint8_t m = preferences.getUInt("master", 1);
     preferences.putUInt("master", !m);
     preferences.end();
     master = !m;
     server.send(200, "text/plain", String(!m));
     ESP.restart();
    });

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

   server.on("/changehost", HTTP_POST, []() {
    if (!server.authenticate(http_username.c_str(), http_password.c_str()))
      return server.requestAuthentication();
    int paramsNr = server.args();
    String h = "";
  
    for (int i = 0; i < paramsNr; i++)
    {

      if (server.argName(i) == "host")
      {
        h = server.arg(i);
      }
    
    }

    if (h.length() > 0)
    {
      preferences.begin("apsettings", false);
      preferences.putString("host", h);
      preferences.end();
      host = h;
      server.send(200, "text/plain", "1");
      delay(100);
      
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
    digitalWrite(LED_BUILTIN, LOW);
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
        digitalWrite(LED_BUILTIN, HIGH);
      }
    }
    else
    {
      server.send(200, "text/plain", "0");
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
    String output = "{\"mode\":\"" + modeToString() + "\",\"status\":\"disconnected\"";
    int n = WiFi.status();

    if (n == 3)
    {
#ifdef DUAL_MODE
      output = "{\"mode\":\"" + modeToString() + "\",\"status\":\"connected\",\"ssid\":\"" + WiFi.SSID() + "\",\"ip\":\"" + WiFi.localIP().toString() + "\",\"mac\":\"" + WiFi.macAddress() + "\",\"port1\":" + SERIAL1_UDP_PORT + ",\"port2\":" + SERIAL2_UDP_PORT;
#else
      output = "{\"mode\":\"" + modeToString() + "\",\"status\":\"connected\",\"ssid\":\"" + WiFi.SSID() + "\",\"ip\":\"" + WiFi.localIP().toString() + "\",\"mac\":\"" + WiFi.macAddress() + "\",\"port1\":" + SERIAL1_UDP_PORT;

#endif
    }
    preferences.begin("apsettings", false);

    String both = ", \"transparent\":" + String(preferences.getUInt("transparent", 1)) + ", \"device\":" + String(preferences.getUInt("device", 1)) + ", \"serial1\":\"" + serialNumber1 + "\", \"serial2\":\"" + serialNumber2 + "\", \"ap\":\"" + preferences.getString("ssid", serialNumber1 + "/" + serialNumber2).c_str() + "\", \"pass\":\"" + preferences.getString("password", String(passwordAP)).c_str() + "\", \"authuser\":\"" + http_username.c_str() + "\", \"authpass\":\"" + http_password.c_str() + "\"";
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
      SERIAL1_UDP_PORT = p1;
      SERIAL2_UDP_PORT = p2;
      preferences.begin("apsettings", false);
      preferences.putInt("port1", p1);
      if (wireless)
      {
        udpWifi.begin(p1);
      }
      else
      {
        udpEthernet.begin(p1);
      }

#ifdef DUAL_MODE
      preferences.putInt("port2", p2);
      if (wireless)
      {
        udpWifi1.begin(p1);
      }
      else
      {
        udpEthernet1.begin(p1);
      }

#endif
      preferences.end();

      server.send(200, "text/plain", "1");

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

  Serial1.setTimeout(300);
  int paramsNr = server1.args();
  String response = "";

  if (paramsNr < 1)
  {
    server1.send(400);
  }
  else
  {
    if (parseString(server1.arg(0), buffer1h))
    {
      Serial1.flush();

      Serial1.write(buffer1h, server1.arg(0).length() / 2);

      uint8_t b[1];
      while (Serial1.readBytes(b, 1))
      {
        Serial1.setTimeout(5);
        response += String(b[0], HEX);
      }

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

  Serial2.setTimeout(300);
  int paramsNr = server2.args();
  String response = "";

  if (paramsNr != 1)
  {
    server2.send(400);
  }
  else
  {

    if (parseString1(server2.arg(0), buffer2h))
    {
      Serial2.flush();

      Serial2.write(buffer2h, server2.arg(0).length() / 2);

      uint8_t b[1];
      while (Serial2.readBytes(b, 1))
      {
        Serial2.setTimeout(5);
        response += String(b[0], HEX);
      }

      server2.send(200, "text/plain", response);
    }
    else
    {
      server2.send(400);
    }
  }
}

void ethernetServerHandle()
{

  /* ethernetServer.on("/togglemode", HTTP_GET, []() {
     if (!ethernetServer.authenticate(http_username.c_str(), http_password.c_str()))
       return ethernetServer.requestAuthentication();
     preferences.begin("apsettings", false);
     uint8_t m = preferences.getUInt("master", 1);
     preferences.putUInt("master", !m);
     preferences.end();
     master = !m;
     ethernetServer.send(200, "text/plain", String(!m));
     ESP.restart();
    });*/

  ethernetServer.on("/", HTTP_GET, []() {
    ethernetServer.send(200, "text/html", String(login_html));
  });

  ethernetServer.on("/login.css", HTTP_GET, []() {
    ethernetServer.send(200, "text/css", String(login_css));
  });

  ethernetServer.on("/login.js", HTTP_GET, []() {
    ethernetServer.send(200, "text/javascript", String(login_js));
  });

  ethernetServer.on("/main", HTTP_GET, []() {
    ethernetServer.send(200, "text/html", String(main_html));
  });

  ethernetServer.on("/style.css", HTTP_GET, []() {
    ethernetServer.send(200, "text/css", String(style_css));
  });

  ethernetServer.on("/script.js", HTTP_GET, []() {
    ethernetServer.send(200, "text/javascript", String(script_js));
  });

  ethernetServer.on("/script1.js", HTTP_GET, []() {
    ethernetServer.send(200, "text/javascript", String(script1_js));
  });

  ethernetServer.on("/script2.js", HTTP_GET, []() {
    ethernetServer.send(200, "text/javascript", String(script2_js));
  });

  ethernetServer.on("/toggletransparent", HTTP_GET, []() {
    if (!ethernetServer.authenticate(http_username.c_str(), http_password.c_str()))
      return ethernetServer.requestAuthentication();
    preferences.begin("apsettings", false);
    uint8_t t = preferences.getUInt("transparent", 1);
    preferences.putUInt("transparent", !t);
    preferences.end();
    transparentMode = !t;
    ethernetServer.send(200, "text/plain", String(!t));
  });
  ethernetServer.on("/changetransparent", HTTP_GET, []() {
    if (!ethernetServer.authenticate(http_username.c_str(), http_password.c_str()))
      return ethernetServer.requestAuthentication();
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
    ethernetServer.send(200, "text/plain", String(d));
  });

  ethernetServer.on("/changeauth", HTTP_POST, []() {
    if (!ethernetServer.authenticate(http_username.c_str(), http_password.c_str()))
      return ethernetServer.requestAuthentication();
    int paramsNr = ethernetServer.args();
    String httpuser = "";
    String httppass = "";
    for (int i = 0; i < paramsNr; i++)
    {

      if (ethernetServer.argName(i) == "username")
      {
        httpuser = ethernetServer.arg(i);
      }
      else if (ethernetServer.argName(i) == "password")
      {
        httppass = ethernetServer.arg(i);
      }
    }

    if (httpuser.length() > 0 && httppass.length() > 0)
    {
      preferences.begin("apsettings", false);
      preferences.putString("http_username", httpuser);
      preferences.putString("http_password", httppass);
      preferences.end();
      ethernetServer.send(200, "text/plain", "1");
      http_username = httpuser;
      http_password = httppass;
    }
    else
    {
      ethernetServer.send(200, "text/plain", "0");
    }
  });

  ethernetServer.on("/auth", HTTP_POST, []() {
    int paramsNr = ethernetServer.args();
    String httpuser = "";
    String httppass = "";
    for (int i = 0; i < paramsNr; i++)
    {

      if (ethernetServer.argName(i) == "username")
      {

        httpuser = ethernetServer.arg(i);
      }
      else if (ethernetServer.argName(i) == "password")
      {
        httppass = ethernetServer.arg(i);
      }
    }
    if (httpuser == "" && ethernetServer.arg(0).length() > 15)
    {
      httpuser = ethernetServer.arg(0).substring(9, ethernetServer.arg(0).indexOf("&"));
      httppass = ethernetServer.arg(0).substring(ethernetServer.arg(0).indexOf("&") + 10);
    }

    preferences.begin("apsettings", false);
    if (preferences.getString("http_username", "ufr") == httpuser && preferences.getString("http_password", "ufr") == httppass)
    {
      ethernetServer.send(200, "text/plain", "1");
    }
    else
    {
      ethernetServer.send(403, "text/plain", "1");
    }

    preferences.end();
  });

  ethernetServer.on("/info", HTTP_GET, []() {
    if (!ethernetServer.authenticate(http_username.c_str(), http_password.c_str()))
      return ethernetServer.requestAuthentication();
    String output = "";
#ifdef DUAL_MODE
    output = "{\"mode\":\"" + modeToString() + "\",\"status\":\"connected\",\"ip\":\"" + Ethernet.localIP().toString() + "\",\"mac\":\"" + ethernetMacToString() + "\",\"port1\":" + String(preferences.getUInt("port1", 8881)) + ",\"port2\":" + String(preferences.getUInt("port2", 8882));
#else
    output = "{\"mode\":\"" + modeToString() + "\",\"status\":\"connected\",\"ip\":\"" + Ethernet.localIP().toString() + "\",\"mac\":\"" + ethernetMacToString() + "\",\"port1\":" + String(preferences.getUInt("port1", 8881));

#endif

    preferences.begin("apsettings", false);

    String both = ", \"transparent\":" + String(preferences.getUInt("transparent", 1)) + ", \"device\":" + String(preferences.getUInt("device", 1)) + ", \"authuser\":\"" + http_username.c_str() + "\", \"authpass\":\"" + http_password.c_str() + "\"";
    preferences.end();

    ethernetServer.send(200, "application/json", output + both + "}");
  });

  ethernetServer.on("/setport", HTTP_POST, []() {
    if (!ethernetServer.authenticate(http_username.c_str(), http_password.c_str()))
      return ethernetServer.requestAuthentication();
    int paramsNr = ethernetServer.args();
    int p1 = 0;
#ifdef DUAL_MODE
    int p2;
#endif
    for (int i = 0; i < paramsNr; i++)
    {

      if (ethernetServer.argName(i) == "port1")
      {
        p1 = ethernetServer.arg(i).toInt();
      }
#ifdef DUAL_MODE
      else if (ethernetServer.argName(i) == "port2")
      {
        p2 = ethernetServer.arg(i).toInt();
      }
#endif
    }

    if (p1 == 0 && ethernetServer.arg(0).length() > 10)
    {
      p1 = ethernetServer.arg(0).substring(6, ethernetServer.arg(0).indexOf("&")).toInt();
      p2 = ethernetServer.arg(0).substring(ethernetServer.arg(0).indexOf("&") + 7).toInt();
    }

#ifdef DUAL_MODE
    if (p1 > 0 && p1 < 65536 && p2 > 0 && p2 < 65536 && p1 != p2)

#else
    if (p1 > 0 && p1 < 65536)
#endif
    {
      SERIAL1_UDP_PORT = p1;
      SERIAL2_UDP_PORT = p2;
      preferences.begin("apsettings", false);
      preferences.putInt("port1", p1);
      if (wireless)
      {
        udpWifi.begin(p1);
      }
      else
      {
        udpEthernet.begin(p1);
      }

#ifdef DUAL_MODE
      preferences.putInt("port2", p2);
      if (wireless)
      {
        udpWifi1.begin(p1);
      }
      else
      {
        udpEthernet1.begin(p1);
      }

#endif
      preferences.end();

      ethernetServer.send(200, "text/plain", "1");
    }
    else
    {
      ethernetServer.send(200, "text/plain", "0");
    }
  });

  ethernetServer.on("/restart", HTTP_GET, []() {
    if (!ethernetServer.authenticate(http_username.c_str(), http_password.c_str()))
      return ethernetServer.requestAuthentication();
    ethernetServer.send(200, "text/plain", "1");
    digitalWrite(LED_BUILTIN, LOW);

    ESP.restart();
  });

  ethernetServer.on("/setdefault", HTTP_GET, []() {
    if (!ethernetServer.authenticate(http_username.c_str(), http_password.c_str()))
      return ethernetServer.requestAuthentication();
    setDefault();
    ethernetServer.send(200, "text/plain", "1");
    digitalWrite(LED_BUILTIN, LOW);

    ESP.restart();
  });


  ethernetServer.on("/identify", HTTP_GET, []() {
    if (!ethernetServer.authenticate(http_username.c_str(), http_password.c_str()))
      return ethernetServer.requestAuthentication();
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    ethernetServer.send(200, "text/plain", "1");
  });


}

void ethernetServerHandle1()
{

  Serial1.setTimeout(300);
  int paramsNr = ethernetServer1.args();
  String response = "";

  if (paramsNr < 1)
  {
    server1.send(400);
  }
  else
  {
    if (parseString(ethernetServer1.arg(0), buffer1h))
    {
      Serial1.flush();

      Serial1.write(buffer1h, ethernetServer1.arg(0).length() / 2);

      uint8_t b[1];
      while (Serial1.readBytes(b, 1))
      {
        Serial1.setTimeout(5);
        response += String(b[0], HEX);
      }

      ethernetServer1.send(200, "text/plain", response);
    }
    else
    {

      server1.send(400);
    }
  }
}

void ethernetServerHandle2()
{

  Serial2.setTimeout(300);
  int paramsNr = ethernetServer2.args();
  String response = "";

  if (paramsNr != 1)
  {
    ethernetServer2.send(400);
  }
  else
  {

    if (parseString1(ethernetServer2.arg(0), buffer2h))
    {
      Serial2.flush();

      Serial2.write(buffer2h, ethernetServer2.arg(0).length() / 2);

      uint8_t b[1];
      while (Serial2.readBytes(b, 1))
      {
        Serial2.setTimeout(5);
        response += String(b[0], HEX);
      }

      ethernetServer2.send(200, "text/plain", response);
    }
    else
    {
      ethernetServer2.send(400);
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
        vTaskDelay(pdMS_TO_TICKS(10));
        digitalWrite(SERIAL1_RESETPIN, LOW);
      }
      else if (num == 2)
      {
#ifdef DUAL_MODE
        digitalWrite(SERIAL2_RESETPIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(10));
        digitalWrite(SERIAL2_RESETPIN, LOW);

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
        if (wireless)
        {
          udpWifi.beginPacket(remoteIp, rPort);
          udpWifi.write(out, 7);
          udpWifi.endPacket();
        }
        else
        {
          udpEthernet.beginPacket(remoteIp.toString().c_str(), rPort);
          udpEthernet.write(out, 7);
          udpEthernet.endPacket();
        }
      }
      else if (num == 2)
      {
#ifdef DUAL_MODE
        if (wireless)
        {
          udpWifi1.beginPacket(remoteIp1, rPort1);
          udpWifi1.write(out, 7);
          udpWifi1.endPacket();
        }
        else
        {
          udpEthernet1.beginPacket(remoteIp1.toString().c_str(), rPort1);
          udpEthernet1.write(out, 7);
          udpEthernet1.endPacket();
        }

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
        if (wireless)
        {
          udpWifi.beginPacket(remoteIp, rPort);
          udpWifi.write(out, 7);
          udpWifi.endPacket();
        }
        else
        {
          udpEthernet.beginPacket(remoteIp.toString().c_str(), rPort);
          udpEthernet.write(out, 7);
          udpEthernet.endPacket();
        }
      }
      else if (num == 2)
      {
#ifdef DUAL_MODE
        if (wireless)
        {
          udpWifi1.beginPacket(remoteIp1, rPort1);
          udpWifi1.write(out, 7);
          udpWifi1.endPacket();
        }
        else
        {
          udpEthernet1.beginPacket(remoteIp1.toString().c_str(), rPort1);
          udpEthernet1.write(out, 7);
          udpEthernet1.endPacket();
        }
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
    lengthTransparent = 0;
    if (transparentDevice == 1)
    {
      sendFrom = 3;
    }
    else
    {
      sendFrom1 = 3;
    }

    Serial.setTimeout(5);
    uint8_t b[1];
    while (Serial.readBytes(b, 1))
    {

      bufferTransparent[lengthTransparent] = b[0];
      lengthTransparent++;
    }

    if (sendFrom == 3 || sendFrom1 == 3)
    {
      uint8_t out[7];
      if (checkChange(bufferTransparent, out) == false)
      {
        transparentSerial[transparentDevice - 1].flush();
        transparentSerial[transparentDevice - 1].write(bufferTransparent, lengthTransparent);
        lengthTransparent = 0;
      }
      else
      {
        Serial.flush();
        Serial.write(out, 7);
        lengthTransparent = 0;
      }
    }
  }

  if (sendFrom == 3 || sendFrom1 == 3)
  {
    transparentSerial[transparentDevice - 1].setTimeout(300);
    lengthTransparent2 = 0;
    if (transparentSerial[transparentDevice - 1].available())
    {

      transparentSerial[transparentDevice - 1].setTimeout(5);
      uint8_t b[1];
      while (transparentSerial[transparentDevice - 1].readBytes(b, 1))
      {

        bufferTransparent2[lengthTransparent2] = b[0];
        lengthTransparent2++;
      }
      Serial.flush();
      Serial.write(bufferTransparent2, lengthTransparent2);

      lengthTransparent2 = 0;
    }
    if (transparentDevice == 1)
    {
      sendFrom = 10;
    }
    else
    {
      sendFrom1 = 10;
    }
  }
}

void serialSetup()
{
  Serial.begin(115200);

  pinMode(SERIAL1_RESETPIN, OUTPUT);
#ifdef DUAL_MODE
  pinMode(SERIAL2_RESETPIN, OUTPUT);
#endif
  delay(50);
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
  delay(1000);

}

void wifiSetup()
{


  preferences.begin("apsettings", false);

  WiFi.disconnect(true);
  delay(500);
  WiFi.mode(WIFI_AP_STA);
  WiFi.persistent(false);
  WiFi.begin(preferences.getString("ssidSTA", "").c_str(), preferences.getString("passwordSTA", "").c_str());

  master = preferences.getUInt("master", 1);
  transparentMode = preferences.getUInt("transparent", 1);
  transparentDevice = preferences.getUInt("device", 1);
  http_username = preferences.getString("http_username", "ufr");
  http_password = preferences.getString("http_password", "ufr");
  SERIAL1_UDP_PORT = preferences.getInt("port1", 8881);
  host = preferences.getString("host", "");

#ifdef DUAL_MODE
  SERIAL2_UDP_PORT = preferences.getInt("port2", 8882);
#endif

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
  SPIFFS.begin();
  serverStart();

  if (master)
  {
    udpWifi.begin(SERIAL1_UDP_PORT);
#ifdef DUAL_MODE

    udpWifi1.begin(SERIAL2_UDP_PORT);
#endif

    serverStart1();
    serverStart2();

    delay(300);
  }

  delay(200);


  serialNumber1 = getSerialNumber(1);
  delay(20);
  serialNumber2 = getSerialNumber(2);
  WiFi.softAP(preferences.getString("ssid", serialNumber1 + "/" + serialNumber2 ).c_str() , preferences.getString("password", String(passwordAP)).c_str());

  preferences.end();
  if (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }


}

void ethernetSetup()
{
  WiFi.mode(WIFI_MODE_NULL);
  Ethernet.init(5);
  preferences.begin("apsettings", false);
  master = preferences.getUInt("master", 1);
  transparentMode = preferences.getUInt("transparent", 1);
  transparentDevice = preferences.getUInt("device", 1);
  http_username = preferences.getString("http_username", "ufr");
  http_password = preferences.getString("http_password", "ufr");
  SERIAL1_UDP_PORT = preferences.getInt("port1", 8881);

#ifdef DUAL_MODE
  SERIAL2_UDP_PORT = preferences.getInt("port2", 8882);
#endif
  preferences.end();

  SPIFFS.begin();


  delay(300);
  esp_efuse_mac_get_default(ethernetMac);

  delay(100);
  if (Ethernet.begin(ethernetMac))
  {

    digitalWrite(LED_BUILTIN, HIGH);
  }
  delay(1000);
  ethernetServerStart();
  if (master)
  {

    ethernetServerStart1();
    ethernetServerStart2();
    udpEthernet.begin(SERIAL1_UDP_PORT);
#ifdef DUAL_MODE

    udpEthernet1.begin(SERIAL2_UDP_PORT);
#endif
  }
}

void otaSetup()
{

  ArduinoOTA.setHostname("uFR_Nano");
  ArduinoOTA.setPassword(http_password.c_str());
  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else
      type = "filesystem";
  })
  .onEnd([]() {
    digitalWrite(LED_BUILTIN, LOW);
  })
  .onProgress([](unsigned int progress, unsigned int total) {

  })
  .onError([](ota_error_t error) {

  });

  ArduinoOTA.begin();
}

String ethernetMacToString()
{
  String m = "";
  for (int i = 0; i < 6; i++)
  {
    m += String(ethernetMac[i], HEX) + ":";
  }
  m.remove(17);
  m.toUpperCase();
  return m;

}

String getSerialNumber(int num)
{
  uint8_t cmd[7] = {0x55, 0x40, 0xaa, 0x00, 0xaa, 0xcc, 0xe0};
  String Sn = "";
  uint8_t serialnum[16];
  if (num == 1)
  {
    Serial1.write(cmd, 7);
    delay(20);
    if (Serial1.available())
    {

      Serial1.readBytes(serialnum, 16);

    }
    else
    {
      return "null";
    }

    for (uint8_t k = 7; k < 15; k++)
    {
      Sn += String((char)unsigned(serialnum[k]));
    }

  }
  else
  {
    Serial2.write(cmd, 7);
    delay(20);
    if (Serial2.available())
    {

      Serial2.readBytes(serialnum, 16);

    }
    else
    {
      return "null";
    }

    for (uint8_t k = 7; k < 15; k++)
    {
      Sn += String((char)unsigned(serialnum[k]));
    }
  }

  return Sn;
}


uint8_t getCardID1(uint8_t *cardID, uint8_t *lengthc1) {
  uint8_t cmd[7] = {0x55, 0x2C, 0xAA, 0x00, 0x00, 0x00, 0xDA};
  uint8_t rsp[7];
  uint8_t ext[11];

  Serial1.flush();
  Serial1.write(cmd, 7);

  if (Serial1.available())
  {
    Serial1.readBytes(rsp, 7);
  }

  if (rsp[6] = rsp[0] ^ rsp[1] ^ rsp[2] ^ rsp[3] ^ rsp[4] ^ rsp[5] + 0x07)
  {
    if (Serial1.available())
    {
      Serial1.readBytes(ext, rsp[3]);
    }

    if (ext[0] == 0xEC)
    {
      return 1;
    }
    else
    {
      *lengthc1 = rsp[5];
      for (uint8_t i = 0; i < *lengthc1; i++)
        cardID[i] = ext[i];
      return 0;
    }
  }
  else
  {
    return 1;
  }



}

uint8_t getCardID2(uint8_t *cardID, uint8_t *lengthc1) {
  uint8_t cmd[7] = {0x55, 0x2C, 0xAA, 0x00, 0x00, 0x00, 0xDA};
  uint8_t rsp[7];
  uint8_t ext[11];

  Serial2.flush();
  Serial2.write(cmd, 7);

  if (Serial2.available())
  {
    Serial2.readBytes(rsp, 7);
  }

  if (rsp[6] = rsp[0] ^ rsp[1] ^ rsp[2] ^ rsp[3] ^ rsp[4] ^ rsp[5] + 0x07)
  {
    if (Serial2.available())
    {
      Serial2.readBytes(ext, rsp[3]);
    }

    if (ext[0] == 0xEC)
    {
      return 1;
    }
    else
    {
      *lengthc1 = rsp[5];
      for (uint8_t i = 0; i < *lengthc1; i++)
        cardID[i] = ext[i];
      return 0;
    }
  }
  else
  {
    return 1;
  }



}

void uiSignal1(uint8_t beep, uint8_t light)
{
  uint8_t cs = (0xd9 ^ beep ^ light) + 0x07;
  uint8_t cmd[7] = {0x55, 0x26, 0xAA, 0x00, beep, light, cs};
  Serial1.flush();
  Serial1.write(cmd, 7);

  if (Serial1.available()) {}
}

void uiSignal2(uint8_t beep, uint8_t light)
{
  uint8_t cs = (0xd9 ^ beep ^ light) + 0x07;
  uint8_t cmd[7] = {0x55, 0x26, 0xAA, 0x00, beep, light, cs};
  Serial2.flush();
  Serial2.write(cmd, 7);

  if (Serial2.available()) {}
}




