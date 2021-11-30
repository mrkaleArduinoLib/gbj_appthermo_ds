<a id="library"></a>

# gbj\_appthermo_ds
This is an application library, which is used usually as a project library for particular PlatformIO project. The library utilizes all active temperature sensors `DS18B20` on one-wire bus for measuring the ambient temperature.

* Library specifies (inherits from) the system `gbj_appbase` library.
* It utilizes error handling from the parent class.
* It averages temperatures from all sensors as an final temperature.


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
* [begin()](#begin)
* [run()](#run)
* [setPeriod()](#period)
* [getPeriod()](#period)
* [getSensors()](#getSensors)
* [getTemperature()](#getTemperature)
* [isMeasured()](#isMeasured)


## Custom data types

* [Temperatures](#temperatures)


<a id="temperatures"></a>

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

#### See also
[Back to interface](#interface)


<a id="gbj_appthermo_ds"></a>

## gbj_appthermo_ds()

#### Description
Constructor creates the class instance object and initiates internal resources.
* It inputs operational parameters for temperature sensors.
* It creates one internal timer without a timer handler for periodical temperature measurement in default interval `5 seconds`.
* The input resolution is set to all sensors on the bus.

#### Syntax
    gbj_appthermo_ds(byte pinBus, byte resolution)

#### Parameters

* **pinBus**: Number of GPIO pin of the microcontroller managing one-wire bus.
  * *Valid values*: positive integers 0 ~ 255
  * *Default value*: none


* **resolution**: Number of bits for measurement resolution.
  * *Valid values*: positive integers 9 ~ 12
  * *Default value*: none

#### Returns
Object performing temperature measurement.

[Back to interface](#interface)


<a id="begin"></a>

## begin()

#### Description
The method exports last known temperature from sensors, which identifiers are present in the external array referenced as input argument.
* The method ignores a sensor's temperature that is equal to initial temperature, which is sign of failed conversion (measurement).
* The method calculates average temperature in either case from all active sensors on the bus ignoring sensors after failed conversion.
* The method is useful when multiple sensors measure in various spaces or environments and the average temperature might be useless.
* The temperature is exported only for sensors that are active on the bus and have identifiers present in the input array.
* The method does not need to be called in a sketch, if none of sensors are individually observed.

#### Syntax
    begin(Temperatures *listSensors, size_t listCount)

#### Parameters

* **listSensors**: Pointer to an external array of corresponding structure type with sensor identifier in the field `id` of items. The array acts as a list of individually observed sensors. It has to be populated with identifier of desired sensor externally before calling the method.
  * *Valid values*: system address space
  * *Default value*: 0


* **listCount**: Number of individually observed sensors (records, rows, structures) in the list. The real number of updated sensor records can be lower due to ignoring sensors after failed conversion and not present or active on the one-wire bus.
  * *Valid values*: non-negative integer
  * *Default value*: 0

#### Returns
None

#### See also
[Temperatures](#temperatures)

[Back to interface](#interface)


<a id="run"></a>

## run()

#### Description
The execution method as the implementation of the virtual method from parent class, which should be called frequently, usually in the loop function of a sketch.
* The method executes conversion for all sensors on the bus at once.
* The final temperature is calculated as an average (mean) of temperature values measured by all successful sensors.

[Back to interface](#interface)


<a id="period"></a>

## setPeriod(), getPeriod()

#### Description
The methods are just straitforward implementation of the virual methods from the parent class.

[Back to interface](#interface)


<a id="getSensors"></a>

## getSensors()

#### Description
The method returns number of active temperature sensors DS18B20 on the one-wire bus, from which the final average temperature has been calculated.

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
The method returns final (the recent known) temperature regadless the recent measurement has been successful or not.
* In case of failed recent measurement at all sensors on the bus the final temperature is `0`.
* The precision of the final temperature value (number of used fraction digits), but not precision of the measurement itself, depends on the measurement resolution set by [constructor](#gbj_appthermo_ds).

#### Syntax
    float getTemperature()

#### Parameters
None

#### Returns
The final temperature in degrees of Celsius averaged from all active sensors.

#### See also
[isMeasured()](#isMeasured)

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
