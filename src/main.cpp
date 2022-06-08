
#include "Arduino.h"
#include <esp_now.h>
#include <WiFi.h>
#define cmdTemp 0x01
#define cmdHum 0x02
#define cmdCamStatus 0x03
union twoByte
{
  byte bVal[2];
  int iVal;
};
float temp;
float hum;
byte camStatus;
uint8_t broadcastAddress[] = {0xE0, 0xE2, 0xE6, 0xCF, 0x9D, 0xAC};
byte peerConnected = 0;
void processData(std::string value);
#define CHANNEL 3
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void broadcast();
void sendData(std::string incomingdata, uint8_t len);
void sendDebugMessages(int tick);
void initBroadcastSlave();
// Structure example to receive data
// Must match the sender structure
esp_now_peer_info_t slave;

typedef struct broadcast_message
{
  char type[16]="unknown";             // data or broadcast
  char ssid[16] = "slave";   // slave or master
  char pass[16] = "unknown"; // slider pan tilt
  uint8_t datalen = 1;
  char data[128] = " ";
} broadcast_message;
// callback function that will be executed when data is received
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

void setup()
{

  Serial.begin(115200);
  Serial.println("CCKIT Main");
  WiFi.mode(WIFI_STA);
  esp_now_deinit();
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  initBroadcastSlave();
}

void loop()
{
  broadcast();
  sendData("pulse", 1);
  sendDebugMessages(100);
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

  default:
    break;
  }
}
void broadcast()
{
  slave.channel = CHANNEL; // pick a channel
  slave.encrypt = 0; // no encryption
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
    Serial.printf("Temp:%.2f\t Humudity:%.2f\t CamStatus:%d\t \n", temp, hum, camStatus);
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