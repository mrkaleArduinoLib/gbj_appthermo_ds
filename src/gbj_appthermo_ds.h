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
  typedef void Handler();

  struct Handlers
  {
    Handler *onMeasureSuccess;
    Handler *onMeasureFail;
    Handler *onMeasureResol;
    Handler *onMeasureSensors;
  };

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
    handlers - A structure with pointers to various callback handler functions.
      - Data type: Handlers
      - Default value: structure with zeroed all handlers
      - Limited range: system address range

    RETURN: object
  */
  inline gbj_appthermo_ds(byte pinBus,
                          byte resolution,
                          Handlers handlers = Handlers())
  {
    handlers_ = handlers;
    sensor_ = new gbj_ds18b20(pinBus);
    timer_ = new gbj_timer(Timing::PERIOD_MEASURE);
    resolution_ = constrain(resolution, 9, 12);
    sensors_ = 0;
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
        // Report changes in number of sensors on the bus
        if (sensor_->getSensors() != sensors_)
        {
          SERIAL_LOG4("sensors: ", sensors_, " -> ", sensor_->getSensors())
          sensors_ = sensor_->getSensors();
          if (handlers_.onMeasureSensors)
          {
            handlers_.onMeasureSensors();
          }
        }
        SERIAL_VALUE("tempAvg", getTemperature())
        if (handlers_.onMeasureSuccess)
        {
          handlers_.onMeasureSuccess();
        }
      }
      else
      {
        SERIAL_VALUE("error", getLastResult())
        if (handlers_.onMeasureFail)
        {
          handlers_.onMeasureFail();
        }
      }
    }
  }

  // Set timer period input as unsigned long in milliseconds
  inline void setPeriod(unsigned long period)
  {
    period = period == 0 ? Timing::PERIOD_MEASURE : period;
    period = max(period, static_cast<unsigned long>(sensor_->getConvMillis()));
    timer_->setPeriod(period);
  }
  // Set timer period input as String in seconds
  inline void setPeriod(String periodSec)
  {
    setPeriod(1000 * static_cast<unsigned long>(periodSec.toInt()));
  }

  // Getters
  inline unsigned long getPeriod() { return timer_->getPeriod(); }
  inline float getTemperature() { return temperature_; }
  inline gbj_ds18b20 *getSensorPtr() { return sensor_; }

private:
  enum Timing : unsigned int
  {
    PERIOD_MEASURE = 1000,
  };
  Handlers handlers_;
  gbj_timer *timer_;
  gbj_ds18b20 *sensor_;
  byte resolution_, sensors_;
  float temperature_;

  ResultCodes measure();
  ResultCodes errorHandler(gbj_ds18b20::ResultCodes errSensor);
};

#endif
