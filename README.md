<a id="library"></a>

# gbj\_appthermo\_ds
This is an application library, which is used usually as a project library for particular PlatformIO project. It encapsulates the functionality of `DS18B20 temperature sensors`. The encapsulation provides following advantages:

* Functionality is hidden from the main sketch.
* The library follows the principle `separation of concerns`.
* The library is reusable for various projects without need to code temperature measurement management.
* Update in library is valid for all involved projects.
* It specifies (inherits from) the parent application library `gbj__appcore`.
* It utilizes funcionality and error handling from the parent class.


## Fundamental functionality
* The library averages temperatures from all active and valid sensors on one-wire bus and statistically smooths the average temperature.
* Valid sensor is one which temperature value is whithin valid range of temperatures.


<a id="dependency"></a>

## Dependency
* **gbj\_appcore**: Parent library for all application libraries loaded from the file `gbj_appcore.h`.
* **gbj\_appfilter**: Library for filtering measured temperature values against a valid range loaded from the file `gbj_appfilter.h`.
* **gbj\_ds18b20**: Library for processing temperature sensors `DS18B20` loaded from the file `gbj_ds18b20.h`.
* **gbj\_exponential**: Library for statistical smoothing final average temperature loaded from the file `gbj_exponential.h`.
* **gbj\_serial\_debug**: Auxilliary library for debug serial output loaded from the file `gbj_serial_debug.h`. It enables to exclude serial outputs from final compilation.
* **paulstoffregen/OneWire**: Third party library for communication on one-wire bus.
* **Arduino.h**: Main include file for the Arduino SDK.

#### Arduino platform
* **inttypes.h**: Integer type conversions. This header file includes the exact-width integer definitions and extends them with additional facilities provided by the implementation.


<a id="interface"></a>

## Custom data types
* [Handler](#handler)
* [Handlers](#handlers)

## Interface
* [gbj_appthermo_ds()](#gbj_appthermo_ds)
* [run()](#run)
* [getTemperature()](#getTemperature)
* [getSensorPtr()](#getSensorPtr)
* [getIdList()](#getIdList)
* [getIds()](#getIds)


<a id="handler"></a>

## Handler

#### Description
The template or the signature of a callback function, which is called at particular event in the processing. It can be utilized for instant communicating with other modules of the application (project).
* A handler is just a bare function without any input parameters and returning nothing.
* A handler can be declared just as `void` type.

#### Syntax
    typedef void Handler()

#### Parameters
None

#### Returns
None

#### See also
[Handlers](#handlers)

[Back to interface](#interface)


<a id="handlers"></a>

## Handlers

#### Description
Structure of pointers to handlers each for particular event in processing.
* Individual or all handlers do not need to be defined in the main sketch, just those that are useful.

#### Syntax
    struct Handlers
    {
      Handler *onMeasureSuccess;
      Handler *onMeasureFail;
      Handler *onMeasureResol;
      Handler *onMeasureSensors;
    }

#### Parameters
* **onMeasureSuccess**: Pointer to a callback function, which is called after successful temperature measurement.
* **onMeasureFail**: Pointer to a callback function, which is called after failed temperature measurement.
* **onMeasureResol**: Pointer to a callback function, which is called after changing the temperature measurement resolution.
* **onMeasureSensors**: Pointer to a callback function, which is called after changing the number of temperature sensors on the bus. Initially this number is set to zero, so that this handler is always called at least once at the very first measurement.

#### Example
```cpp
void onThermoSuccess()
{
  ...
}
void onThermoFail()
{
  ...
}
void onThermoResol()
{
  ...
}
void onThermoSensors()
{
  ...
}
gbj_appthermo_ds::Handlers handlersThermo = {
  .onMeasureSuccess = onThermoSuccess,
  .onMeasureFail = onThermoFail,
  .onMeasureResol = onThermoResol,
  .onMeasureSensors = onThermoSensors,
};
gbj_appthermo_ds thermo = gbj_appthermo_ds(..., handlersThermo);
```

#### See also
[Handler](#handler)

[gbj_appthermo_ds](#gbj_appthermo_ds)

[Back to interface](#interface)


<a id="gbj_appthermo_ds"></a>

## gbj\_appthermo\_ds()

#### Description
Constructor creates the class instance object and initiates internal resources.
* It inputs operational parameters for temperature sensors.
* The input resolution is set to all sensors on the bus.

#### Syntax
    gbj_appthermo_ds(byte pinBus, float tempMax, float tempMin, float smoothingFactor, byte resolution, Handlers handlers)

#### Parameters
* **pinBus**: Number of GPIO pin of the microcontroller managing one-wire bus.
  * *Valid values*: positive integers 0 ~ 255
  * *Default value*: none

* **tempMax**: Maximal temperature in centigrades (°C) of valid range.
  * *Valid values*: rational number
  * *Default value*: none

* **tempMin**: Minimal temperature in centigrades (°C) of valid range.
  * *Valid values*: rational number
  * *Default value*: 0.0

* **smoothingFactor**: Smoothing factor for exponential filtering.
  * *Valid values*: real number 0.0 ~ 1.0
  * *Default value*: 0.2

* **resolution**: Number of bits for temperature measurement resolution.
  * *Valid values*: positive integers 9 ~ 12
  * *Default value*: 10

* **handlers**: Pointer to a structure of callback functions. This structure as well as handlers should be defined in the main sketch.
  * *Data type*: Handlers
  * *Default value*: empty structure

#### Returns
Object performing temperature measurement.

#### See also
[Handlers()](#handlers)

[Back to interface](#interface)


<a id="run"></a>

## run()

#### Description
The execution method should be called when the new temperature value is needed, usually in a timer or suitable handler.
* The method executes conversion for all sensors on the bus at once.
* An average (mean) temperature is calculated from temperature values measured by all successful and valid sensors.
  * Successful sensor is one that communicates on the bus.
  * Valid sensor is one which temperature value is whithin valid range of temperatures.
* The final temperature is obtained from statistical smoothing of the average temperature by exponential filtering.
* The handlers are called at every run, if they are declared in the constructor and corresponding conditions have been met.

#### See also
[Handlers()](#handlers)

[gbj_appthermo_ds()](#gbj_appthermo_ds)

[Back to interface](#interface)


<a id="getTemperature"></a>

## getTemperature()

#### Description
The method returns the recently known averaged and statistically smoothed temperature from all active and valid sensors with successful recent conversion.
* In case of entirelly failed conversion at all sensors on the bus the average temperature is `0`.
* The precision of the averaged temperature value, but not precision of the measurement itself, depends on the measurement resolution (number of used fraction digits) of sensors set by [constructor](#gbj_appthermo_ds).

#### Syntax
    float getTemperature()

#### Parameters
None

#### Returns
The averaged temperature in degrees of Celsius (°C).

[Back to interface](#interface)


<a id="getSensorPtr"></a>

## getSensorPtr()

#### Description
The method returns the pointer to the internal object controlling the DS18B20 temperature sensor itself.
* It allows to utilize entire interface of the corresponding library `gbj_ds18b20` without wrapping or mirroring it.
* The methods of this internal object are valid for currently selected sensor on the bus.

#### Syntax
	gbj_ds18b20 *getSensorPtr()

#### Parameters
None

#### Returns
Pointer to the control object of the DS18B20 temperature sensor hardware.

#### Example
```cpp
gbj_appthermo_ds thermo = gbj_appthermo_ds(...);
void setup()
{
  thermo.getSensorPtr()->getSensors();
}
```

[Back to interface](#interface)


<a id="getIdList"></a>

## getIdList()

#### Description
The method returns comma separated list of identifiers of valid temperature sensors.
* As a valid temperature sensor is consider a sensor, which measured temperature is within valid range of temperatures.
* The identifier is the <abbr title='Cyclic Redundancy Code'>CRC</abbr> code of the temperature sensor's <abbr title='Read Only Memory'>ROM</abbr> from its last byte.

#### Syntax
    String getIdList()

#### Parameters
None

#### Returns
Comma separated list of valid temperature sensors' identifiers.

#### See also
[getIds()](#getIds)

[Back to interface](#interface)


<a id="getIds"></a>

## getIds()

#### Description
The method returns number of valid temperature sensors.
* As a valid temperature sensor is consider a sensor, which measured temperature is within valid range of temperatures.

#### Syntax
    byte getIds()

#### Parameters
None

#### Returns
Number of valid temperature sensors.

#### See also
[getIdList()](#getIdList)

[Back to interface](#interface)
