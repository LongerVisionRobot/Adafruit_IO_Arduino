//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Copyright (c) 2015-2016 Adafruit Industries
// Authors: Tony DiCola, Todd Treece
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.
//
#include "AdafruitIO_Data.h"
#include "AdafruitIO_Feed.h"

AdafruitIO_Data::AdafruitIO_Data()
{
  feed = 0;
  _csv = 0;
  _value = 0;
  _lat = 0;
  _lon = 0;
  _ele = 0;
}

AdafruitIO_Data::AdafruitIO_Data(AdafruitIO_Feed *f)
{
  feed = f;
  _csv = 0;
  _value = 0;
  _lat = 0;
  _lon = 0;
  _ele = 0;
}

AdafruitIO_Data::AdafruitIO_Data(AdafruitIO_Feed *f, char *csv)
{
  feed = f;
  _csv = csv;
  _value = 0;
  _lat = 0;
  _lon = 0;
  _ele = 0;

  _parseCSV();
}

bool AdafruitIO_Data::setCSV(char *csv)
{
  _csv = csv;
  return _parseCSV();
}

void AdafruitIO_Data::setLocation(double lat, double lon, double ele)
{
  // if lat, lon, ele == 0, don't set them
  if((abs(0-lat) < 0.000001) && (abs(0-lon) < 0.000001) && (abs(0-ele) < 0.000001))
    return;

  _lat = lat;
  _lon = lon;
  _ele = ele;
}

static char _converted[AIO_DATA_LENGTH];

void AdafruitIO_Data::setValue(const char *value, double lat, double lon, double ele)
{
  _value = (char *)value;
  setLocation(lat, lon, ele);
}

void AdafruitIO_Data::setValue(char *value, double lat, double lon, double ele)
{
  _value = value;
  setLocation(lat, lon, ele);
}

void AdafruitIO_Data::setValue(bool value, double lat, double lon, double ele)
{
  if(value)
    _value = (char *)"1";
  else
    _value = (char *)"0";

  setLocation(lat, lon, ele);
}

void AdafruitIO_Data::setValue(String value, double lat, double lon, double ele)
{
  value.toCharArray(_value, value.length() + 1);
  setLocation(lat, lon, ele);
}

void AdafruitIO_Data::setValue(int value, double lat, double lon, double ele)
{
  memset(_converted, 0, sizeof(_converted));
  itoa(value, _converted, 10);
  _value = _converted;
  setLocation(lat, lon, ele);
}

void AdafruitIO_Data::setValue(unsigned int value, double lat, double lon, double ele)
{
  memset(_converted, 0, sizeof(_converted));
  utoa(value, _converted, 10);
  _value = _converted;
  setLocation(lat, lon, ele);
}

void AdafruitIO_Data::setValue(long value, double lat, double lon, double ele)
{
  memset(_converted, 0, sizeof(_converted));
  ltoa(value, _converted, 10);
  _value = _converted;
  setLocation(lat, lon, ele);
}

void AdafruitIO_Data::setValue(unsigned long value, double lat, double lon, double ele)
{
  memset(_converted, 0, sizeof(_converted));
  ultoa(value, _converted, 10);
  _value = _converted;
  setLocation(lat, lon, ele);
}

void AdafruitIO_Data::setValue(float value, double lat, double lon, double ele, int precision)
{
  memset(_converted, 0, sizeof(_converted));

  #if defined(ARDUINO_ARCH_AVR)
    // Use avrlibc dtostre function on AVR platforms.
    dtostre(value, _converted, 10, 0);
  #elif defined(ESP8266)
    // ESP8266 Arduino only implements dtostrf and not dtostre.  Use dtostrf
    // but accept a hint as to how many decimals of precision are desired.
    dtostrf(value, 0, precision, _converted);
  #else
    // Otherwise fall back to snprintf on other platforms.
    snprintf(_converted, sizeof(_converted)-1, "%f", value);
  #endif

  _value = _converted;
  setLocation(lat, lon, ele);
}

void AdafruitIO_Data::setValue(double value, double lat, double lon, double ele, int precision)
{
  memset(_converted, 0, sizeof(_converted));

  #if defined(ARDUINO_ARCH_AVR)
    // Use avrlibc dtostre function on AVR platforms.
    dtostre(value, _converted, 10, 0);
  #elif defined(ESP8266)
    // ESP8266 Arduino only implements dtostrf and not dtostre.  Use dtostrf
    // but accept a hint as to how many decimals of precision are desired.
    dtostrf(value, 0, precision, _converted);
  #else
    // Otherwise fall back to snprintf on other platforms.
    snprintf(_converted, sizeof(_converted)-1, "%f", value);
  #endif


  _value = _converted;
  setLocation(lat, lon, ele);
}

char* AdafruitIO_Data::feedName()
{
  if(! feed)
    return (char*)"";

  return (char *)feed->name;
}

char* AdafruitIO_Data::value()
{
  return toChar();
}

char* AdafruitIO_Data::toChar()
{
  return _value;
}

String AdafruitIO_Data::toString()
{
  return String(_value);
}

bool AdafruitIO_Data::toBool()
{
  if(strcmp(_value, "1") == 0 || _value[0] == 't' || _value[0] == 'T')
    return true;
  else
    return false;
}

bool AdafruitIO_Data::isTrue()
{
  return toBool();
}

bool AdafruitIO_Data::isFalse()
{
  return !toBool();
}

int AdafruitIO_Data::toInt()
{
  char* endptr;
  return (int)strtol(_value, &endptr, 10);
}

int AdafruitIO_Data::toPinLevel()
{
  if(isTrue())
    return HIGH;
  else
    return LOW;
}

unsigned int AdafruitIO_Data::toUnsignedInt()
{
  char* endptr;
  #ifdef ESP8266
    // For some reason strtoul is not defined on the ESP8266 platform right now.
    // Just use a strtol function and hope for the best.
    return (unsigned int)strtol(_value, &endptr, 10);
  #else
    return (unsigned int)strtoul(_value, &endptr, 10);
  #endif
}

float AdafruitIO_Data::toFloat()
{
  char* endptr;
  return (float)strtod(_value, &endptr);
}

double AdafruitIO_Data::toDouble()
{
  char* endptr;
  return strtod(_value, &endptr);
}

long AdafruitIO_Data::toLong()
{
  char* endptr;
  return strtol(_value, &endptr, 10);
}

unsigned long AdafruitIO_Data::toUnsignedLong()
{
  char* endptr;
  #ifdef ESP8266
      // For some reason strtoul is not defined on the ESP8266 platform right now.
      // Just use a strtol function and hope for the best.
      return (unsigned long)strtol(_value, &endptr, 10);
  #else
      return strtoul(_value, &endptr, 10);
  #endif
}

char* AdafruitIO_Data::toCSV()
{
  char csv[150];

  strcpy(csv, _value);
  strcat(csv, ",");
  strcat(csv, charFromDouble(_lat));
  strcat(csv, ",");
  strcat(csv, charFromDouble(_lon));
  strcat(csv, ",");
  strcat(csv, charFromDouble(_ele, 2));

  _csv = csv;

  return _csv;
}

double AdafruitIO_Data::lat()
{
  return _lat;
}

double AdafruitIO_Data::lon()
{
  return _lon;
}

double AdafruitIO_Data::ele()
{
  return _ele;
}

static char _double_buffer[20];

char* AdafruitIO_Data::charFromDouble(double d, int precision)
{
  memset(_double_buffer, 0, sizeof(_double_buffer));

  #if defined(ARDUINO_ARCH_AVR)
    // Use avrlibc dtostre function on AVR platforms.
    dtostre(d, _double_buffer, 10, 0);
  #elif defined(ESP8266)
    // ESP8266 Arduino only implements dtostrf and not dtostre.  Use dtostrf
    // but accept a hint as to how many decimals of precision are desired.
    dtostrf(d, 0, precision, _double_buffer);
  #else
    // Otherwise fall back to snprintf on other platforms.
    snprintf(_double_buffer, sizeof(_double_buffer)-1, "%f", d);
  #endif

  return _double_buffer;
}

bool AdafruitIO_Data::_parseCSV()
{
  // parse value from csv
  _value = strtok(_csv, ",");
  if (! _value) return false;

  // parse lat from csv and convert to float
  char *lat = strtok(NULL, ",");
  if (! lat) return false;

  _lat = atof(lat);

  // parse lon from csv and convert to float
  char *lon = strtok(NULL, ",");
  if (! lon) return false;

  _lon = atof(lon);

  // parse ele from csv and convert to float
  char *ele = strtok(NULL, ",");
  if (! ele) return false;

  _ele = atof(ele);

  return true;
}
