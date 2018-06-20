#include <esp_wifi.h>
#include <WiFi.h>
#include <WiFiUdp.h>

//#define DUAL_MODE //Uncomment for DUAL_MODE UFR mode

#define UART_BAUD 115200
#define bufferSize 10240
#define timeout 5

#define UART_BAUD1 115200
#define SERIAL_PARAM1 SERIAL_8N1
#define SERIAL1_RXPIN 16
#define SERIAL1_TXPIN 17
#define SERIAL1_UDP_PORT 8881

#ifdef DUAL_MODE
#define UART_BAUD2 115200
#define SERIAL_PARAM2 SERIAL_8N1
#define SERIAL2_RXPIN 15
#define SERIAL2_TXPIN 4
#define SERIAL2_UDP_PORT 8882
#endif
const char *ssid = "";
const char *pw = "";

HardwareSerial Serial1(1);
WiFiUDP udp;
IPAddress remoteIp;
uint16_t rPort;
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

#ifdef DUAL_MODE
uint8_t buffer11[bufferSize];
uint8_t length11 = 0;
uint8_t buffer21[bufferSize];
uint8_t length21 = 0;
#endif
int LED_BUILTIN = 2;


void setup() {

  delay(500);

  Serial.begin(UART_BAUD);
  Serial1.begin(UART_BAUD1, SERIAL_PARAM1, SERIAL1_RXPIN, SERIAL1_TXPIN);
  #ifdef DUAL_MODE
  Serial2.begin(UART_BAUD2, SERIAL_PARAM2, SERIAL2_RXPIN, SERIAL2_TXPIN);
  #endif
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pw);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP Servers");
  udp.begin(SERIAL1_UDP_PORT);
  #ifdef DUAL_MODE
  udp1.begin(SERIAL2_UDP_PORT);
  #endif
}


void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize > 0) {
    remoteIp = udp.remoteIP();
    rPort = udp.remotePort();
    udp.read(buffer1, bufferSize);
    Serial1.write(buffer1, packetSize);
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
    Serial2.write(buffer11, packetSize1);
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