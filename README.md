# TPS55289_BE Library

Arduino library for the TPS55289 dcdc converter made by BurningElectronics (https://www.youtube.com/@BurningElectronics)

**For more info see:**

- My YT video: https://youtu.be/CVDHFQry238

- The DCDC converter project: https://www.pcbway.com/project/shareproject/NEW_I2C_controllable_buck_boost_converter_V3_0_b1095f66.html

- TPS55289 datasheet: https://www.ti.com/lit/ds/symlink/tps55289.pdf

## Description

The library is an attempt to make the programming of the dcdc converter easier, it allows you to control the voltage, current and many more features


### Constructor

- **TPS55289_BE(const uint8_t address, TwoWire \*wire = &Wire);** Create your DCDC object


```cpp
byte address = 0x74;
TPS55289_BE converter(address);
```


- **bool begin();** Initialization of dcdc converter, returns false if initialization failed
- **bool isConnected();** Returns false if initialization failed


```cpp
  Serial.begin(115200);

  Wire.begin();

  if(!converter.begin()) {
    Serial.println("Failed to connect");
  }
  else Serial.println("Connected!");
```


### Main functions

- **void enable(bool en);** Turns the output of the converter on/off, by default the dcdc converter is off
- **bool isEnabled();** Returns 0 if converter is off, 1 if on


```cpp
converter.enable(1); //Converter Enabled
```


- **void setVoltageRaw(uint16_t vref);** Set output voltage by writing directly to vref from 0 to 2047
- **void setVoltage(uint16_t v);** Set output voltage in mV
- **uint16_t getVoltageRaw();** Returns raw value of set output voltage from 0 to 2047

To calculate **Vout(mV)** we use:

Vout(vref) = round((45+vref*0.5645)/0.0564)

You can play with it at:
WolframAlpha: https://www.wolframalpha.com/input?i=round%28%2845%2Bx*0.5645%29%2F0.0564%29+where+x+%3D+0


| Vref | Vout | 
|:----:|:----:|
|0     |0.80  |
|10    |0.90  |
|100   |1.80  |
|420   |5.00  |
|1000  |10.81 |
|2047  |21.29 |


To calculate **Vref** we use:

Vref(vout[mV]) = round((x*0.0564-45)/0.5645)

You can play with it at:
WolframAlpha: https://www.wolframalpha.com/input?i=round%28%28x*0.0564-45%29%2F0.5645%29+where+x+%3D+5000


| Vout | Vref | 
|:----:|:----:|
|0.80  |0     |
|1.00  |20    |
|3.30  |250   |
|5.00  |420   |
|9.00  |819   |
|12.00 |1119  |
|20.00 |1919  |


```cpp
converter.setVoltageRaw(420); //Sets output voltage to 5V
delay(1000);
converter.setVoltage(12000); //Sets output voltage to 12V 
```


- **void iLimEnable(bool enCurrent);** Enables/disables the CC feature, by default is enabled
- **setCurrentRaw(uint8_t current);** Set CC current value by writing directly to the register, values can range from 0 to 127
- **void setCurrent(uint16_t current, uint8_t res);** Set CC current in mV, you need to specify the shunt resistor value in mOhm in order to set the current, res default value is 10 (10mOhm shunt)
- **uint8_t getCurrentRaw();** Returns raw current value from 0 to 127

To calculate **Iout(mA)** we use:

Iout(cRaw) = cRaw*0.5/5*1000 for a 5mOhm shunt

You can play with it at:
WolframAlpha: https://www.wolframalpha.com/input?i=x*0.5%2F5*1000+where+x%3D10


Iout(cRaw) = cRaw*0.5/5*1000 for a 10mOhm shunt

You can play with it at:
WolframAlpha: https://www.wolframalpha.com/input?i=x*0.5%2F10*1000+where+x%3D10


The generalized function is:

Iout(cRaw, res[mOhm]) = cRaw*0.5/res*1000

You can play with it at:
WolframAlpha: https://www.wolframalpha.com/input?i=c*0.5%2Fr*1000+where+c%3D10%2C+r%3D10


| cRaw | Iout (5mOhm) | Iout (10mOhm) | 
|:----:|:------------:|:-------------:|
|0     |0             |0              |
|1     |100           |50             |
|10    |1000          |500            |
|50    |5000          |2500           |
|100   |n/a           |5000           |
|127   |n/a           |6350           |


To calculate **cRaw** we use:

cRaw(iout[mA], res[mOhm]) = iout*res/1000*2 

You can play with it at:
WolframAlpha: https://www.wolframalpha.com/input?i=c*r%2F1000*2+where+c+%3D+1000%2C+r+%3D+5


| Iout (5mOhm) | cRaw |
|:------------:|:----:|
|0             |0     |
|100           |1     |
|200           |2     |
|1000          |10    |
|5000          |50    |
|6000          |60    |


| Iout (10mOhm) | cRaw |
|:-------------:|:----:|
|0              |0     |
|50             |1     |
|100            |2     |
|200            |4     |
|1000           |20    |
|1350           |27    |
|5000           |100   |
|6000           |120   |
 

```cpp
converter.iLimEnable(1); //Enable CC feature
delay(10);
converter.setCurrentRaw(10); //Current is set at 500mA (using 10mOhm) or 1A (using 5mOhm)
delay(1000);
converter.setCurrent(1000, 5); //Current is set to 1A, second argument 5 is the shunt value 
```


NOTE1: With a 5mOhm shunt we have 100mA step, with 10mOhm we have 50mA step
NOTE2: We can evaluate minimum step using Iout(1, res[mOhm]) = 0.5/res*1000
NOTE3: We can evaluate maximum Iout using Iout(127, res[mOhm]) = 127*0.5/res*1000


| shunt[mOhm]  | step[mA] | Ioutmax[mA] | 
|:------------:|:--------:|:-----------:|
|5             |100       |n/a          |
|10            |50        |6350         |
|20            |25        |3175         |
|25            |20        |2540         |
|50            |10        |1270         |


### Other Functions

- **void setCDC(uint8_t cdcValue);** Set CDC value from 0 to 7 (0mV/A to 70mV/A for 5mOhm, 0mV/A to 140mV/A for 10mOhm)
- **uint8_t getCDC();** Returns CDC value from 0 to 7


```cpp
converter.setCDC(5); //Set CDC value to 50mV/A (5mOhm) or 100mV/A (10mOhm)
```


-**void setIntFB(uint8_t intfb);** Sets internal feedback ratio from 0 to 3, 3 by default
-**float getIntFB();** Returns internal feedback ratio as a float

The output voltage is not only function of vref, as seen before, but it is also function of the internal feedback ratio intfb, which by default is set to "3" or 0.0564, so it also changes maximum and minimum voltage you can have at the output


|   intfb   | Voutmin | VoutMAX | Vstep |
|:---------:|:-------:|:-------:|:-----:|
|3 (0.0564) |0.80     |21.29    |10mV   |
|2 (0.0752) |0.60     |15.96    |7.5mV  |
|1 (0.1128) |0.40     |10.64    |5mV    |
|0 (0.2256) |0.20     |5.32     |2.5mV  |


To give a better idea this is vout in respect to intfb and vref:


|   intfb   | vref = 250 | vref = 420 | vref = 819 |
|:---------:|:----------:|:----------:|:----------:|
|3 (0.0564) |3.30        |5.00        |9.00        |
|2 (0.0752) |2.48        |3.75        |6.75        |
|1 (0.1128) |1.65        |2.50        |4.50        |
|0 (0.2256) |0.83        |1.25        |2.25        |


To calculate **Vout(mV)** we can use the generalized function:

Vout(vref, intfb) = round((45+vref*0.5645)/intfb)

You can play with it at:
WolframAlpha: https://www.wolframalpha.com/input?i=round%28%2845%2Bv*0.5645%29%2Ff%29+where+v+%3D+420%2C+f+%3D+0.2256


```cpp
converter.setVoltageRaw(420); //Sets output voltage to 5V
delay(1000);
converter.setIntFB(2); //The output voltage is now 3.75V
```


- **void discharge(bool dsg);** Enable/Disable Vout discharge by an internal 100mA current sink


```cpp
converter.discharge(1); //Enable discharge
```


