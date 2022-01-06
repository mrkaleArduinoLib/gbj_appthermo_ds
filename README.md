<a id="library"></a>

# gbj\_appthermo_ds
This is an application library, which is used usually as a project library for particular PlatformIO project. The library utilizes all active temperature sensors `DS18B20` on one-wire bus for measuring the ambient temperature.

* Library specifies (inherits from) the system `gbj_appbase` library.
* It utilizes error handling from the parent class.
* It averages temperatures from all sensors.
* It creates internal cache with identifiers and recent temperatures of all active temperature sensors on the one-wire bus.


<a id="dependency"></a>

## Dependency

* **gbj\_appbase**: Parent library for all application libraries loaded from the file `gbj_appbase.h`.
* **gbj\_timer**: Library for executing internal timer within an instance object loaded from the file `gbj_timer.h`.
* **gbj\_serial\_debug**: Auxilliary library for debug serial output loaded from the file `gbj_serial_debug.h`. It enables to exclude serial outputs from final compilation.
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

* **gbj\_appthermo_ds::VERSION**: Name and semantic version of the library.

Other constants and enumerations are inherited from the parent library.


<a id="interface"></a>

## Interface

* [gbj_appthermo_ds()](#gbj_appthermo_ds)
* [run()](#run)
* [setPeriod()](#period)
* [getPeriod()](#period)
* [getSensors()](#getSensors)
* [getTemperature()](#getTemperature)
* [getCachePtr()](#getCachePtr)
* [isMeasured()](#isMeasured)


## Custom data types

* [Handler()](#handler)
* [Temperatures](#Temperatures)


<a id="handler"></a>

## Handler

#### Description
The template or the signature of a handler function, which is called after every internal timer run, regardless of succesful or failed temperature measurement.
* A handler is just a bare function without any input parameters and returning nothing.
* A handler can be declared just as `void` type. It is not needed to be declared as `gbj_appthermo_ds::Handler` type.

#### Syntax
    gbj_appthermo_ds::Handler handler()
    void handler()

#### Parameters
None

#### Returns
None

[Back to interface](#interface)


<a id="Temperatures"></a>

## Temperatures

#### Description
Custom data type declaring structure with temperature sensor identifiers (CRC field of hardware ROM) and their actual temperature in centigrades.
* The data type is primarily aim for definition two dimensional arrays, while their items represent sensor records.

#### Declaration
    typedef struct Temperatures
    {
      byte id;
      float temperature;
    } Temperatures;

#### Syntax
    gbj_appthermo_ds::Temperatures thermoRecords[]

#### Parameters

* **id**: Identifier of a temperature sensor.
  * *Valid values*: positive integers 0 ~ 255
  * *Default value*: none


* **temperature**: Temperature in centigrades as recent know value of a sensor.
  * *Valid values*: real numbers -55.0 ~ +125.0
  * *Default value*: none

[Back to interface](#interface)


<a id="gbj_appthermo_ds"></a>

## gbj_appthermo_ds()

#### Description
Constructor creates the class instance object and initiates internal resources.
* It inputs operational parameters for temperature sensors.
* It creates one internal timer for periodical temperature measurement in default interval `5 seconds`.
* The constructor accepts two external handlers for broadcasting temperature measurement and errors, which act as timer handlers.
* The input resolution is set to all sensors on the bus.

#### Syntax
    gbj_appthermo_ds(byte pinBus, byte resolution, Handler *handlerData, Handler *handlerErr)

#### Parameters

* **pinBus**: Number of GPIO pin of the microcontroller managing one-wire bus.
  * *Valid values*: positive integers 0 ~ 255
  * *Default value*: none


* **resolution**: Number of bits for measurement resolution.
  * *Valid values*: positive integers 9 ~ 12
  * *Default value*: none


* **handlerData**: Pointer to a function within a sketch that receives no parameters and returns no value, and is called within every internal timer run at successful temperature measurement.
  * *Valid values*: system address range
  * *Default value*: 0


* **handlerErr**: Pointer to a function within a sketch that receives no parameters and returns no value, and is called within every internal timer run at failed temperature measurement.
  * *Valid values*: system address range
  * *Default value*: 0

#### Returns
Object performing temperature measurement.

#### See also
[Handler()](#handler)

[Back to interface](#interface)


<a id="run"></a>

## run()

#### Description
The execution method as the implementation of the virtual method from parent class, which should be called frequently, usually in the loop function of a sketch.
* The method executes conversion for all sensors on the bus at once.
* The final temperature is calculated as an average (mean) of temperature values measured by all successful sensors.
* The handlers is called at every run, if it is declared in the constructor.

#### See also
[Handler()](#handler)

[gbj_appthermo_ds()](#gbj_appthermo_ds)

[Back to interface](#interface)


<a id="period"></a>

## setPeriod(), getPeriod()

#### Description
The methods are just straitforward implementation of the virual methods from the parent class.

[Back to interface](#interface)


<a id="getSensors"></a>

## getSensors()

#### Description
The method returns number of active temperature sensors DS18B20 on the one-wire bus.

#### Syntax
    byte getSensors()

#### Parameters
None

#### Returns
Number of active sensors on the bus.

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

#### See also
[isMeasured()](#isMeasured)

[Back to interface](#interface)


<a id="getCachePtr"></a>

## getCachePtr()

#### Description
The method provides pointer to the internal sensors cache, which is an array of type [Temperatures](#Temperatures) and size of number of active temperature sensors on the one-wire bus published by getter [getSensors](#getSensors).
* The method puts the identifier and the current temperature of all active sensors on the bus in the cache regardless of those with failed conversion (measurement). Such sensors have initial temperature in the cache instead.
* The method calculates average temperature in either case from all active sensors on the bus ignoring sensors with failed conversion.

#### Syntax
    Temperatures *getCachePtr()

#### Parameters
None

#### Returns
Pointer to the internal cache.

#### See also
[Temperatures](#Temperatures)

[Back to interface](#interface)


<a id="isMeasured"></a>

## isMeasured()

#### Description
The method returns a flag whether the recent temperature measurement has been successful as a whole.
* In case of fail measurement it is not worth to read final temperature by method [getTemperature](#getTemperature).

#### Syntax
    bool isMeasured()

#### Parameters
None

#### Returns
Flag about temperature measurement success.

[Back to interface](#interface)
