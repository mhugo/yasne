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
    int ticks() const { return tick_; }
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

    void dump_mem( const std::string& out_file ) const;

    void render();

 private:
    // 8 registers
    uint8_t regs[8];

    SDL_Window* win_;
    SDL_Renderer* renderer_;

    std::vector<uint8_t> mem_;
    std::vector<uint8_t> screen_;

    int tick_;
    int scanline_;

public:
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
        void print( std::ostream& ostr ) const {
            ostr << "(last:" << int(bits.last)
                 << " sp_overflow:" << int(bits.sprite_overflow)
                 << " sp0_hit:" << int(bits.sprite0_hit)
                 << " vblank:" << int(bits.vblank)
                 << ")";
        }
    };
private:
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
public:
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
        void print( std::ostream& ostr ) const {
            ostr << "(nametable:" << int(bits.nametable)
                 << " vram_inc:" << int(bits.vram_increment)
                 << " sp_pattern:" << int(bits.sprite_pattern)
                 << " bg_pattern:" << int(bits.background_pattern)
                 << " sp_8x16:" << int(bits.sprites_are_8x16)
                 << " ext:" << int(bits.ext)
                 << " nmi:" << int(bits.nmi)
                 << ")";
        }
    };
private:
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
public:
    union Mask {
        struct
        {
            uint8_t grayscale                : 1;
            uint8_t show_leftmost_background : 1;
            uint8_t show_leftmost_sprites    : 1;
            uint8_t show_background          : 1;
            uint8_t show_sprites             : 1;
            uint8_t intensify_reds           : 1;
            uint8_t intensify_greens         : 1;
            uint8_t intensify_blues          : 1;
        } bits;
        uint8_t raw;
        Mask() : raw(0) {}
        void print( std::ostream& ostr ) const {
            ostr << "(grayscale: " << int(bits.grayscale)
                 << " show_leftmost_bg: " << int(bits.show_leftmost_background)
                 << " show_leftmost_sp: " << int(bits.show_leftmost_sprites)
                 << " show_bg: " << int(bits.show_background)
                 << " show_sp: " << int(bits.show_sprites)
                 << " int_r: " << int(bits.intensify_reds)
                 << " int_g: " << int(bits.intensify_greens)
                 << " int_b: " << int(bits.intensify_blues)
                 << ")";
        }
    };
private:
    Mask mask_;

    // yyy NN YY YYY XXXXX
    // ||| || || ||| +++++-- coarse X scroll
    // ||| || ++ +++-------- coarse Y scroll
    // ||| ++--- ----------- nametable select
    // +++------ ----------- fine Y scroll
public:
    union Address
    {
        struct {
            // scroll
            uint16_t coarse_x  : 5;
            uint16_t coarse_y  : 5;
            uint16_t nametable : 2;
            uint16_t fine_y    : 3;
        } bits;
        uint16_t raw;
        Address( uint16_t v = 0 ) : raw(v) {}

        void print( std::ostream& ostr ) const {
            ostr << "(N:" << bits.nametable
                 << " X:" << bits.coarse_x
                 << " Y:" << bits.coarse_y
                 << " y:" << bits.fine_y
                 << ")";
        }
    };

private:
    // vram address
    mutable Address ppuaddr;
    // temporary vram address
    mutable Address ppuaddr_t;
    // fine X scroll
    mutable uint8_t fine_x_;
    // toggle high/low address (shared by ppuaddr and ppuscroll)
    mutable int write_low_addr_;

    mutable uint16_t bg_shift_l, bg_shift_h;

    CPU* cpu_;

    // Object Attribute Memory (sprites)
    uint8_t oam_[64*4];
    uint8_t oam2_[8*4];
    uint8_t oam_addr_;

    // sprites for the next scanline
    uint8_t next_sprites_[8][2];
    // X coordinate for sprites of the next scanline
    int sprite_x_[8];
    int n_next_sprites_;

    // temporary screen arrays
    // each cell contains a pixel value
    std::vector<uint8_t> nametable_;

    // SDL texture used for screen framebuffer
    SDL_Texture *screen_tex_;
};

std::ostream& operator<<( std::ostream& ostr, const PPU::Status& adr );
std::ostream& operator<<( std::ostream& ostr, const PPU::Controller& adr );
std::ostream& operator<<( std::ostream& ostr, const PPU::Mask& adr );
std::ostream& operator<<( std::ostream& ostr, const PPU::Address& adr );
