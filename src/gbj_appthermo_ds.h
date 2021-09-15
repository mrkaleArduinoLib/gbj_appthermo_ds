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

class gbj_appthermo_ds : gbj_appbase
{
public:
  static const String VERSION;

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

    RETURN: object
  */
  inline gbj_appthermo_ds(byte pinBus, byte resolution)
  {
    _sensor = new gbj_ds18b20(pinBus);
    _timer = new gbj_timer(Timing::PERIOD_MEASURE);
    _resolution = constrain(resolution, 9, 12);
  }

  /*
    Processing.

    DESCRIPTION:
    The method should be called in an application sketch loop.
    It processes main functionality and is control by the internal timer.

    PARAMETERS: None

    RETURN: none
  */
  inline void run()
  {
    if (_timer->run())
    {
      measure();
      if (isSuccess())
      {
        SERIAL_VALUE_UNIT("temperature", getTemperature(), "'C");
      }
      else
      {
        SERIAL_VALUE("error", getLastResult());
      }
    }
  }

  // Setters
  inline void setPeriod(unsigned long period) { _timer->setPeriod(period); }

  // Getters
  inline unsigned long getPeriod() { return _timer->getPeriod(); }
  inline float getTemperature() { return _temperature; }
  inline byte getSensors() { return _sensors; }
  inline bool isMeasured() { return isSuccess(); }

private:
  enum Timing : unsigned int
  {
    PERIOD_MEASURE = 1000,
  };
  gbj_timer *_timer;
  gbj_ds18b20 *_sensor;
  byte _resolution;
  byte _sensors;
  float _temperature;

  ResultCodes measure();
  ResultCodes errorHandler(gbj_ds18b20::ResultCodes errSensor);
};

#endif
