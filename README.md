<a id="library"></a>
# gbj\_appthermo_ds
This is an application library, wich is used usually as a project library for particular PlatformIO project. The library utilizes all active temperature sensors `DS18B20` on one-wire bus for measuring the ambient temperature.

- Library specifies (inherits from) the system `gbj_appbase` library.
- It utilizes error handling from the parent class.
- It averages temperatures from all sensors as an final temperature.


<a id="dependency"></a>
## Dependency

- **gbj\_appbase**: Parent library for all application libraries loaded from the file `gbj_appbase.h`.
- **gbj\_timer**: Library for executing internal timer within an instance object loaded from the file `gbj_timer.h`.
- **gbj\_serial\_debug**: Auxilliary library for debug serial output loaded from the file `gbj_serial_debug.h`. It enables to exclude serial outputs from final compilation.
- **gbj\_ds18b20**: Library for processing temperature sensors `DS18B20` loaded from the file `gbj_ds18b20.h`.

#### Particle platform
- **Particle.h**: Includes alternative (C++) data type definitions.

#### Arduino platform
- **Arduino.h**: Main include file for the Arduino SDK.
- **inttypes.h**: Integer type conversions. This header file includes the exact-width integer definitions and extends them with additional facilities provided by the implementation.

#### Espressif platform
- **Arduino.h**: Main include file for the Arduino platform.


<a id="constants"></a>
## Constants

- **gbj\_appthermo_ds::VERSION**: Name and semantic version of the library.

Other constants and enumerations are inherited from the parent library.


<a id="interface"></a>
## Interface

- [gbj_appthermo_ds()](#gbj_appthermo_ds)
- [run()](#run)
- [setPeriod()](#setPeriod)
- [getPeriod()](#getPeriod)
- [getSensors()](#getSensors)
- [getTemperature()](#getTemperature)
- [isMeasured()](#isMeasured)


<a id="gbj_appthermo_ds"></a>
## gbj_appthermo_ds()

#### Description
Constructor creates the class instance object and initiates internal resources.
- It inputs operational parameters for temperature sensors.
- It creates one internal timer without a timer handler for periodical temperature measurement in default interval `5 seconds`.
- The input resolution is set to all sensors on the bus.

#### Syntax
    gbj_appthermo_ds(byte pinBus, byte resolution);

#### Parameters

- **pinBus**: Number of GPIO pin of the microcontroller managing one-wire bus.
  - *Valid values*: positive integers 0 ~ 255
  - *Default value*: none


- **resolution**: Number of bits for measurement resolution.
  - *Valid values*: positive integers 9 ~ 12
  - *Default value*: none

#### Returns
Object performing temperature measurement.

[Back to interface](#interface)


<a id="run"></a>
## run()

#### Description
The execution method, which should be called frequently, usually in the loop function of a sketch.
- The method executes conversion for all sensors on the bus at once.
- The final temperature is calculated as a mean (average) of temperature values measured by all sensors.

#### Syntax
	void run();

#### Parameters
None

#### Returns
None

#### Example
Calling methods in the sketch loop.
```cpp
gbj_appthermo_ds thermo = gbj_appthermo_ds(4, 10);

void loop()
{
  thermo.run();
}
```

[Back to interface](#interface)


<a id="setPeriod"></a>
## setPeriod()

#### Description
The method sets a new internal timer period as a measurement interval. It allows to dynamically change a frequency of temperature measurement.

#### Syntax
    void setPeriod(unsigned long period)

#### Parameters
* **period**: Duration of a repeating interval in milliseconds.
  * *Valid values*: 0 ~ 2^32 * 1
  * *Default value*: none

#### Returns
None

#### See also
[getPeriod()](#getPeriod)

[Back to interface](#interface)


<a id="getPeriod"></a>
## getPeriod()

#### Description
The method returns current internal timer period, i.e., frequency of the temperature measurement.

#### Syntax
    unsigned long getPeriod()

#### Parameters
None

#### Returns
Current timer period in milliseconds.

#### See also
[setPeriod()](#getPeriod)

[Back to interface](#interface)


<a id="getSensors"></a>
## getSensors()

#### Description
The method returns number of active temperature sensors DS18B20 on the one-wire bus, from which the final temperature has been calculated.

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
The method returns the final temperature from recent measurement regadless it has been successful or not.
- In case of failed recent measurement the final temperature is usually `0`.
- The precision of the final temperature value (number of used fraction digits), but not precision of the measurement itself, depends on the measurement resolution set by [constructor](#gbj_appthermo_ds).

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
The method returns a flag whether the recent temperature measurement has been successful.
- In case of fail measurement it is not worth to read final temperature by method [getTemperature](#getTemperature).

#### Syntax
    bool isMeasured();

#### Parameters
None

#### Returns
Flag about temperature measurement success.

[Back to interface](#interface)
