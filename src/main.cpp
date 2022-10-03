#include "decs.h"
#include "GyverPID.h"
#include <esp_wifi.h>
// #define PIDDEBUG
#define DEBUG
// pid settings and gains
#define OUTPUT_MIN 0
#define OUTPUT_MAX 255
#define WATER_LEVEL_PIN 15
#define WATER_POWER_PIN 18
int waterLevel = 0;
bool ssrState = 0;
char cckitCamMac[18] = {0};
uint8_t cckitCamMacBytes[6];

AsyncWebServer server(80);

void setup()
{

  pinMode(humidifierPin, OUTPUT);
  pinMode(heaterFan, OUTPUT);
  pinMode(ssrPin, OUTPUT);
  digitalWrite(humidifierPin, HIGH);
  digitalWrite(heaterFan, HIGH);
  digitalWrite(ssrPin, LOW);
  Serial.begin(115200);
  if (!EEPROM.begin(512))
  {
    Serial.println("failed to init EEPROM");
  }
  Serial.println("CCKIT Main");
  mode = loadWifiMode();
  functions.loadFromEEPROM();
  vTaskDelay(500 / portTICK_PERIOD_MS);
  functions.loadPIDFromEEPROM();

  if (mode == 1)
  {
    WiFi.mode(WIFI_AP_STA);
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    Serial.print("channel:");
    Serial.println(WiFi.channel());
    WiFi.config(staticIP, gateway, subnet, dns);
    WiFi.begin(functions.esid.c_str(), functions.epass.c_str());
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
  else if (mode == 2)
  {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(staticIP, gateway, subnet, dns);
    WiFi.softAP(ssidAP, passwordAP);

    IP = WiFi.softAPIP();
    Serial.println(IP);
    Serial.println("AP mode started");
  }
  else
  {
    WiFi.mode(WIFI_AP_STA);
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    uint8_t primaryChan = CHANNEL;
    wifi_second_chan_t secondChan = WIFI_SECOND_CHAN_NONE;
    esp_wifi_set_channel(primaryChan, secondChan);
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(false));
    WiFi.softAPConfig(staticIP, gateway, subnet, dns);
    WiFi.softAP(ssidAP, passwordAP);
    IP = WiFi.softAPIP();
    Serial.println("Config mode started");
    Serial.print("channel:");
    Serial.println(WiFi.channel());
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
  xTaskCreatePinnedToCore(
      displayTask,                 /* Function to implement the task */
      "Task1",                     /* Name of the task */
      10000,                       /* Stack size in words */
      NULL,                        /* Task input parameter */
      255,                         /* Priority of the task */
      NULL /* Task handle. */, 1); /* Core where the task should run */
}

void loop()
{
  broadcast();
  // sendDebugMessages(100);
  vTaskDelay(1 / portTICK_PERIOD_MS);
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
    humtwo.bVal[0] = value[3];
    humtwo.bVal[1] = value[4];
    hum = humtwo.iVal / 10.0;
    if (temp >= 70)
    {
      updateProcessStatus(0, 3);
    }
    break;
  case cmdResponseSetting: // DOESNT WORK
    camWifiSettingResponse = value[1];
    break;
  case cmdCamTempError:
    sensorError = value[1];
    break;
  default:
    break;
  }
}
void broadcast()
{
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;
  if (currentMillis - previousMillis >= 500)
  {
    slave.channel = CHANNEL; // pick a channel
    slave.encrypt = 0;       // no encryption
    for (int ii = 0; ii < 6; ++ii)
    {
      slave.peer_addr[ii] = (uint8_t)0xff;
    }
    broadcast_message data;
    ((String) "main").toCharArray(data.device, 16);
    ((String) "broadcast").toCharArray(data.type, 16);
    esp_err_t result = esp_now_send(slave.peer_addr, (uint8_t *)&data, sizeof(data));
    sendData("pulse", 1);
    previousMillis = currentMillis;
  }
}
void sendDebugMessages(int tick)
{
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;
  if (currentMillis - previousMillis >= tick)
  {
#ifdef DEBUG
    Serial.printf("Temp:%.2f\t Humudity:%.2f\t TarTemp:%d TarHum:%d CamStatus:%d WifiMode:%d CamLastreceive:%d\n", temp, hum, targetTemp, targetHum, camStatus, mode, camLastReceive);
#endif
#ifdef PIDDEBUG
    Serial.printf("Current Temp:%f\tTarget Temp:%f\tSsrState:%d\t KP:%f KI:%f KD:%f \n", temp, targetTemp, ssrState, KP, KI, KD);
#endif
    previousMillis = currentMillis;
  }
}
void sendData(std::string incomingdata, uint8_t len)
{
  broadcast_message data;
  data.datalen = len;
  ((String) "main").toCharArray(data.device, 16);
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
  if (mode == 1 || mode == 2)
  {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/liveData", HTTP_GET, handleLiveData);
    server.on("/readPrcButton", HTTP_GET, handlePrcStatus);
    server.on("/readToast", HTTP_GET, handleToast);
    server.on("/setLED", HTTP_GET, handleLED);
    server.on("/setRGB", HTTP_GET, handleProcess);
    server.on("/setWifiMode", HTTP_GET, handleWifiMode);
    server.on("/emgStop", HTTP_GET, handleEmergencyStop);
    server.on("/flashToggle", HTTP_GET, handleFlash);
    server.on("/message", HTTP_GET, handleMessage);
    server.on("/notifyCAM", HTTP_GET, handleNotifyCam);
    server.on("/savePID", HTTP_GET, handleSavePID);
    server.on("/camUpdate", HTTP_GET, handleCamUpdate); // readToast

    server.onNotFound(handleNotFound);
  }
  else
  {
    server.on("/", HTTP_GET, handleRootWifi);
    server.on("/setWifi", HTTP_GET, handleWifiSettings);
    server.on("/startAP", HTTP_GET, handleStartAP);
    server.on("/notifyCAM", HTTP_GET, handleNotifyCam);
    server.on("/message", HTTP_GET, handleMessage);
    server.onNotFound(handleNotFound);
  }

  // start server
  server.begin();
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len)
{
  broadcast_message myData;
  memcpy(&myData, data, sizeof(myData));

  if ((String)myData.type == "broadcast")
  {
    if ((String)myData.device == "camera")
    {
      camLastReceive = millis();
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
          sprintf(cckitCamMac, // save macadress in variables library
                  "%02X:%02X:%02X:%02X:%02X:%02X",
                  mac_addr[0],
                  mac_addr[1],
                  mac_addr[2],
                  mac_addr[3],
                  mac_addr[4],
                  mac_addr[5]);
          memcpy(cckitCamMacBytes, mac_addr, 6);
          camStatus = 1;
          Serial.println("Cam connected");
          functions.toastMessage = "Camera Connected";
        }
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
  else
  {
    Serial.println("type error");
  }
}
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  char macStr[18] PROGMEM = {0};
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  if ((String)macStr == (String)cckitCamMac)
  {
    if (status == ESP_NOW_SEND_SUCCESS)
    {
      camStatus = 1;
    }
    else
    {
      esp_err_t delStatus = esp_now_del_peer(mac_addr);
      camStatus = 0;
      Serial.println("peer disconnected");
      functions.toastMessage = "Camera Disconnected";
      processStatus = 0;
    }
  }
}
void getWifiConfFromHTTP(void *parameter)
{
  functions.message = "Processing config...";
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
    functions.message = "Timeout: No response from cam module. Main setted successfully. Connect to WIFI that you init and refresh the page.";
  }
  else
  {
    functions.message = "CAM and Main setted successfully. Connect to WIFI that you init and refresh the page.";
  }
  functions.saveToEEPROM(ssidRecv, passRecv);
  writeWifiMode(1); // set sta mode
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  ESP.restart();
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
void initDisplay()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(WHITE);
  display.setTextSize(1); // Draw 2X-scale text
  drawCentreString("CCKIT MAIN", 64, 24);
  display.display();
  vTaskDelay(500 / portTICK_PERIOD_MS);
}
void composeDisplay()
{
  display.clearDisplay();
  if (mode == 1)
  {
    char sensorDisplay[100];
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    drawCentreString(WiFi.localIP().toString().c_str(), 64, 34);
    display.setFont(&SourceSansPro_Regular6pt7b);
    sprintf(sensorDisplay, "%dC %dH", (int)temp, (int)hum);
    drawCentreString(sensorDisplay, 64, 58);
    drawCentreString(functions.esid.c_str(), 64, 12);
  }
  else if (mode == 0)
  {
    display.setFont(&SourceSansPro_Regular6pt7b);
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    drawCentreString("CCKIT 192.168.1.151", 64, 12);
    display.setFont(&FreeSans9pt7b);
    drawCentreString("WIFI CONFIG", 64, 34);
  }

  display.display();
  vTaskDelay(50 / portTICK_PERIOD_MS);
}
void drawCentreString(const char *buf, int x, int y)
{
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, 0, y, &x1, &y1, &w, &h); // calc width of new string
  display.setCursor(x - w / 2, y);
  display.print(buf);
}
void displayTask(void *parameter)
{
  initDisplay();
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  for (;;)
  {
    composeDisplay();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void processAction(int ttem, int thum)
{
  targetHum = thum;
  targetTemp = ttem;

  if (!sensorError && camStatus)
  {
    updateProcessStatus(1, 2);
    xTaskCreate(
        humidityTask,             /* Function to implement the task */
        "humidityTask",           /* Name of the task */
        4096,                     /* Stack size in words */
        NULL,                     /* Task input parameter */
        1,                        /* Priority of the task */
        NULL /* Task handle. */); /* Core where the task should run */
    xTaskCreate(
        heaterTask,               /* Function to implement the task */
        "heaterTask",             /* Name of the task */
        4096,                     /* Stack size in words */
        NULL,                     /* Task input parameter */
        1,                        /* Priority of the task */
        NULL /* Task handle. */); /* Core where the task should run */
  }
  else
  {
    updateProcessStatus(0, 2);
    functions.message = "Cannot read data from sensor";
  }
}
void humidityTask(void *parameter)
{
  while (processStatus)
  {
    if (hum < targetHum)
    {
      digitalWrite(humidifierPin, LOW);
      humidifierStatus = 1;
    }
    else
    {
      digitalWrite(humidifierPin, HIGH);
      humidifierStatus = 0;
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
  Serial.println("Humidication stopped.");
  digitalWrite(humidifierPin, HIGH);
  targetHum = 0;
  vTaskDelete(NULL);
}

GyverPID regulator(functions.KP, functions.KI, functions.KD, 1000);

void heaterTask(void *parameter)
{

  regulator.setDirection(NORMAL);
  regulator.setLimits(0, 1);
  regulator.setpoint = targetTemp;

  regulator.Kp = functions.KP;
  regulator.Ki = functions.KI;
  regulator.Kd = functions.KD;

  while (processStatus)
  {

      regulator.input = temp;
      stateHeaterFan(1);
      ssrState = regulator.getResultTimer();
      stateHeater(ssrState);
      Serial.print("state:");
      Serial.println(ssrState);
    

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }

  stateHeater(0);
  targetTemp = 0;
  ssrState = 0;
  while (!digitalRead(heaterFan))
  {
    stateHeaterFan(0);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
  Serial.println("heater process finished");
  vTaskDelete(NULL);
}
void stateHeater(byte state)
{
  if (state)
  {
    digitalWrite(ssrPin, HIGH);
  }
  else
  {
    digitalWrite(ssrPin, LOW);
    lastHeaterOpen = millis();
  }
}
void stateHeaterFan(byte state)
{
  if (state)
  {
    digitalWrite(heaterFan, LOW);
    heaterStatus = 1;
  }
  else
  {
    if (millis() - lastHeaterOpen > MIN_FANTIME)
    {
      digitalWrite(heaterFan, HIGH);
      heaterStatus = 0;
    }
  }
}
void updateProcessStatus(byte status, byte message)
{
  if (status)
  {
    processStatus = 1;
    if (message == 1)
    {
      functions.toastMessage = "Process Started";
      functions.message = "Process Started";
    }
    else if (message == 2)
    {
      functions.message = "Running...";
    }
  }
  else
  {
    if (message == 1)
    {
      functions.message = "Process stopped";
      functions.toastMessage = "Process Stopped";
    }
    else if (message == 2)
    {
      functions.message = "Cannot receive data from CCKIT-CAM";
      functions.toastMessage = "Cannot receive data from CCKIT-CAM";
    }
    else if (message == 3)
    {
      functions.message = "Reached MAX Temperature";
      functions.toastMessage = "Reached MAX Temperature";
    }
    else if (message == 4)
    {
      functions.message = "Current Temperature is too high according to target temperature";
      functions.toastMessage = "Current Temperature is too high according to target temperature";
    }
    processStatus = 0;
    targetTemp = 0;
    targetHum = 0;
    temp = 0;
    hum = 0;
  }
}