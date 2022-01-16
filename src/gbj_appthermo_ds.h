/*
  NAME:
  gbj_appthermo_ds

  DESCRIPTION:
  Application library for temperature measurement with DS18B20 sensor.
  - The library utilizes all sensors on the one-wire bus available
    and averages their temperature.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the license GNU GPL v3
  http://www.gnu.org/licenses/gpl-3.0.html (related to original code) and MIT
  License (MIT) for added code.

  CREDENTIALS:
  Author: Libor Gabaj
 */
#ifndef GBJ_APPTHERMO_DS_H
#define GBJ_APPTHERMO_DS_H

#if defined(__AVR__)
  #include <Arduino.h>
  #include <inttypes.h>
#elif defined(ESP8266) || defined(ESP32)
  #include <Arduino.h>
#elif defined(PARTICLE)
  #include <Particle.h>
#endif
#include "gbj_appbase.h"
#include "gbj_ds18b20.h"
#include "gbj_serial_debug.h"
#include "gbj_timer.h"

#undef SERIAL_PREFIX
#define SERIAL_PREFIX "gbj_appthermo_ds"

class gbj_appthermo_ds : public gbj_appbase
{
public:
  static const String VERSION;

  typedef void Handler();
  typedef struct Temperatures
  {
    byte id;
    float temperature;
  } Temperatures;

  /*
    Constructor

    DESCRIPTION:
    Constructor creates the class instance object and sets operational
    parameters.

    PARAMETERS:
    pinBus - Number of GPIO pin of the microcontroller managing one-wire bus.
      - Data type: non-negative integer
      - Default value: none
      - Limited range: 0 ~ 255
    resolution - Number of bits for measurement resolution.
      - Data type: non-negative integer
      - Default value: none
      - Limited range: 9 ~ 12
    onMeasure - Pointer to a callback function within that receives no
      parameters and returns no value, and is called within every internal timer
      run at successful as well as failed temperature measurement.
      - Data type: Handler
      - Default value: 0
      - Limited range: system address range

    RETURN: object
  */
  inline gbj_appthermo_ds(byte pinBus, byte resolution, Handler *onMeasure = 0)
  {
    onMeasure_ = onMeasure;
    sensor_ = new gbj_ds18b20(pinBus);
    timer_ = new gbj_timer(Timing::PERIOD_MEASURE);
    resolution_ = constrain(resolution, 9, 12);
    // Create cache for sensors id and temperature
    if (getSensors())
    {
      byte i = 0;
      listSensors_ = new Temperatures[getSensors()];
      while (sensor_->isSuccess(sensor_->sensors()))
      {
        listSensors_[i++].id = sensor_->getId();
      }
    }
    setSensorIds();
  }

  /*
    Processing.

    DESCRIPTION:
    The method should be called in an application sketch loop.
    It processes main functionality and is controlled by the internal timer.

    PARAMETERS: None

    RETURN: none
  */
  inline void run()
  {
    if (timer_->run())
    {
      measure();
      if (isSuccess())
      {
        SERIAL_VALUE("sensors", getSensors());
        SERIAL_VALUE("tempAvg", getTemperature());
      }
      else
      {
        SERIAL_VALUE("error", getLastResult());
      }
      if (onMeasure_)
      {
        onMeasure_();
      }
    }
  }

  // Setters
  inline void setPeriod(unsigned long period) { timer_->setPeriod(period); }

  // Getters
  inline unsigned long getPeriod() { return timer_->getPeriod(); }
  inline float getTemperature() { return temperature_; }
  inline byte getSensors() { return sensor_->getSensors(); }
  inline const char *getSensorIds() { return sensorIds_; }
  inline byte getResolutionBits() { return sensor_->getResolutionBits(); }
  inline float getResolutionTemp() { return sensor_->getResolutionTemp(); }
  inline bool isMeasured() { return isSuccess(); }
  inline Temperatures *getCachePtr() { return listSensors_; }

private:
  enum Timing : unsigned int
  {
    PERIOD_MEASURE = 1000,
  };
  Handler *onMeasure_;
  gbj_timer *timer_;
  gbj_ds18b20 *sensor_;
  Temperatures *listSensors_;
  byte resolution_;
  float temperature_;
  char sensorIds_[20];

  ResultCodes measure();
  ResultCodes errorHandler(gbj_ds18b20::ResultCodes errSensor);
  /*
    Textual list of temperature sensors' identifiers.

    DESCRIPTION:
    The block composes a textual list of temperature sensors' identifiers
    in form <count>(<id1>, <id2>, ...), e.g. 2(108, 35). The list is
    suitable for publishing to IoT platforms.
  */
  inline void setSensorIds()
  {
    String result = String(getSensors());
    if (getSensors())
    {
      result += " (";
      for (byte i = 0; i < getSensors(); i++)
      {
        result += (i ? ", " : "") + String(listSensors_[i].id);
      }
      result += ")";
    }
    strcpy(sensorIds_, result.c_str());
  }
};

#endif
