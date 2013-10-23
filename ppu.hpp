#include <vector>
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

    // called on each clock tick
    void tick();
    // called on each frame
    void frame();

    std::vector<uint8_t>& memory() { return mem_; }

    // current tick wihtin the scanline
    int ticks() const { return ticks_; }
    // current scanline
    int scanline() const { return scanline_; }

    void print_context();

 private:
    // 8 registers
    uint8_t regs[8];

    SDL_Window* win_;
    SDL_Renderer* renderer_;

    std::vector<uint8_t> mem_;
    std::vector<uint8_t> screen_;

    int ticks_;
    int scanline_;

    union Status {
        struct
        {
            uint8_t last : 5;
            uint8_t sprite_overflow : 1;
            uint8_t sprite0_hit : 1;
            uint8_t vblank : 1;
        } bits;
        uint8_t raw;
    };
    mutable Status status_;

    // controller register
    //    7654 3210
    //    |||| ||||
    //    |||| ||++- Base nametable address
    //    |||| ||    (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
    //    |||| |+--- VRAM address increment per CPU read/write of PPUDATA
    //    |||| |     (0: add 1, going across; 1: add 32, going down)
    //    |||| +---- Sprite pattern table address for 8x8 sprites
    //    ||||       (0: $0000; 1: $1000; ignored in 8x16 mode)
    //    |||+------ Background pattern table address (0: $0000; 1: $1000)
    //    ||+------- Sprite size (0: 8x8; 1: 8x16)
    //    |+-------- PPU master/slave select
    //    |          (0: read backdrop from EXT pins; 1: output color on EXT pins)
    //    +--------- Generate an NMI at the start of the
    //    vertical blanking interval (0: off; 1: on)
    union Controller {
        struct
        {
            uint8_t nametable          : 2;
            uint8_t vram_increment     : 1;
            uint8_t sprite_pattern     : 1;
            uint8_t background_pattern : 1;
            uint8_t sprite_size        : 1;
            uint8_t ext                : 1;
            uint8_t nmi                : 1;
        } bits;
        uint8_t raw;
    };
    Controller ctrl_;

    // 76543210
    // ||||||||
    // |||||||+- Grayscale (0: normal color; 1: produce a monochrome display)
    // ||||||+-- 1: Show background in leftmost 8 pixels of screen; 0: Hide
    // |||||+--- 1: Show sprites in leftmost 8 pixels of screen; 0: Hide
    // ||||+---- 1: Show background
    // |||+----- 1: Show sprites
    // ||+------ Intensify reds (and darken other colors)
    // |+------- Intensify greens (and darken other colors)
    // +-------- Intensify blues (and darken other colors)
    union Mask {
        struct
        {
            uint8_t grayscale                : 1;
            uint8_t show_leftmost_background : 1;
            uint8_t show_leftmost_sprites    : 1;
            uint8_t show_background          : 1;
            uint8_t show_sprites             : 1;
            uint8_t intensity_reds           : 1;
            uint8_t intensify_greens         : 1;
            uint8_t intensify_blues          : 1;
        } bits;
        uint8_t raw;
    };
    Mask mask_;

    union Scroll {
        struct
        {
            uint8_t x;
            uint8_t y;
        } bits;
        uint16_t raw;
    };
    Scroll scroll_;

    mutable uint16_t ppuaddr;
};