#include "gbj_appthermo_ds.h"
const String gbj_appthermo_ds::VERSION = "GBJ_APPTHERMO_DS 1.0.0";

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
  _sensors = 0;
  _temperature = 0.0;
  // Scan all sensor on the bus
  if (_sensor->conversion())
  {
    return errorHandler(_sensor->getLastResult());
  }
  while (_sensor->isSuccess(_sensor->sensors()))
  {
    // Ignore power-up temperature
    if (_sensor->getTemperature() != _sensor->getTemperatureIni())
    {
      _sensors++;
      _temperature += _sensor->getTemperature();
    }
    // Correct resolution for next run
    if (_sensor->getResolutionBits() != _resolution)
    {
      SERIAL_VALUE("resolution", _sensor->getResolutionBits());
      _sensor->cacheResolutionBits(_resolution);
      if (_sensor->setCache())
      {
        return errorHandler(_sensor->getLastResult());
      }
    }
  }
  // Calculate average temperature
  if (_sensors)
  {
    _temperature /= _sensors;
    return setLastResult();
  }
  else
  {
    return setLastResult(ResultCodes::ERROR_NODEVICE);
  }
}
