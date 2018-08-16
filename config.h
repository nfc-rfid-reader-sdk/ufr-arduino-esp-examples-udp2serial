#ifndef CONFIG
#define CONFIG

#define DUAL_MODE //Uncomment for DUAL_MODE UFR mode

#define UART_BAUD 115200
#define DEBUG_PORT 8880
#define bufferSize 300
#define timeout 5

#define UART_BAUD1 115200
#define SERIAL_PARAM1 SERIAL_8N1
#define SERIAL1_RXPIN 21
#define SERIAL1_TXPIN 22
#define SERIAL1_RESETPIN 26
int SERIAL1_UDP_PORT;

#ifdef DUAL_MODE
#define UART_BAUD2 115200
#define SERIAL_PARAM2 SERIAL_8N1
#define SERIAL2_RXPIN 16
#define SERIAL2_TXPIN 17
#define SERIAL2_RESETPIN 27
int SERIAL2_UDP_PORT;
#endif

#define SET_DEFAULTPIN 34

#define LED_BUILTIN 2

#define WIRED_STATE 35
  
const char *ssid = "";
const char *pw = "";
WebServer server(80);
WebServer server1(81);
WebServer server2(82);

ESP8266EthernetWebServer ethernetServer(80);
ESP8266EthernetWebServer ethernetServer1(81);
ESP8266EthernetWebServer ethernetServer2(82);

const char *ssidAP = "";
const char *passwordAP = "password";

WiFiUDP udpWifi;
EthernetUDP udpEthernet;
IPAddress remoteIp;
uint16_t rPort;

#ifdef DUAL_MODE

WiFiUDP udpWifi1;
EthernetUDP udpEthernet1;
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

uint8_t bufferTransparent2[bufferSize];
uint8_t lengthTransparent2 = 0;

uint8_t bufferDebug[bufferSize];
uint8_t lengthDebug = 0;
uint8_t buffer2Debug[bufferSize];
uint8_t length2Debug = 0;

uint8_t transparentMode;
uint8_t transparentDevice;
uint8_t sendFrom = 0;
uint8_t sendFrom1 = 0;

String http_username;
String http_password;


Preferences preferences;

#ifdef DUAL_MODE
uint8_t buffer11[bufferSize];
uint8_t length11 = 0;
uint8_t buffer21[bufferSize];
uint8_t length21 = 0;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

uint8_t temprature_sens_read();

#ifdef __cplusplus
}
#endif

int wireless = 1;

int master = 1;

String serialNumber1;
String serialNumber2;

byte ethernetMac[6];

String host;
#endif

