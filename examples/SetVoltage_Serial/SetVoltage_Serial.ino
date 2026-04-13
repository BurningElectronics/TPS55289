// This code uses the Serial monitor as an input to set the output voltage of the DCDC converter


#include "TPS55289_BE.h"

byte address = 0x74;

bool mode = 0; //if 0: voltage is adjusted using raw value (0 to 2047), if 1: voltage is adjusted in mV

TPS55289_BE converter(address);

void setup() {
  Serial.begin(115200);

  Wire.begin();

  if(!converter.begin()) {
    Serial.println("Failed to connect");
  }
  else Serial.println("Connected!");

  converter.enable(1);
  converter.iLimEnable(0); //By default the current limit is enabled, here I disabled it

}

void loop() {
  if(Serial.available() > 0) {

    int val = Serial.readString().toInt();
    if(!mode) converter.setVoltageRaw(val);
    else converter.setVoltage(val);
    Serial.print("Raw value:");
    Serial.println(converter.getVoltageRaw());
    
  }

}
