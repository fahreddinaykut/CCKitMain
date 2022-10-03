#include "functionLib.h"

functionLib::functionLib()
{

}
void functionLib::saveToEEPROM(String qsid, String qpass)
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
void functionLib::loadFromEEPROM()
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
void functionLib::savePIDEEPROM(double p, double i, double d)
{
  EEPROM.writeDouble(503, p);
  EEPROM.writeDouble(495, i);
  EEPROM.writeDouble(487, d);
  EEPROM.commit();
  message = "PID Saved";
  toastMessage="PID Settings Saved";
  loadPIDFromEEPROM();
}
void functionLib::loadPIDFromEEPROM()
{
  KP = EEPROM.readDouble(503);
  KI = EEPROM.readDouble(495);
  KD = EEPROM.readDouble(487);
}