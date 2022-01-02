#include "gbj_appthermo_ds.h"
const String gbj_appthermo_ds::VERSION = "GBJ_APPTHERMO_DS 1.1.0";

gbj_appthermo_ds::ResultCodes gbj_appthermo_ds::errorHandler(
  gbj_ds18b20::ResultCodes errSensor)
{
  setLastResult(ResultCodes::ERROR_UKNOWN);
  switch (errSensor)
  {
    case gbj_ds18b20::SUCCESS:
      setLastResult();
      break;

    case gbj_ds18b20::END_OF_LIST:
    case gbj_ds18b20::ERROR_NO_DEVICE:
    case gbj_ds18b20::ERROR_CONVERSION:
      setLastResult(ResultCodes::ERROR_NODEVICE);
      break;

    case gbj_ds18b20::ERROR_CRC_ADDRESS:
      setLastResult(ResultCodes::ERROR_ADDR);
      break;

    case gbj_ds18b20::ERROR_CRC_SCRATCHPAD:
      setLastResult(ResultCodes::ERROR_DATA);
      break;

    case gbj_ds18b20::ERROR_NO_ALARM:
    case gbj_ds18b20::ERROR_ALARM_LOW:
    case gbj_ds18b20::ERROR_ALARM_HIGH:
      break;
  }
  return getLastResult();
}

gbj_appthermo_ds::ResultCodes gbj_appthermo_ds::measure()
{
  byte temps = 0;
  temperature_ = 0.0;
  // Scan all sensor on the bus
  if (sensor_->conversion())
  {
    return errorHandler(sensor_->getLastResult());
  }
  while (sensor_->isSuccess(sensor_->sensors()))
  {
    // Update individual sensors
    for (byte i = 0; i < getSensors(); i++)
    {
      if (listSensors_[i].id == sensor_->getId())
      {
        listSensors_[i].temperature = sensor_->getTemperature();
        SERIAL_LOG4("id=",
                    listSensors_[i].id,
                    SERIAL_F(", temp="),
                    listSensors_[i].temperature);
        break;
      }
    }
    // Ignore power-up temperature
    if (sensor_->getTemperature() != sensor_->getTemperatureIni())
    {
      // Count sensor and sum its temperature for average
      temps++;
      temperature_ += sensor_->getTemperature();
    }
    // Correct resolution for next run
    if (sensor_->getResolutionBits() != resolution_)
    {
      SERIAL_VALUE("resolution", sensor_->getResolutionBits());
      sensor_->cacheResolutionBits(resolution_);
      if (sensor_->setCache())
      {
        return errorHandler(sensor_->getLastResult());
      }
    }
  }
  // Calculate average temperature
  if (temps)
  {
    temperature_ /= temps;
    return setLastResult();
  }
  else
  {
    return setLastResult(ResultCodes::ERROR_NODEVICE);
  }
}
