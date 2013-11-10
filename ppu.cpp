#include <stdexcept>
#include <iostream>
#include "ppu.hpp"

PPU::PPU() : mem_( 0x4000 ),
             screen_( 240*256 ),
             ticks_(0),
             scanline_(0)
{
    // init SDL
    SDL_Init( SDL_INIT_VIDEO );

    win_ = SDL_CreateWindow( "Test", 0, 0, 256, 240, 0 );
    if ( ! win_ ) {
        throw std::runtime_error( "create window" );
    }
    renderer_ = SDL_CreateRenderer( win_, -1, 0 );
    if ( ! renderer_ ) {
        throw std::runtime_error( "create renderer" );
    }
}

PPU::~PPU()
{
    SDL_Quit();
}

void PPU::print_context()
{
#if 0
    //
    for ( int k = 0; k < 256; k++ ) {
        printf("%d\n", k);
        for ( int i = 0; i < 8; i++ ) {
            uint8_t spA = mem_[k*16+i+0];
            uint8_t spB = mem_[k*16+i+1];
            for ( int j = 7; j >= 0; j-- ) {
                char p = ((spA & (1 << j)) >> j) | (((spB & (1 << j)) >> j) << 1);
                printf("%c", ".XOM"[p]);
            }
            printf("\n");
        }
    }
#endif
    printf("status: %04X ctrl: %04X mask: %04X\n", status_.raw, ctrl_.raw, mask_.raw );

    uint16_t nametable = (ctrl_.bits.nametable << 10) | 0x2000;
    printf("%04X\n", nametable);

    // read nametable
    for ( int x = 0; x < 32; x++ ) {
        for ( int y = 0; y < 30; y++ ) {
            uint8_t idx = mem_[ nametable + y*32+x ];
            printf("%02x", idx);
        }
        printf("\n");
    }
}

void PPU::get_pattern( int idx, uint8_t* ptr, int row_length )
{
    for ( int i = 0; i < 8; i++ ) {
        uint8_t spA = mem_[idx*16+i+0];
        uint8_t spB = mem_[idx*16+i+1];
        for ( int j = 7; j >= 0; j-- ) {
            uint8_t p = ((spA & (1 << j)) >> j) | (((spB & (1 << j)) >> j) << 1);
            *ptr++ = p * 64;
        }
        ptr += row_length-8;
    }
}

void PPU::frame()
{
    // browse nametable
    uint16_t nametable = (ctrl_.bits.nametable << 10) | 0x2000;

    // TODO : add scroll

    // read nametable
    for ( int y = 0; y < 30; y++ ) {
        for ( int x = 0; x < 32; x++ ) {
            uint8_t idx = mem_[ nametable + y*32+x ];
            get_pattern( idx, &screen_[0] + y*8*8*32 + x*8, 32*8 );
        }
    }

    SDL_Surface* surf = SDL_CreateRGBSurfaceFrom( &screen_[0],
                                                  32*8,
                                                  30*8,
                                                  8,
                                                  32*8,
                                                  0,
                                                  0,
                                                  0,
                                                  0);
    SDL_Palette *palette = surf->format->palette;
    for ( int i = 0; i < palette->ncolors; i++ ) {
        palette->colors[i].r = i;
        palette->colors[i].g = i;
        palette->colors[i].b = 255-i;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface( renderer_, surf );

    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, tex, NULL, NULL);
    SDL_RenderPresent(renderer_);
    SDL_FreeSurface( surf );
}

void PPU::tick()
{
    if ( (scanline_ == 0) && (ticks_ == 0) ) {
        frame();
    }

    ticks_ = (ticks_ + 1) % 341;
    if (ticks_ == 0 ) {
        if ( scanline_ == 0 ) {
            SDL_Event e;
            if ( SDL_PollEvent(&e) ) {
                if ( e.type == SDL_QUIT ) {
                    exit(0);
                }
            }
        }
        scanline_ = (scanline_ + 1) % 262;
    }

    if ( (ticks_ == 1) && (scanline_ == 241) ) {
        status_.bits.vblank = 1;
    }
    if ( (ticks_ == 1) && (scanline_ == 261) ) {
        status_.bits.vblank = 0;
    }
}

uint8_t PPU::read( uint16_t addr ) const
{
    if ( addr == PPUStatus ) {
        uint8_t c = status_.raw;
        status_.bits.vblank = 0;
        return c;
    }
    else if ( addr == PPUData ) {
        uint8_t b = mem_[ ppuaddr % mem_.size() ];
        ppuaddr = ppuaddr + (ctrl_.bits.vram_increment ? 32 : 1 );
        return b;
    }
    printf("read %04X\n", addr);
    //    std::cin.get();
    return 0;
}

void PPU::write( uint16_t addr, uint8_t val )
{
    if ( addr == PPUCtrl ) {
        ctrl_.raw = val;
    }
    else if ( addr == PPUMask ) {
        mask_.raw = val;
    }
    else if ( addr == PPUAddr ) {
        ppuaddr = (ppuaddr << 8) | val;
    }
    else if ( addr == PPUData ) {
        printf("PPU data write to %04X <= %02X\n", ppuaddr, val);
        mem_[ ppuaddr % mem_.size() ] = val;
        ppuaddr = ppuaddr + (ctrl_.bits.vram_increment ? 32 : 1 );
    }
    else if ( addr == PPUScroll ) {
        scroll_.raw = ( scroll_.raw << 8) | val;
    }
    else {
        printf("write %04X <= %02X\n", addr, val);
        //        std::cin.get();
    }
}
