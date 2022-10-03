#pragma once
#include "Arduino.h"
#include "EEPROM.h"
class functionLib
{
private:
public:
String message = "Waiting for process";
String toastMessage = "";
functionLib();
void saveToEEPROM(String qsid, String qpass);
void loadFromEEPROM();
void savePIDEEPROM(double p, double i, double d);
void loadPIDFromEEPROM();
String esid;
String epass = "";
double KP = 0.0, KI = 0.0, KD = 0.0;
};