#include <esp_wifi.h>
#include <WiFi.h>
#include <WiFiUdp.h>

//#define DUAL_MODE //Uncomment for DUAL_MODE UFR mode

#define UART_BAUD 115200
#define DEBUG_PORT 8880
#define bufferSize 300
#define timeout 5

#define UART_BAUD1 115200
#define SERIAL_PARAM1 SERIAL_8N1
#define SERIAL1_RXPIN 16
#define SERIAL1_TXPIN 17
#define SERIAL1_RESETPIN 5
#define SERIAL1_UDP_PORT 8881

#ifdef DUAL_MODE
#define UART_BAUD2 115200
#define SERIAL_PARAM2 SERIAL_8N1
#define SERIAL2_RXPIN 15
#define SERIAL2_TXPIN 4
#define SERIAL2_RESETPIN 18
#define SERIAL2_UDP_PORT 8882
#endif
const char *ssid = "";
const char *pw = "";

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

uint8_t bufferDebug[bufferSize];
uint8_t lengthDebug = 0;
uint8_t buffer2Debug[bufferSize];
uint8_t length2Debug = 0;

#ifdef DUAL_MODE
uint8_t buffer11[bufferSize];
uint8_t length11 = 0;
uint8_t buffer21[bufferSize];
uint8_t length21 = 0;
#endif
int LED_BUILTIN = 2;
#ifdef __cplusplus
extern "C" {
#endif

uint8_t temprature_sens_read();

#ifdef __cplusplus
}
#endif

void setup() {

  delay(500);

  pinMode(SERIAL1_RESETPIN, OUTPUT);
  Serial1.begin(UART_BAUD1, SERIAL_PARAM1, SERIAL1_RXPIN, SERIAL1_TXPIN);


#ifdef DUAL_MODE
  Serial2.begin(UART_BAUD2, SERIAL_PARAM2, SERIAL2_RXPIN, SERIAL2_TXPIN);
  pinMode(SERIAL2_RESETPIN, OUTPUT);
#endif
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pw);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  digitalWrite(SERIAL1_RESETPIN, HIGH);
  delay(100);
  digitalWrite(SERIAL1_RESETPIN, LOW);
  udp.begin(SERIAL1_UDP_PORT);
#ifdef DUAL_MODE
  digitalWrite(SERIAL2_RESETPIN, HIGH);
  delay(10);
  digitalWrite(SERIAL2_RESETPIN, LOW);
  udp1.begin(SERIAL2_UDP_PORT);
#endif
  delay(300);
  digitalWrite(LED_BUILTIN, HIGH);
}

bool debug(uint8_t *in, int num) {

  uint8_t out[7];

  if (in[0] == 0x55 && in[1] == 0xFE && in[2] == 0xAA && in[3] == 0x00 && in[5] == 0x00)
  {
    if (in[4] == 0x01 && in[6] == ((in[0] ^ in[1] ^ in[2] ^ in[3]^ in[4] ^ in[5]) + 0x07))
    {
      if (num == 1)
      {
        digitalWrite(SERIAL1_RESETPIN, HIGH);
        delay(10);
        digitalWrite(SERIAL1_RESETPIN, LOW);
      } else if (num == 2)
      {
#ifdef DUAL
        digitalWrite(SERIAL2_RESETPIN, HIGH);
        delay(10);
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
        udp.beginPacket(remoteIp, rPort);
        udp.write(out, 7);
        udp.endPacket();
      } else if (num == 2)
      {
#ifdef DUAL
        udp1.beginPacket(remoteIp1, rPort1);
        udp1.write(out, 7);
        udp1.endPacket();
#endif
      }


    } else if (in[4] == 0x02  && in[6] == ((in[0] ^ in[1] ^ in[2] ^ in[3]^ in[4] ^ in[5]) + 0x07))
    {
 
      out[0] = 0xDE;
      out[1] = 0xFE;
      out[2] = 0xED;
      out[3] = 0x00;
      out[4] = (temprature_sens_read()- 32 ) / 1.8;
      out[5] = 0x00;
      out[6] = (out[0] ^ out[1] ^ out[2] ^ out[3] ^ out[4] ^ out[5]) + 0x07;
      if (num == 1)
      {
        udp.beginPacket(remoteIp, rPort);
        udp.write(out, 7);
        udp.endPacket();
      } else if (num == 2)
      {
#ifdef DUAL
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


void loop() {

  int packetSize = udp.parsePacket();
  if (packetSize > 0) {
    remoteIp = udp.remoteIP();
    rPort = udp.remotePort();
    udp.read(buffer1, bufferSize);
    if (!debug(buffer1, 1))
    {
      Serial1.write(buffer1, packetSize);
    }

  }

  if (Serial1.available()) {
    while (1) {
      if (Serial1.available()) {
        buffer2[length2] = Serial1.read();
        if (length2 < bufferSize - 1) {
          length2++;
        }
      }
      else {
        delay(timeout);
        if (!Serial1.available()) {
          break;
        }
      }
    }

    udp.beginPacket(remoteIp, rPort);
    udp.write(buffer2, length2);
    udp.endPacket();
    length2 = 0;
  }

#ifdef DUAL_MODE
  int packetSize1 = udp1.parsePacket();
  if (packetSize1 > 0) {
    remoteIp1 = udp1.remoteIP();
    rPort1 = udp1.remotePort();
    udp1.read(buffer11, bufferSize);
    if (!debug(buffer1, 2))
    {
      Serial2.write(buffer11, packetSize1);
    }
  }

  if (Serial2.available()) {
    while (1) {
      if (Serial2.available()) {
        buffer21[length21] = Serial2.read();
        if (length21 < bufferSize - 1) {
          length21++;
        }
      }
      else {
        delay(timeout);
        if (!Serial2.available()) {
          break;
        }
      }
    }

    udp1.beginPacket(remoteIp1, rPort1);
    udp1.write(buffer21, length21);
    udp1.endPacket();
    length21 = 0;
  }
#endif
}
