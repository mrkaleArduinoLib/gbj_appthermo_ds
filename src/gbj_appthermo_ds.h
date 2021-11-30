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

    RETURN: object
  */
  inline gbj_appthermo_ds(byte pinBus, byte resolution)
  {
    sensor_ = new gbj_ds18b20(pinBus);
    timer_ = new gbj_timer(Timing::PERIOD_MEASURE);
    resolution_ = constrain(resolution, 9, 12);
  }

  /*
    Initialization.

    DESCRIPTION:
    The method exports last known temperature from sensors, which ids are
    present in the external referenced array.
    - The method ignores a sensor's temperature that is equal to initial
      temperature, which is sign of failed conversion (measurement).
    - The method is useful when multiple sensors measure in various spaces and
      the mean temperature might be useless.
    - The temperature is exported only for sensors that are active on the bus
      and have ids present in the input array.
    - The method calculates average temperature in either case from all active
      sensors on the bus.
    - The method does not need to be called in a sketch's setup, if temperature
      of individual sensors is not desired.

    PARAMETERS:
    listSensors - Pointer to an external array corresponding structure type
      with sensor identifier in the field `id` of items.
      - Data type: Temperatures
      - Default value: 0
      - Limited range: system address space
    listCount - Number of records (rows, structures) in the list.
      - Data type: size_t
      - Default value: 0
      - Limited range: 0 ~ getSensors()

    RETURN: None
  */
  inline void begin(Temperatures *listSensors = 0, size_t listCount = 0)
  {
    SERIAL_TITLE("begin");
    SERIAL_VALUE("Exports", listCount);
    listSensors_ = listSensors;
    listCount_ = listCount;
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
    }
  }

  // Setters
  inline void setPeriod(unsigned long period) { timer_->setPeriod(period); }

  // Getters
  inline unsigned long getPeriod() { return timer_->getPeriod(); }
  inline float getTemperature() { return temperature_; }
  inline byte getSensors() { return sensors_; }
  inline byte getResolutionBits() { return sensor_->getResolutionBits(); }
  inline float getResolutionTemp() { return sensor_->getResolutionTemp(); }
  inline bool isMeasured() { return isSuccess(); }

private:
  enum Timing : unsigned int
  {
    PERIOD_MEASURE = 1000,
  };
  gbj_timer *timer_;
  gbj_ds18b20 *sensor_;
  Temperatures *listSensors_;
  byte resolution_;
  byte sensors_;
  byte listCount_;
  float temperature_;

  ResultCodes measure();
  ResultCodes errorHandler(gbj_ds18b20::ResultCodes errSensor);
};

#endif
