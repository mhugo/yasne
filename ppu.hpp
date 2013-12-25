#include <vector>
#include "SDL.h"
#include "bus_device.hpp"

class CPU;

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

    PPU( CPU* cpu );
    ~PPU();

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

    //
    // fills a 8x8 bytes pattern
    // idx: pattern index
    // ptr: memory address to fill
    // row_length: length of a memory row
    void get_pattern( uint16_t baseAddr, int idx, uint8_t* ptr, int row_length, int paletteNum );

    void get_sprite( int idx, uint8_t* ptr );

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

    // status register
    // 7654 3210
    // |||| ||||
    // |||+-++++- Least significant bits previously written into a PPU register
    // |||        (due to register not being updated for this address)
    // ||+------- Sprite overflow. The intent was for this flag to be set
    // ||         whenever more than eight sprites appear on a scanline, but a
    // ||         hardware bug causes the actual behavior to be more complicated
    // ||         and generate false positives as well as false negatives; see
    // ||         PPU sprite evaluation. This flag is set during sprite
    // ||         evaluation and cleared at dot 1 (the second dot) of the
    // ||         pre-render line.
    // |+-------- Sprite 0 Hit.  Set when a nonzero pixel of sprite 0 overlaps
    // |          a nonzero background pixel; cleared at dot 1 of the pre-render
    // |          line.  Used for raster timing.
    // +--------- Vertical blank has started (0: not in VBLANK; 1: in VBLANK).
    //            Set at dot 1 of line 241 (the line *after* the post-render
    //            line); cleared after reading $2002 and at dot 1 of the
    //            pre-render line.
    union Status {
        struct
        {
            uint8_t last : 5;
            uint8_t sprite_overflow : 1;
            uint8_t sprite0_hit : 1;
            uint8_t vblank : 1;
        } bits;
        uint8_t raw;
        Status() : raw(0) {}
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
            uint8_t sprites_are_8x16   : 1;
            uint8_t ext                : 1;
            uint8_t nmi                : 1;
        } bits;
        uint8_t raw;
        Controller() : raw(0) {}
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
        Mask() : raw(0) {}
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
    CPU* cpu_;

    // Object Attribute Memory (sprites)
    uint8_t oam_[256];
    uint8_t oam_addr_;

    // temporary screen arrays
    // each cell contains a pixel value
    std::vector<uint8_t> nametable_;
};
