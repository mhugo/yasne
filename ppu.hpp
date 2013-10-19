#include "SDL.h"
#include "bus_device.hpp"

class PPU : public BusDevice
{
 public:
    static const int PPUCtrl =   0;
    static const int PPUMask =   1;
    static const int PPUStatus = 2;
    static const int OAMAddr =   3;
    static const int OAMData =   4;
    static const int PPUScroll = 5;
    static const int PPUAddr =   6;
    static const int PPUData =   7;

    PPU();
    virtual ~PPU();

    uint8_t read( uint16_t addr ) const;
    void write( uint16_t addr, uint8_t val );

    void frame();
 private:
    // 8 registers
    uint8_t regs[8];

    SDL_Window* win_;
    SDL_Renderer* renderer_;
};
