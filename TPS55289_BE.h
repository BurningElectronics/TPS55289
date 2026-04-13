#pragma once
//    FILE: TPS55289_BE.h
//  AUTHOR: BurningElectronics
// VERSION: 1.0
//    DATE: 29/03/2026
//     URL: --
//  Read the datasheet for the details: https://www.ti.com/lit/ds/symlink/tps55289.pdf

#include "Arduino.h"
#include "Wire.h"

class TPS55289_BE
{
  public:

   //Address can be 0x74 or 0x75   
    explicit TPS55289_BE(const uint8_t address, TwoWire *wire = &Wire);

    bool begin();
    bool isConnected();

    void enable(bool en);
    void setVoltageRaw(uint16_t vref);
    void setVoltage(uint16_t v);
    void setIntFB(uint8_t intfb);
    void iLimEnable(bool enCurrent);
    void setCurrentRaw(uint8_t current);
    void setCurrent(uint16_t current, uint8_t res);
    void setCDC(uint8_t cdcValue);
    void discharge(bool dsg);

    bool isEnabled();
    uint16_t getVoltageRaw();
    uint8_t getCurrentRaw();
    float getIntFB();
    uint8_t getCDC();
  
  private:

    uint8_t  _address;
    uint16_t _ref;
    uint8_t _current;
    uint8_t _cdc;
    uint8_t  _mode;
    uint8_t  _intfb;
    TwoWire *_wire;

    


};

// --- END OF FILE ---
