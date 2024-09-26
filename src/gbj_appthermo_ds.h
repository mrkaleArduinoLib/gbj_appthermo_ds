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

#include <Arduino.h>
#if defined(__AVR__)
  #include <inttypes.h>
#endif
#include "gbj_appcore.h"
#include "gbj_appfilter.h"
#include "gbj_ds18b20.h"
#include "gbj_exponential.h"
#include "gbj_serial_debug.h"

#undef SERIAL_PREFIX
#define SERIAL_PREFIX "gbj_appthermo_ds"

class gbj_appthermo_ds : public gbj_appcore
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
    tempMax - Maximum valid temperature.
      - Data type: float
      - Default value: none
    tempMin - Minimum valid temperature.
      - Data type: float
      - Default value: 0.0
    smoothingFactor - Smoothing factor for exponential filtering.
      - Data type: float
      - Default value: 0.2
      - Limited range: 0.0 ~ 1.0
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
                          float tempMax,
                          float tempMin = 0.0,
                          float smoothingFactor = 0.2,
                          byte resolution = 10,
                          Handlers handlers = Handlers())
  {
    handlers_ = handlers;
    sensor_ = new gbj_ds18b20(pinBus);
    filter_ = new gbj_appfilter<float>(tempMax, tempMin);
    smooth_ = new gbj_exponential(smoothingFactor);
    resolution_ = constrain(resolution, 9, 12);
  }

  /*
    Processing.

    DESCRIPTION:
    The method processes main functionality, i.e., measuring temperature and
    determining number of valid sensors.
    - The resulting temperature is the average from all valid sensors.

    PARAMETERS: None

    RETURN: none
  */
  inline void run()
  {
    measure();
    if (isSuccess())
    {
      // Report changes in number of sensors on the bus
      if (sensor_->getSensors() != ids_)
      {
        SERIAL_CHANGE("Sensors", sensor_->getSensors(), ids_)
        if (handlers_.onMeasureSensors != nullptr)
        {
          handlers_.onMeasureSensors();
        }
      }
      if (handlers_.onMeasureSuccess != nullptr)
      {
        handlers_.onMeasureSuccess();
      }
    }
    else
    {
      SERIAL_VALUE("Measurement ERROR", getLastResult())
      if (handlers_.onMeasureFail)
      {
        handlers_.onMeasureFail();
      }
    }
  }

  // Getters
  inline float getTemperature() { return temperature_; }
  inline gbj_ds18b20 *getSensorPtr() { return sensor_; }
  inline String getIdList() { return idList_; }
  inline byte getIds() { return ids_; }

private:
  Handlers handlers_;
  gbj_appfilter<float> *filter_;
  gbj_exponential *smooth_;
  gbj_ds18b20 *sensor_;
  byte resolution_, ids_;
  String idList_;
  float temperature_;

  ResultCodes measure();
  ResultCodes errorHandler(gbj_ds18b20::ResultCodes errSensor);
};

#endif
