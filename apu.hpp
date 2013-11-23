#include <vector>
#include "SDL.h"
#include "bus_device.hpp"
#include "controller.hpp"

class CPU;

class APU : public BusDevice
{
 public:
    APU( CPU* cpu, Controller* ctrl );
    virtual ~APU();

    uint8_t read( uint16_t addr ) const;
    void write( uint16_t addr, uint8_t val );

 private:
    CPU* cpu_;
    Controller* controller_;
};
