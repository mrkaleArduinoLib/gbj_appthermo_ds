#include "gbj_appthermo_ds.h"

gbj_appthermo_ds::ResultCodes gbj_appthermo_ds::errorHandler(
  gbj_ds18b20::ResultCodes errSensor)
{
  setLastResult(ResultCodes::ERROR_UNKNOWN);
  switch (errSensor)
  {
    case gbj_ds18b20::SUCCESS:
      setLastResult();
      break;

    case gbj_ds18b20::END_OF_LIST:
    case gbj_ds18b20::ERROR_NO_DEVICE:
    case gbj_ds18b20::ERROR_NO_SENSOR:
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
  idList_ = "";
  ids_ = 0;
  temperature_ = 0.0;
  // Execute conversion for temperature measurement
  if (sensor_->conversion())
  {
    temperature_ = filter_->getMinimum();
    return errorHandler(sensor_->getLastResult());
  }
  // Scan all sensors on the bus
  while (sensor_->isSuccess(sensor_->sensors()))
  {
    filter_->setValue(sensor_->getTemperature());
    if (filter_->isValid())
    {
      idList_.concat(ids_ > 0 ? ", " : "");
      idList_.concat(String(sensor_->getId()));
      ids_++;
      temperature_ += filter_->getValue();
    }
    // Correct resolution for next run
    if (sensor_->getResolutionBits() != resolution_)
    {
      SERIAL_CHANGE("Resolution", sensor_->getResolutionBits(), resolution_)
      sensor_->cacheResolutionBits(resolution_);
      if (sensor_->setCache())
      {
        return errorHandler(sensor_->getLastResult());
      }
      else if (handlers_.onMeasureResol != nullptr)
      {
        handlers_.onMeasureResol();
      }
    }
  }
  // Calculate average temperature
  if (ids_ > 0)
  {
    temperatureRaw_ = temperature_ / ids_;
    temperature_ = smooth_->getValue(temperatureRaw_);
    statRegister();
    return setLastResult();
  }
  else
  {
    temperature_ = temperatureRaw_ = filter_->getMinimum();
    return setLastResult(ResultCodes::ERROR_NODEVICE);
  }
}
