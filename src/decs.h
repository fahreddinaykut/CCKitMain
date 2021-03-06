#include <ESPAsyncWebServer.h>
#include "Arduino.h"
#include <esp_now.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "myfonts.h"
#include "html.h"
#include "EEPROM.h"
#include <AutoPID.h>
#include <Fonts/FreeSans9pt7b.h>

#define humidifierPin 26
#define heaterFan 25
#define ssrPin 13

#define MIN_FANTIME 20000

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

union twoByte
{
    byte bVal[2];
    int iVal;
};

#define CHANNEL 1

#define cmdTemp 0x01
#define cmdHum 0x02
#define cmdCamStatus 0x03
#define cmdResponseSetting 0x04
#define cmdFlashToggle 0x5
#define cmdCamTempError 0x6
#define cmdCamUpdateMode 0x7
double temp, hum, targetTemp, targetHum, tempOutputVal, humOutputVal, KP = 0.0, KI = 0.0, KD = 0.0;

String esid;
String epass = "";

const char *ssidAP = "CCKIT";
const char *passwordAP = "12345678";

IPAddress staticIP(192, 168, 1, 151);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 254);

byte sensorError = 0;
byte camStatus;
byte processStatus = 0;
byte peerConnected = 0;
byte camWifiSettingResponse = 0;
uint8_t broadcastAddress[] = {0xE0, 0xE2, 0xE6, 0xCF, 0x9D, 0xAC};
uint8_t mode = 0;
long lastHeaterOpen;

String message = "Waiting for process";

String ssidRecv;
String passRecv;

IPAddress IP;
esp_now_peer_info_t slave;

typedef struct broadcast_message
{
    char type[16] = "unknown"; // data or broadcast
    char ssid[16] = "slave";   // slave or master
    char pass[16] = "unknown"; // slider pan tilt
    uint8_t datalen = 1;
    char data[128] = " ";
} broadcast_message;

// FUNCTIONS//
uint8_t loadWifiMode();
void writeWifiMode(uint8_t mode);
void processData(std::string value);
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
void initDisplay();
void composeDisplay();
void drawCentreString(const char *buf, int x, int y);
void displayTask(void *parameter);
void processAction(int ttem, int thum);
void humidityTask(void *parameter);
void heaterTask(void *parameter);
void stateHeater(byte state);
void stateHeaterFan(byte state);
void savePIDEEPROM(double p, double i, double d);
void loadPIDFromEEPROM();
////////////////HANDLES///////////////

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

    Serial.print(rVal);
    Serial.print("\t");
    Serial.println(gVal);
    if (processStatus)
    {
        processStatus = 0;
    }
    else
    {
        processStatus = 1;
    }
    // send answer to client
    request->send(200, "text/plane", "1");
    processAction(rVal.toInt(), gVal.toInt());
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
void handleStartAP(AsyncWebServerRequest *request)
{

    // send answer to client
    request->send(200, "text/plane", "1");
    writeWifiMode(2);
    Serial.println("wifi mode 2");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    ESP.restart();
}

void handleLiveData(AsyncWebServerRequest *request)
{
    String liveData;
    liveData.concat(String(temp));
    liveData.concat("|");
    liveData.concat(String(targetTemp));
    liveData.concat("|");
    liveData.concat(String(hum));
    liveData.concat("|");
    liveData.concat(String(targetHum));
    liveData.concat("|");
    liveData.concat(String(KP));
    liveData.concat("|");
    liveData.concat(String(KI));
    liveData.concat("|");
    liveData.concat(String(KD));
    request->send(200, "text/plane", String(liveData));
}
void handlePrcStatus(AsyncWebServerRequest *request)
{
    String processBtn;
    if (processStatus)
    {
        message = "Processing...";
        processBtn = "Stop";
    }
    else
    {
        if (millis() > 5000)
            message = "Stopped.";

        processBtn = "Process";
    }
    request->send(200, "text/plane", processBtn);
}
void handleNotifyCam(AsyncWebServerRequest *request)
{
    String status;
    if (peerConnected)
    {
        status = "Camera ON";
    }
    else
    {
        status = "Camera OFF";
    }
    request->send(200, "text/plane", status);
}

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
void handleEmergencyStop(AsyncWebServerRequest *request)
{

    String req = request->arg("w");

    Serial.println(req);

    // send answer to client
    request->send(200, "text/plane", "1");
    processStatus = 0;
    Serial.println("emergency Stop");
    message = "Emergency Stop";
}
void handleCamUpdate(AsyncWebServerRequest *request)
{

    // send answer to client
    request->send(200, "text/plane", "1");
    processStatus = 0;
    Serial.println("Cam Update");
    message = "Starting cam update mode";
    uint8_t senddata[] = {cmdCamUpdateMode};
    std::string dataval = std::string((char *)senddata, 1);
    sendData(dataval, 1);
}
void handleSavePID(AsyncWebServerRequest *request)
{
    double kp = request->arg("p").toDouble();
    double ki = request->arg("i").toDouble();
    double kd = request->arg("d").toDouble();
    Serial.print("kp ");
    Serial.print(kp);
    Serial.print("ki ");
    Serial.print(ki);
    Serial.print("kd ");
    Serial.print(kd);
    request->send(200, "text/plane", "1");
    Serial.println(" Saving PID");
    savePIDEEPROM(kp, ki, kd);
}

void handleFlash(AsyncWebServerRequest *request)
{

    String req = request->arg("w");

    Serial.println(req);

    // send answer to client
    request->send(200, "text/plane", "1");
    uint8_t senddata[] = {cmdFlashToggle};
    std::string dataval = std::string((char *)senddata, 1);
    sendData(dataval, 1);
}
