
#include "Arduino.h"
#include <esp_now.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "html.h"
#include "EEPROM.h"

const char *ssidAP = "CCKIT";
const char *passwordAP = "12345678";

const char *PARAM_STRING = "inputString";
const char *PARAM_INT = "inputInt";
const char *PARAM_FLOAT = "inputFloat";

String esid;
String epass = "";
IPAddress staticIP(192, 168, 1, 151);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 254);
#define cmdTemp 0x01
#define cmdHum 0x02
#define cmdCamStatus 0x03
#define cmdResponseSetting 0x04
union twoByte
{
  byte bVal[2];
  int iVal;
};
float temp;
float hum;
byte camStatus;
byte processStatus=0;
uint8_t broadcastAddress[] = {0xE0, 0xE2, 0xE6, 0xCF, 0x9D, 0xAC};
byte peerConnected = 0;
byte camWifiSettingResponse = 0;
uint8_t mode = 0;
uint8_t loadWifiMode();
void writeWifiMode(uint8_t mode);
void processData(std::string value);
#define CHANNEL 3
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void broadcast();
void sendData(std::string incomingdata, uint8_t len);
void sendDebugMessages(int tick);
void initBroadcastSlave();
void configCamWifi(const char *ssid, const char *pass);
void startPage();
void saveToEEPROM(String qsid, String qpass);
void loadFromEEPROM();
void getWifiConfFromHTTP(void *parameter);
String inputParam;
String inputMessage;
String ssidRecv;
String passRecv;
// Structure example to receive data
// Must match the sender structure
esp_now_peer_info_t slave;

typedef struct broadcast_message
{
  char type[16] = "unknown"; // data or broadcast
  char ssid[16] = "slave";   // slave or master
  char pass[16] = "unknown"; // slider pan tilt
  uint8_t datalen = 1;
  char data[128] = " ";
} broadcast_message;
// callback function that will be executed when data is received
AsyncWebServer server(80);

void handleLED(AsyncWebServerRequest *request)
{
  String t_state = request->arg("LEDstate"); // Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
  String t_num = request->arg("ledNum");
  Serial.print(t_state);
  Serial.print("\t");
  Serial.println(t_num);

  request->send(200, "text/plane", "1"); // Send web page
}
void handleRGB(AsyncWebServerRequest *request)
{
  // get RGB values from parameters
  String rVal = request->arg("r");
  String gVal = request->arg("g");
  String sVal = request->arg("s");
  String checkbox = request->arg("cb");

  // show values on serial monitor
  Serial.print(checkbox);
  Serial.print("\t");
  Serial.print(sVal);
  Serial.print("\t");
  Serial.print(rVal);
  Serial.print("\t");
  Serial.println(gVal);

  // send answer to client
  request->send(200, "text/plane", "1");
}
void handleWifiSettings(AsyncWebServerRequest *request)
{

  ssidRecv = request->arg("s");
  passRecv = request->arg("p");

  // show values on serial monitor
  Serial.print(ssidRecv);
  Serial.print("\t");
  Serial.println(passRecv);

  // send answer to client
  request->send(200, "text/plane", "1");
  xTaskCreate(getWifiConfFromHTTP, "getWifiConfFromHTTP", 4096, (void *)1, 1, NULL);
}
void handleADC(AsyncWebServerRequest *request)
{
  request->send(200, "text/plane", String(temp));
}
void handlePrcStatus(AsyncWebServerRequest *request)
{
  String processBtn;
  if (processStatus)
  {
    processBtn="Stop";
  }
  else
  {
    processBtn="Process"
  }
  request->send(200, "text/plane", processBtn);
}
void handleNotifyCam(AsyncWebServerRequest *request)
{
  String status;
  if (peerConnected)
  {
    status = "CKIT CAM Connected";
  }
  else
  {
    status = "Couldn't Find CCKIT CAM";
  }
  request->send(200, "text/plane", status);
}
String message = "Waiting for process";
void handleMessage(AsyncWebServerRequest *request)
{

  request->send(200, "text/plane", message);
}
void handleRoot(AsyncWebServerRequest *request)
{
  Serial.println("Controller connected");
  request->send(200, "text/html", MAIN_PAGE);
}
void handleRootWifi(AsyncWebServerRequest *request)
{
  Serial.println("Wifi settings connected");
  request->send(200, "text/html", SETTINGS_PAGE);
}
void handleNotFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/html", PAGE_404);
}
void handleWifiMode(AsyncWebServerRequest *request)
{

  String req = request->arg("w");

  Serial.println(req);

  // send answer to client
  request->send(200, "text/plane", "1");
  writeWifiMode(0);
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  ESP.restart();
}

IPAddress IP;
void setup()
{

  Serial.begin(115200);
  if (!EEPROM.begin(512))
  {
    Serial.println("failed to init EEPROM");
  }
  Serial.println("CCKIT Main");
  // writeWifiMode(0);
  mode = loadWifiMode();
  // saveToEEPROM("Aykut", "edirne12345");
  loadFromEEPROM();
  if (mode == 1)
  {
    WiFi.mode(WIFI_STA);
    WiFi.config(staticIP, gateway, subnet, dns, dns);
    WiFi.begin(esid.c_str(), epass.c_str());
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      Serial.println("WiFi Failed!");
      Serial.println();
      writeWifiMode(0);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      ESP.restart();
    }
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    WiFi.mode(WIFI_AP);
     WiFi.softAP(ssidAP, passwordAP);
    WiFi.softAPConfig(staticIP, gateway, subnet);
   
    IP = WiFi.softAPIP();
    Serial.println(IP);
  }

  esp_now_deinit();
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  startPage();
  initBroadcastSlave();
}

void loop()
{
  if (Serial.available() > 0)
  {
    // read the incoming string:
    message = Serial.readString();
  }
  broadcast();
  sendData("pulse", 1);
  sendDebugMessages(100);
  // Serial.print("AP IP address: ");
  // Serial.println(IP);
  vTaskDelay(100 / portTICK_PERIOD_MS);
}
void processData(std::string value)
{
  twoByte temptwo;
  twoByte humtwo;
  switch ((byte)value[0])
  {
  case cmdTemp:
    temptwo.bVal[0] = value[1];
    temptwo.bVal[1] = value[2];
    temp = temptwo.iVal / 10.0;
    break;
  case cmdHum:
    humtwo.bVal[0] = value[1];
    humtwo.bVal[1] = value[2];
    hum = humtwo.iVal / 10.0;
    break;
  case cmdCamStatus:
    camStatus = value[1];
    break;
  case cmdResponseSetting:
    camWifiSettingResponse = value[1];
    break;
  default:
    break;
  }
}
void broadcast()
{
  slave.channel = CHANNEL; // pick a channel
  slave.encrypt = 0;       // no encryption
  for (int ii = 0; ii < 6; ++ii)
  {
    slave.peer_addr[ii] = (uint8_t)0xff;
  }
  broadcast_message data;
  ((String) "broadcast").toCharArray(data.type, 16);
  esp_err_t result = esp_now_send(slave.peer_addr, (uint8_t *)&data, sizeof(data));
}
void sendDebugMessages(int tick)
{
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;
  if (currentMillis - previousMillis >= tick)
  {
    Serial.printf("Temp:%.2f\t Humudity:%.2f\t PeerStatus:%d WifiMode:%d\t \n", temp, hum, peerConnected, mode);
    previousMillis = currentMillis;
  }
}
void sendData(std::string incomingdata, uint8_t len)
{
  broadcast_message data;
  data.datalen = len;

  ((String) "data").toCharArray(data.type, 16);
  for (int i = 0; i < incomingdata.length(); i++)
  {
    data.data[i] = incomingdata[i];
  }
  esp_err_t result2 = esp_now_send(0, (uint8_t *)&data, sizeof(data)); // send to all peers
}
void initBroadcastSlave()
{
  // clear slave data
  memset(&slave, 0, sizeof(slave));
  for (int ii = 0; ii < 6; ++ii)
  {
    slave.peer_addr[ii] = (uint8_t)0xff;
  }
  esp_err_t addStatus = esp_now_add_peer(&slave);
}
void configCamWifi(const char *ssid, const char *pass)
{
  broadcast_message data;

  ((String) "wificonfig").toCharArray(data.type, 16);
  ((String)ssid).toCharArray(data.ssid, 16);
  ((String)pass).toCharArray(data.pass, 16);
  esp_err_t result2 = esp_now_send(0, (uint8_t *)&data, sizeof(data)); // send to all peers
}
void startPage()
{
  if (mode == 1)
  {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/readADC", HTTP_GET, handleADC);
    server.on("/readPrcButton", HTTP_GET, handlePrcStatus);
    server.on("/setLED", HTTP_GET, handleLED);
    server.on("/setRGB", HTTP_GET, handleRGB);
    server.on("/setWifiMode", HTTP_GET, handleWifiMode);
    server.onNotFound(handleNotFound);
  }
  else
  {
    server.on("/", HTTP_GET, handleRootWifi);
    server.on("/setWifi", HTTP_GET, handleWifiSettings);
    server.on("/notifyCAM", HTTP_GET, handleNotifyCam);
    server.on("/message", HTTP_GET, handleMessage);
    server.onNotFound(handleNotFound);
  }

  // start server
  server.begin();
}
void loadFromEEPROM()
{
  for (int i = 0; i < 32; ++i)
  {
    byte readValue = EEPROM.read(i);

    if (readValue == '|')
    {
      break;
    }

    esid += char(readValue);
  }
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
  for (int i = 32; i < 96; ++i)
  {
    byte readValue = EEPROM.read(i);

    if (readValue == '|')
    {
      break;
    }

    epass += char(readValue);
  }
  Serial.print("PASS: ");
  Serial.println(epass);
}
void saveToEEPROM(String qsid, String qpass)
{
  qsid.concat("|");
  qpass.concat("|");
  Serial.println("writing eeprom ssid:");
  for (int i = 0; i < qsid.length(); ++i)
  {
    EEPROM.write(i, qsid[i]);
    Serial.print("Wrote: ");
    Serial.println(qsid[i]);
  }
  Serial.println("writing eeprom pass:");
  for (int i = 0; i < qpass.length(); ++i)
  {
    EEPROM.write(32 + i, qpass[i]);
    Serial.print("Wrote: ");
    Serial.println(qpass[i]);
  }
  EEPROM.commit();
}
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len)
{
  broadcast_message myData;
  memcpy(&myData, data, sizeof(myData));

  if ((String)myData.type == "broadcast")
  {
    esp_now_peer_info_t mypeerInf;
    mypeerInf.channel = CHANNEL;
    mypeerInf.encrypt = 0;
    mypeerInf.ifidx = WIFI_IF_STA;
    memcpy(mypeerInf.peer_addr, mac_addr, 6);
    if (!esp_now_is_peer_exist(mac_addr))
      if (esp_now_add_peer(&mypeerInf) != ESP_OK)
      {
        Serial.println("peer couldn't added");
        return;
      }
      else
      {
        peerConnected = 1;
        Serial.println("peer connected");
      }
  }
  else if ((String)myData.type == "data")
  {
    int i;
    std::string s = "";
    for (i = 0; i < myData.datalen; i++)
    {
      s = s + myData.data[i];
    }
    processData(s);
  }
  else if ((String)myData.type == "unknown")
  {
    Serial.println("unkown data received");
  }
  else
  {
    Serial.println("type error");
  }
}
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  if (status == ESP_NOW_SEND_SUCCESS)
  {
  }
  else
  {
    esp_err_t delStatus = esp_now_del_peer(mac_addr);
    peerConnected = 0;
    Serial.println("peer disconnected");
  }
}
void getWifiConfFromHTTP(void *parameter)
{
  message = "Processing config...";
  broadcast_message data;
  ((String) "wificonfig").toCharArray(data.type, 16);
  (ssidRecv).toCharArray(data.ssid, 16);
  (passRecv).toCharArray(data.pass, 16);
  esp_err_t result = esp_now_send(0, (uint8_t *)&data, sizeof(data));
  long startTime = millis();
  long totalTime;
  while (!camWifiSettingResponse && totalTime < 6000)
  {
    totalTime = millis() - startTime;
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  if (!camWifiSettingResponse)
  {
    message = "Timeout: no response from cam module";
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    Serial.println("Timeout: no response from cam module");
    message = "Done. Restarting...";
    saveToEEPROM(ssidRecv, passRecv);
    writeWifiMode(1); // set sta mode
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    ESP.restart();
  }
  else
  {
    message = "Done. Restarting...";
    saveToEEPROM(ssidRecv, passRecv);
    writeWifiMode(1); // set sta mode
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    ESP.restart();
  }
  vTaskDelete(NULL);
}

void writeWifiMode(uint8_t mode)
{
  EEPROM.write(511, mode); // EEPROM.put(address, boardId);
  EEPROM.commit();
}
uint8_t loadWifiMode()
{
  return EEPROM.read(511);
}