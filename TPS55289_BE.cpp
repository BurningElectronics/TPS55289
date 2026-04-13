//    FILE: TPS55289_BE.cpp
//  AUTHOR: BurningElectronics
// VERSION: 1.0
//    DATE: 29/03/2026
//     URL: --
//  Read the datasheet for the details: https://www.ti.com/lit/ds/symlink/tps55289.pdf


#include "TPS55289_BE.h"


//Registers
#define REF_LSB 0
#define REF_MSB 1
#define IOUT_LIMIT 2
#define VOUT_SR 3
#define VOUT_FS 4
#define CDC 5
#define MODE 6
#define STATUS 7

//Reset values
#define RST_REF_LSB B00000001
#define RST_REF_MSB B10100100
#define RST_IOUT_LIMIT B11100100
#define RST_VOUT_SR B00000001
#define RST_VOUT_FS B00000011
#define RST_CDC B11100000
#define RST_MODE B00100000
#define RST_STATUS B00000011

//INTFB values
#define INTFB_0 0.2256F
#define INTFB_1 0.1128F
#define INTFB_2 0.0752F
#define INTFB_3 0.0564F

//VREF LSB
#define VREF_STEP 0.5645F


//CONSTRUCTOR
TPS55289_BE::TPS55289_BE(const uint8_t address, TwoWire *wire)
{
  _address = address;
  _wire = wire;
  _ref = RST_REF_MSB + RST_REF_LSB;
  _current = RST_IOUT_LIMIT;
  _intfb = RST_VOUT_FS;
  _cdc = 0;
  _mode = RST_MODE;
}


/**
 * @brief Initialization of TPS55289 IC
 * @return false if initialization failed
 */
bool TPS55289_BE::begin()
{
  if (!isConnected()) return false;
  return true;
}

/**
 * @brief Initialization of TPS55289 IC
 * @return false if initialization failed
 */
bool TPS55289_BE::isConnected()
{
  _wire->beginTransmission(_address);
  return ( _wire->endTransmission() == 0);
}

/**
 * @brief Output Enable of TPS55289 IC
 * @param en Enable status 0 - Off, 1 - On
 */
void TPS55289_BE::enable(bool en)
{
  uint8_t b = _mode;
  bitWrite(b, 7, en);
  _wire->beginTransmission(_address);
  _wire->write(MODE);
  _wire->write(b);
  _wire->endTransmission();

  _mode = b;

}

/**
 * @brief Set output voltage using vref value, see https://www.ti.com/lit/ds/symlink/tps55289.pdf#page=26
 * @param vref Voltage reference value (0 to 2047, ex. 5V = 420)
 */
void TPS55289_BE::setVoltageRaw(uint16_t vref)
{

  uint16_t b = constrain(vref, 0, 2047);
  uint8_t b_lsb = b & 255;
  uint8_t b_msb = (b & 1792) >> 8;

  _wire->beginTransmission(_address);
  _wire->write(REF_LSB);
  _wire->write(b_lsb);
  _wire->endTransmission();
  delay(10);
  _wire->beginTransmission(_address);
  _wire->write(REF_MSB);
  _wire->write(b_msb);
  _wire->endTransmission();

  _ref = b;

}

/**
 * @brief Set output voltage using target voltage value in mV
 * @param v Voltage in mV (ex. 12.67V = 12670)
 */
void TPS55289_BE::setVoltage(uint16_t v)
{

  if(v*getIntFB()-45 > 0) v = round((v*getIntFB()-45.0)/VREF_STEP);
  else v = 0;
  
  setVoltageRaw(v);

}

/**
 * @brief Set internal FB ratio, see https://www.ti.com/lit/ds/symlink/tps55289.pdf#page=29
 * @param intfb value of internal feedback, 0 = 0.2256, 1 = 0.1128, 2 = 0.0752, 3 = 0.0564
 */
void TPS55289_BE::setIntFB(uint8_t intfb)
{

  intfb = constrain(intfb, 0, 3);

  _wire->beginTransmission(_address);
  _wire->write(VOUT_FS);
  _wire->write(intfb);
  _wire->endTransmission();

  _intfb = intfb;
}

/**
 * @brief Enables Control Current mode, see https://www.ti.com/lit/ds/symlink/tps55289.pdf#page=27
 * @param enCurrent Enable, 0 - Off, 1 - On
 */
void TPS55289_BE::iLimEnable(bool enCurrent) {

  uint8_t b = _current;
  bitWrite(b, 7, enCurrent);

  _wire->beginTransmission(_address);
  _wire->write(IOUT_LIMIT);
  _wire->write(b);
  _wire->endTransmission();

  _current = b;

}

/**
 * @brief Set CC current using raw current value, the actual value depends on the shunt resistor, see https://www.ti.com/lit/ds/symlink/tps55289.pdf#page=27
 * @param current Current value (0 to 127)
 */
void TPS55289_BE::setCurrentRaw(uint8_t current) {

  current = constrain(current, 0, 127);

  current = (_current & B10000000) + current;

  _wire->beginTransmission(_address);
  _wire->write(IOUT_LIMIT);
  _wire->write(current);
  _wire->endTransmission();

  _current = current;

}

/**
 * @brief Set CC current using current value in mA
 * @param current Current value in mA
 * @param res Shunt value in mOhm, default value 10
 */
void TPS55289_BE::setCurrent(uint16_t current, uint8_t res = 10) {

  uint8_t b = (current*res)/1000*2;
  setCurrentRaw(b);


}

/**
 * @brief Set Cable Drop Compensation value, depends on shunt resistor, see https://www.ti.com/lit/ds/symlink/tps55289.pdf#page=30
 * @param cdc CDC value, 0 = 0mV/A (Off), 1 = 10mV/A ... 7 = 70mV/A for 5mOhm shunt, multiply by 2 if you're using 10mOhm
 */
void TPS55289_BE::setCDC(uint8_t cdcValue) {

  cdcValue = constrain(cdcValue, 0, 7);
  _wire->beginTransmission(_address);
  _wire->write(CDC);
  _wire->write(RST_CDC + cdcValue);
  _wire->endTransmission();

  _cdc = cdcValue;

}

/**
 * @brief Enable output discharge, see https://www.ti.com/lit/ds/symlink/tps55289.pdf#page=31
 * @param dsg Enable discharge, 0 - Off, 1 - On
 */
void TPS55289_BE::discharge(bool dsg) {

  uint8_t b = _mode;
  bitWrite(b, 4, dsg);
  _wire->beginTransmission(_address);
  _wire->write(MODE);
  _wire->write(b);
  _wire->endTransmission();

  _mode = b;

}

/**
 * @brief Checks if Output Voltage is On/Off 
 * @return 0 - Off, 1 - On
 */
bool TPS55289_BE::isEnabled() {
  return _mode & B10000000;
}

/**
 * @brief Gets the cdc value 
 * @return CDC value (0 to 7)
 */
uint8_t TPS55289_BE::getCDC() {

  return _cdc;

}

/**
 * @brief Gets the Raw value of the Current set for CC
 * @return Raw Current Value (0 to 127)
 */
uint8_t TPS55289_BE::getCurrentRaw() {

  return _current & B01111111;

}

/**
 * @brief Gets the Raw value of set output voltage
 * @return Raw Current Value (0 to 2047)
 */
uint16_t TPS55289_BE::getVoltageRaw()
{

  return _ref;

}

/**
 * @brief Gets Internal Feedback value as a Float
 * @return Internal Feedback value
 */
float TPS55289_BE::getIntFB()
{
  uint8_t intfb = _intfb & B00000011;

  switch (intfb) {
    case 0:
        return INTFB_0;
        break;
    case 1:
        return INTFB_1;
        break;
    case 2:
        return INTFB_2;
        break; 
    case 3:
        return INTFB_3;
        break;
  }
}
