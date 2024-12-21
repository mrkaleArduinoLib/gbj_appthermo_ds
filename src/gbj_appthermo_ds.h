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

#include "gbj_appcore.h"
#include "gbj_appfilter.h"
#include "gbj_appstatistics.h"
#include "gbj_ds18b20.h"
#include "gbj_exponential.h"
#include "gbj_serial_debug.h"
#include <Arduino.h>

#undef SERIAL_PREFIX
#define SERIAL_PREFIX "gbj_appthermo_ds"

// Class definition for DS18B20 thermometers
class gbj_appthermo_ds
  : public gbj_appcore
  , gbj_appstatistics
{
public:
  typedef void Handler();

  struct Handlers
  {
    Handler *onMeasureSuccess;
    Handler *onMeasureFailure;
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

  // Measuring temperature and determining current number of valid sensors. The
  // resulting temperature is the average from all valid sensors.
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
      if (handlers_.onMeasureFailure != nullptr)
      {
        handlers_.onMeasureFailure();
      }
    }
  }

  // Reset statistical values for another statistical period
  inline void reset()
  {
    statMax_.reset();
    statMin_.reset();
    statAvg_.reset();
  }

  // Return pointer to temperature sensor
  inline gbj_ds18b20 *getSensorPtr() { return sensor_; }
  // Return comma separated list of active temperature sensors' identiviers
  inline String getIdList() { return idList_; }
  // Return number of active temperature sensors
  inline byte getIds() { return ids_; }
  // Return recently measured and smoothed temperature
  inline float getTemperature() { return temperature_; }
  // Return recently measured and smoothed temperature rounded to input decimal
  // places
  inline float getTemperatureRound(byte precision = 2)
  {
    return roundoff(temperature_, precision);
  }
  // Return recently only measured temperature
  inline float getTemperatureRaw() { return temperatureRaw_; }
  // Return recently only measured temperature rounded to input decimal places
  inline float getTemperatureRawRound(byte precision = 2)
  {
    return roundoff(temperatureRaw_, precision);
  }
  // Return average temperature within the current statistical period
  inline float getTemperatureAvg() { return statAvg_.get(); };
  // Return maximal temperature within the current statistical period
  inline float getTemperatureMax() { return statMax_.get(); };
  // Return minimal temperature within the current statistical period
  inline float getTemperatureMin() { return statMin_.get(); };
  // Return uptime milliseconds of the very first maximal temperature within the
  // current statistical period
  inline unsigned long getTemperatureMaxTime() { return statMax_.getTime(); };
  // Return uptime milliseconds of the very first minimal temperature within the
  // current statistical period
  inline unsigned long getTemperatureMinTime() { return statMin_.getTime(); };

private:
  // Utilized handlers
  Handlers handlers_;
  // Actuator for filtering temperature values
  gbj_appfilter<float> *filter_;
  // Actuator for statistical smoothing temperature values
  gbj_exponential *smooth_;
  // Pointer to thermometer sensor object
  gbj_ds18b20 *sensor_;
  // Current temperature measurement resolution code
  byte resolution_;
  // Number of active temperature sensors
  byte ids_;
  // List of identifiers of active temperature sensors
  String idList_;
  // Recently measured and smoothed temperature in Celsius centigrades
  float temperature_;
  // Recently only measured temperature in Celsius centigrades
  float temperatureRaw_;

  // Measure temperature
  ResultCodes measure();
  // Process and translate errors
  ResultCodes errorHandler(gbj_ds18b20::ResultCodes errSensor);

  // Object for temperature maximum
  StatisticMax statMax_ = StatisticMax();
  // Object for temperature minimum
  StatisticMin statMin_ = StatisticMin();
  // Object for temperature average
  StatisticAvg statAvg_ = StatisticAvg();

  // Register resulting temperature for statistical evaluation
  inline void statRegister()
  {
    statMax_.set(temperature_);
    statMin_.set(temperature_);
    statAvg_.set(temperature_);
  }
};

#endif
