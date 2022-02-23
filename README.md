<a id="library"></a>

# gbj\_appthermo_ds
This is an application library, which is used usually as a project library for particular PlatformIO project. It encapsulates the functionality of `DS18B20 temperature sensors`. The encapsulation provides following advantages:

* Functionality is hidden from the main sketch.
* The library follows the principle `separation of concerns`.
* The library is reusable for various projects without need to code temperature measurement management.
* Update in library is valid for all involved projects.
* It specifies (inherits from) the parent application library `gbj_appbase`.
* It utilizes funcionality and error handling from the parent class.


## Fundamental functionality
* The library averages temperatures from all active sensors on one-wire bus.
* It creates internal cache with identifiers and recent temperatures of all active temperature sensors on the one-wire bus.
* The library utilizes internal timer for periodical temperature measurement.


<a id="internals"></a>

## Internal parameters
Internal parameter is hard-coded in the library as enumerations and has neither setter nor getter associated.

* **Temperature measurement period** (1000 ms): It is a default time period for temperature measurement.


<a id="dependency"></a>

## Dependency
* **gbj\_appbase**: Parent library for all application libraries loaded from the file `gbj_appbase.h`.
* **gbj\_serial\_debug**: Auxilliary library for debug serial output loaded from the file `gbj_serial_debug.h`. It enables to exclude serial outputs from final compilation.
* **gbj\_timer**: Library for executing internal timer within an instance object loaded from the file `gbj_timer.h`.
* **gbj\_ds18b20**: Library for processing temperature sensors `DS18B20` loaded from the file `gbj_ds18b20.h`.

#### Arduino platform
* **Arduino.h**: Main include file for the Arduino SDK.
* **inttypes.h**: Integer type conversions. This header file includes the exact-width integer definitions and extends them with additional facilities provided by the implementation.

#### Espressif platform
* **Arduino.h**: Main include file for the Arduino platform.

#### Particle platform
* **Particle.h**: Includes alternative (C++) data type definitions.


<a id="constants"></a>

## Constants
* **VERSION**: Name and semantic version of the library.

Other constants, enumerations, result codes, and error codes are inherited from the parent library.


<a id="interface"></a>

## Custom data types
* [Handler](#handler)
* [Handlers](#handlers)
* [Temperature](#Temperature)

## Interface
* [gbj_appthermo_ds()](#gbj_appthermo_ds)
* [run()](#run)
* [setPeriod()](#period)
* [getPeriod()](#period)
* [getSensorIds()](#getSensorIds)
* [getTemperature()](#getTemperature)
* [getSensorPtr()](#getSensorPtr)
* [getCachePtr()](#getCachePtr)


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
    }

#### Parameters
* **onMeasureSuccess**: Pointer to a callback function, which is call after successful temperature measurement.
* **onMeasureFail**: Pointer to a callback function, which is call after failed temperature measurement.

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
gbj_appthermo_ds::Handlers handlersThermo = { .onMeasureSuccess = onThermoSuccess,
                                              .onMeasureFail = onThermoFail };
gbj_appthermo_ds thermo = gbj_appthermo_ds(..., handlersThermo);
```

#### See also
[Handler](#handler)

[gbj_appthermo_ds](#gbj_appthermo_ds)

[Back to interface](#interface)


<a id="Temperature"></a>

## Temperature

#### Description
Structure with temperature sensor identifiers (CRC field of hardware ROM) and their actual temperature in centigrades.
* The structure is primarily aimed as definition of a sensor record.

#### Declaration
    struct Temperature
    {
      byte id;
      float temperature;
    };

#### Syntax
    Temperature thermoRecords[]

#### Parameters
* **id**: Identifier of a temperature sensor.
  * *Valid values*: positive integers 0 ~ 255
  * *Default value*: none


* **temperature**: Temperature in centigrades as recent known value of a sensor.
  * *Valid values*: real numbers -55.0 ~ +125.0
  * *Default value*: none

[Back to interface](#interface)


<a id="gbj_appthermo_ds"></a>

## gbj_appthermo_ds()

#### Description
Constructor creates the class instance object and initiates internal resources.
* It inputs operational parameters for temperature sensors.
* It creates an internal timer for periodical temperature measurement.
* The input resolution is set to all sensors on the bus.

#### Syntax
    gbj_appthermo_ds(byte pinBus, byte resolution, Handlers handlers)

#### Parameters
* **pinBus**: Number of GPIO pin of the microcontroller managing one-wire bus.
  * *Valid values*: positive integers 0 ~ 255
  * *Default value*: none


* **resolution**: Number of bits for temperature measurement resolution.
  * *Valid values*: positive integers 9 ~ 12
  * *Default value*: none


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
The execution method as the implementation of the virtual method from the parent class, which should be called frequently, usually in the loop function of a sketch.
* The method executes conversion for all sensors on the bus at once.
* The final temperature is calculated as an average (mean) of temperature values measured by all successful sensors.
* The handlers are called at every run, if they are declared in the constructor.

#### See also
[Handlers()](#handlers)

[gbj_appthermo_ds()](#gbj_appthermo_ds)

[Back to interface](#interface)


<a id="period"></a>

## setPeriod(), getPeriod()

#### Description
The methods are just straitforward implementation of the virual methods from the parent class.

[Back to interface](#interface)


<a id="getSensorIds"></a>

## getSensorIds()

#### Description
The method composes a textual list of temperature sensors' identifiers in form _\<count\>(\<id1\>, \<id2\>, ...)_, e.g. 2(108, 35).

#### Syntax
    const char *getSensorIds()

#### Parameters
None

#### Returns
Pointer to the textual list of temperature sensors's identifiers.

[Back to interface](#interface)


<a id="getTemperature"></a>

## getTemperature()

#### Description
The method returns the recently known averaged temperature from all active sensors with successful recent conversion.
* In case of entirelly failed conversion at all sensors on the bus the average temperature is `0`.
* The precision of the averaged temperature value, but not precision of the measurement itself, depends on the measurement resolution (number of used fraction digits) of sensors set by [constructor](#gbj_appthermo_ds).

#### Syntax
    float getTemperature()

#### Parameters
None

#### Returns
The averaged temperature in degrees of Celsius.

[Back to interface](#interface)


<a id="getSensorPtr"></a>

## getSensorPtr()

#### Description
The method returns the pointer to the internal object controlling the DS18B20 temperature sensor itself. It allows to utilize entire interface of the corresponding library `gbj_ds18b20` without wrapping or mirroring it.

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


<a id="getCachePtr"></a>

## getCachePtr()

#### Description
The method provides pointer to the internal sensors cache, which is an array of type [Temperature](#Temperature) and size of number of active temperature sensors on the one-wire bus.
* The method puts the identifier and the current temperature of all active sensors on the bus in the cache regardless of those with failed conversion (measurement). Such sensors have initial temperature in the cache instead.
* The method calculates average temperature in either case from all active sensors on the bus ignoring sensors with failed conversion.

#### Syntax
    Temperature *getCachePtr()

#### Parameters
None

#### Returns
Pointer to the internal cache of temperature sensors identifiers and recent known temperatures.

#### See also
[Temperature](#Temperature)

[Back to interface](#interface)
