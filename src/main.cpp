#include "decs.h"
#define PIDDEBUG
// #define DEBUG
// pid settings and gains
#define OUTPUT_MIN 0
#define OUTPUT_MAX 255

bool ssrState = 0;
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
  loadFromEEPROM();
  vTaskDelay(500/portTICK_PERIOD_MS);
  loadPIDFromEEPROM();
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
  else if (mode == 2)
  {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssidAP, passwordAP);
    WiFi.softAPConfig(staticIP, gateway, subnet);

    IP = WiFi.softAPIP();
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
  xTaskCreate(
      displayTask,              /* Function to implement the task */
      "Task1",                  /* Name of the task */
      10000,                    /* Stack size in words */
      NULL,                     /* Task input parameter */
      1,                        /* Priority of the task */
      NULL /* Task handle. */); /* Core where the task should run */


}

void loop()
{
  if (Serial.available() > 0)
  {
    message = Serial.readString();
  }
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
  case cmdResponseSetting:
    camWifiSettingResponse = value[1];
    break;
    case cmdCamTempError:
    sensorError=value[1];
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
#ifdef DEBUG
    Serial.printf("Temp:%.2f\t Humudity:%.2f\t TarTemp:%d TarHum:%d PeerStatus:%d WifiMode:%d\t \n", temp, hum, targetTemp, targetHum, peerConnected, mode);
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
    server.on("/setLED", HTTP_GET, handleLED);
    server.on("/setRGB", HTTP_GET, handleRGB);
    server.on("/setWifiMode", HTTP_GET, handleWifiMode);
    server.on("/emgStop", HTTP_GET, handleEmergencyStop);
    server.on("/flashToggle", HTTP_GET, handleFlash);
    server.on("/message", HTTP_GET, handleMessage);
    server.on("/notifyCAM", HTTP_GET, handleNotifyCam);
    server.on("/savePID", HTTP_GET, handleSavePID);
    server.on("/camUpdate", HTTP_GET, handleCamUpdate);
    
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
    message = "Timeout: No response from cam module. Main setted successfully. Restarting on STA Mode...";
    saveToEEPROM(ssidRecv, passRecv);
    writeWifiMode(1); // set sta mode
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    ESP.restart();
  }
  else
  {
    message = "CAM and Main setted successfully. Restarting on STA Mode...";
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
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  drawCentreString(WiFi.localIP().toString().c_str(), 64, 29);
  display.setFont(&SourceSansPro_Regular6pt7b);
  display.setTextSize(1);
  display.setCursor(80, 10);
  display.print((int)temp);
  display.print("C° ");
  display.print((int)hum);
  display.println("% ");
  drawCentreString(esid.c_str(), 38, 10);
  if (WiFi.isConnected())
  {
  }
  else
  {
  }
  display.display();
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
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void processAction(int ttem, int thum)
{
  targetHum = thum;
  targetTemp = ttem;
  processStatus = 1;
  Serial.println("processing...");
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
void humidityTask(void *parameter)
{
  while (processStatus)
  {
    if (hum < targetHum)
    {
      digitalWrite(humidifierPin, LOW);
    }
    else
    {
      digitalWrite(humidifierPin, HIGH);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
  digitalWrite(humidifierPin, HIGH);
  targetHum=0;
  vTaskDelete(NULL);
}
void heaterTask(void *parameter)
{
  AutoPIDRelay myPID(&temp, &targetTemp, &ssrState, 5000.0, KP, KI, KI);
    myPID.setBangBang(20);
  // set PID update interval to 4000ms
  myPID.setTimeStep(4000);
  while (processStatus)
  {
     stateHeaterFan(1);
    myPID.run();
    stateHeater(ssrState);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  myPID.stop();
  stateHeater(0);
  targetTemp=0;
  ssrState=0;
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
  }
  else
  {
    if (millis() - lastHeaterOpen > MIN_FANTIME)
    {
      digitalWrite(heaterFan, HIGH);
    }
  }
}
void savePIDEEPROM(double p, double i, double d)
{
  EEPROM.writeDouble(503, p);
  EEPROM.writeDouble(495, i);
  EEPROM.writeDouble(487, d);
  EEPROM.commit();
  message = "Saved";
  loadPIDFromEEPROM();
}
void loadPIDFromEEPROM()
{
  KP = EEPROM.readDouble(503);
  KI = EEPROM.readDouble(495);
  KD = EEPROM.readDouble(487);
}