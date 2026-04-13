// This code uses the Serial monitor as an input to set the Current for the cc mode of the DCDC converter


#include "TPS55289_BE.h"

byte address = 0x74;

bool mode = 0; //if 0: current is adjusted using raw value (0 to 127), if 1: current is adjusted in mA (50mA/100mA step depending on sense resistor)

TPS55289_BE converter(address);

void setup() {
  Serial.begin(115200);

  Wire.begin();

  if(!converter.begin()) {
    Serial.println("Failed to connect");
  }
  else Serial.println("Connected!");

  converter.enable(1);
  converter.iLimEnable(1); //By default the current limit is enabled, but we need to still call the function in case it was disabled previously
  
  converter.setCurrentRaw(10); //We set current at 500mA (using 10mOhm) or 1A (using 5mOhm)

}

void loop() {
  if(Serial.available() > 0) {

    int val = Serial.readString().toInt();
    if(!mode) converter.setCurrentRaw(val);
    else converter.setCurrent(val, 5); //5 is the value in mOhm of your shunt resistor, change it if needed
    Serial.print("Raw value:");
    Serial.println(converter.getCurrentRaw());
    
  }

}
