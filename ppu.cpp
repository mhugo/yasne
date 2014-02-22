#include <stdexcept>
#include <iostream>
#include "ppu.hpp"
#include "cpu.hpp"

const char NesPalette[][3] = {
    0x7C,0x7C,0x7C,
    0x00,0x00,0xFC,
    0x00,0x00,0xBC,
    0x44,0x28,0xBC,
    0x94,0x00,0x84,
    0xA8,0x00,0x20,
    0xA8,0x10,0x00,
    0x88,0x14,0x00,
    0x50,0x30,0x00,
    0x00,0x78,0x00,
    0x00,0x68,0x00,
    0x00,0x58,0x00,
    0x00,0x40,0x58,
    0x00,0x00,0x00,
    0x00,0x00,0x00,
    0x00,0x00,0x00,
    0xBC,0xBC,0xBC,
    0x00,0x78,0xF8,
    0x00,0x58,0xF8,
    0x68,0x44,0xFC,
    0xD8,0x00,0xCC,
    0xE4,0x00,0x58,
    0xF8,0x38,0x00,
    0xE4,0x5C,0x10,
    0xAC,0x7C,0x00,
    0x00,0xB8,0x00,
    0x00,0xA8,0x00,
    0x00,0xA8,0x44,
    0x00,0x88,0x88,
    0x00,0x00,0x00,
    0x00,0x00,0x00,
    0x00,0x00,0x00,
    0xF8,0xF8,0xF8,
    0x3C,0xBC,0xFC,
    0x68,0x88,0xFC,
    0x98,0x78,0xF8,
    0xF8,0x78,0xF8,
    0xF8,0x58,0x98,
    0xF8,0x78,0x58,
    0xFC,0xA0,0x44,
    0xF8,0xB8,0x00,
    0xB8,0xF8,0x18,
    0x58,0xD8,0x54,
    0x58,0xF8,0x98,
    0x00,0xE8,0xD8,
    0x78,0x78,0x78,
    0x00,0x00,0x00,
    0x00,0x00,0x00,
    0xFC,0xFC,0xFC,
    0xA4,0xE4,0xFC,
    0xB8,0xB8,0xF8,
    0xD8,0xB8,0xF8,
    0xF8,0xB8,0xF8,
    0xF8,0xA4,0xC0,
    0xF0,0xD0,0xB0,
    0xFC,0xE0,0xA8,
    0xF8,0xD8,0x78,
    0xD8,0xF8,0x78,
    0xB8,0xF8,0xB8,
    0xB8,0xF8,0xD8,
    0x00,0xFC,0xFC,
    0xF8,0xD8,0xF8,
    0x00,0x00,0x00,
    0x00,0x00,0x00
};

PPU::PPU( CPU* cpu ) : mem_( 0x4000 ),
                       screen_( 240*256 ),
                       tick_(0),
                       scanline_(0),
                       ppuaddr( 0 ),
                       cpu_( cpu ),
                       oam_addr_( 0 ),
                       n_next_sprites_( 0 ),
                       nametable_( 240*256 ),
                       screen_tex_( 0 )
{
    win_ = SDL_CreateWindow( "Test", 0, 0, 512, 480, 0 );
    if ( ! win_ ) {
        throw std::runtime_error( "create window" );
    }
    renderer_ = SDL_CreateRenderer( win_, -1, 0 );
    if ( ! renderer_ ) {
        throw std::runtime_error( "create renderer" );
    }

    screen_tex_ = SDL_CreateTexture( renderer_,
                                     SDL_PIXELFORMAT_RGB24,
                                     SDL_TEXTUREACCESS_STREAMING,
                                     32*8,
                                     30*8 );
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
    printf("ctrl: %02X mask: %02X status: %02X scroll: (%d,%d) tick: %d SL: %d\n",
           ctrl_.raw,
           mask_.raw,
           status_.raw,
           scroll_.bits.x,
           scroll_.bits.y,
           tick_,
           scanline_ );

    uint16_t nametable = (ctrl_.bits.nametable << 10) | 0x2000;
    printf("%04X\n", nametable);

#if 0
    // read nametable
    for ( int x = 0; x < 32; x++ ) {
        for ( int y = 0; y < 30; y++ ) {
            uint8_t idx = mem_[ nametable + y*32+x ];
            printf("%02x", idx);
        }
        printf("\n");
    }
#endif

#if 0
    printf("OAM:\n");
    for ( int i = 0; i < 64; ++i ) {
        printf("%d ", i);
        uint8_t y   = oam_[ i * 4 + 0 ];
        uint8_t x   = oam_[ i * 4 + 3 ];
        uint8_t idx = oam_[ i * 4 + 1 ];
        uint8_t att = oam_[ i * 4 + 2 ];
        printf("(%03d,%03d):%02x[%02x]\n", x, y, idx, att);
    }
#endif
}

void PPU::get_pattern( uint16_t baseAddr, int idx, uint8_t* ptr, int row_length, int paletteNum )
{
    uint8_t palette0 = mem_[0x3F00];
    uint8_t *palette = &mem_[0x3F00 + paletteNum * 4];
    for ( int i = 0; i < 8; i++ ) {
        uint8_t spA = mem_[baseAddr + idx*16+i+0];
        uint8_t spB = mem_[baseAddr + idx*16+i+8];
        for ( int j = 7; j >= 0; j-- ) {
            uint8_t c = ((spA & (1 << j)) >> j) | (((spB & (1 << j)) >> j) << 1);
            if ( c == 0 ) {
                *ptr++ = palette0 & 63;
            }
            else {
                *ptr++ = palette[c] & 63;
            }
        }
        ptr += row_length-8;
    }
}

void PPU::get_sprite( int idx, uint8_t *ptr )
{
    uint16_t baseAddr = ctrl_.bits.sprite_pattern ? 0x1000 : 0;
    for ( int i = 0; i < 8; i++ ) {
        uint8_t spA = mem_[baseAddr + idx*16+i+0];
        uint8_t spB = mem_[baseAddr + idx*16+i+8];
        for ( int j = 7; j >= 0; j-- ) {
            uint8_t c = ((spA & (1 << j)) >> j) | (((spB & (1 << j)) >> j) << 1);
            *ptr++ = c;
        }
    }
}

void PPU::frame()
{
#if 0
    if ( mask_.bits.show_background ) {
        // browse nametable
        uint16_t nametable = (ctrl_.bits.nametable << 10) | 0x2000;

        // TODO : add scroll
        int xs = scroll_.bits.x / 8;

        // read nametable
        // iterate by block of 8x8
        for ( int y = 0; y < 30; y++ ) {
            for ( int x = 0; x < 32; x++ ) {
                // 32x32 pixel block coordinates
                int x32 = (x+xs)/4;
                int y32 = y/4;
                // 16x16 pixel block coordinates
                int x16 = (x+xs)/2;
                int y16 = y/2;
                uint8_t pal = mem_[ nametable + 0x3C0 + x32 + y32 * 8];
                if ( x16&1 ) {
                    if ( y16&1 ) {
                        pal = (pal & 0xC0) >> 6;
                    }
                    else {
                        pal = (pal & 0x0C) >> 2;
                    }
                }
                else {
                    if ( y16&1 ) {
                        pal = (pal & 0x30) >> 4;
                    }
                    else {
                        pal = (pal & 0x03);
                    }
                }
                uint8_t patIdx = mem_[ nametable + y*32+(x+xs) ];
                get_pattern( ctrl_.bits.background_pattern ? 0x1000 : 0,
                             patIdx,
                             &nametable_[0] + y*8*(8*32) + (x+xs)*8,
                             32*8,
                             pal);
            }
        }
    }

    if ( mask_.bits.show_sprites ) {
        // TODO deal with 8x16 sprites
        if ( ! ctrl_.bits.sprites_are_8x16 ) {
            uint16_t baseAddr = ctrl_.bits.sprite_pattern ? 0x1000 : 0;
            uint8_t current_sprite[8*8];
            for ( int i = 0; i < 64; ++i ) {
                uint8_t y   = oam_[ i * 4 + 0 ];
                uint8_t x   = oam_[ i * 4 + 3 ];
                uint8_t idx = oam_[ i * 4 + 1 ];
                uint8_t att = oam_[ i * 4 + 2 ];
                if ( y > 0xef ) {
                    continue;
                }
                get_sprite( idx, current_sprite );
                int palNum = (att & 0x3) + 4;
                uint8_t *palette = &mem_[0x3F00 + palNum * 4];

                // merge sprite and nametable
                // TODO : sprite priority
                for ( int yy = 0; yy < 8; yy++ ) {
                    for ( int xx = 0; xx < 8; xx++ ) {
                        uint8_t* screen_pixel = &nametable_[0] + (y+yy)*(8*32) + (x+xx);
                        uint8_t sprite_val = current_sprite[xx+yy*8];

                        if ( sprite_val ) {
                            if ( i == 0 && *screen_pixel ) {
                                status_.bits.sprite0_hit = 1;
                            }
                            *screen_pixel = palette[sprite_val];
                        }
                    }
                }
            }
        }
    }
#else

    // pre-render scanline
    if ( scanline_ == 261 ) {
        // fecth two first tiles of the next scanline (0)
    }
    else if ( scanline_ >= 0 && scanline_ <= 239 ) {
        // visible scanline
        if ( tick_ < 256 ) {
            uint16_t nt_addr = (ctrl_.bits.nametable << 10) | 0x2000;
            uint16_t bg_addr = ctrl_.bits.background_pattern ? 0x1000 : 0;
            // Scroll : CCCCCFFF
            int coarse_xs = scroll_.bits.x >> 3;
            int coarse_ys = scroll_.bits.y >> 3;
            int fine_xs = scroll_.bits.x & 0x7;
            int fine_ys = scroll_.bits.y & 0x7;
            int x = tick_; // + fine_xs ?
            int y = scanline_; // + fine_y ?
            int tile_x = (x >> 3) + coarse_xs; // starting at tile #2
            int tile_y = (y >> 3) + coarse_ys;
            if (tile_x & 32) {
                tile_x -= 32;
                nt_addr ^= 0x400;
            }
            int x16 = tile_x>>1;
            int y16 = tile_y>>1;

            uint8_t nametable_byte = mem_[ nt_addr + (tile_y<<5)+tile_x ];
            uint8_t pal = mem_[ nt_addr + 0x3C0 + (tile_x>>2) + ((tile_y>>2) << 3) ];
            int y_in_tile = y & 0x7;
            int x_in_tile = x & 0x7;
            uint8_t tile_l = mem_[ bg_addr + (nametable_byte<<4)+y_in_tile+0 ];
            uint8_t tile_h = mem_[ bg_addr + (nametable_byte<<4)+y_in_tile+8 ];

            // bg color
            int d = 7 - x_in_tile;
            uint8_t c = ((tile_l & (1<<d)) >> d) |
            (((tile_h & (1<<d)) >> d)<<1);
            // palette number
            if ( x16&1 ) {
                if ( y16&1 ) {
                    pal = (pal & 0xC0) >> 6;
                }
                else {
                    pal = (pal & 0x0C) >> 2;
                }
            }
            else {
                if ( y16&1 ) {
                    pal = (pal & 0x30) >> 4;
                }
                else {
                    pal = (pal & 0x03);
                }
            }
            uint8_t p_color = c ? mem_[0x3F00 + pal * 4 + c ] : mem_[0x3F00];
            //            p_color = 0;

#if 1
            if ( mask_.bits.show_sprites ) {
                // sprites
                for ( int i = 0; i < n_next_sprites_; i++ ) {
                    if ( sprite_x_[i] ) {
                        sprite_x_[i]--;
                    }
                    else {
                        
                        // TODO deal with 8x16 sprites
                        //                        if ( ! ctrl_.bits.sprites_are_8x16 ) {
                        uint8_t idx = oam2_[ i * 4 + 1 ];
                        uint8_t att = oam2_[ i * 4 + 2 ];
                        uint8_t pal = (att & 3) + 4;
                        
                        uint8_t sp_c = ((next_sprites_[i][0] & 0x80) >> 7) | ( ((next_sprites_[i][1] & 0x80) >> 7) << 1 );
                        next_sprites_[i][0] <<= 1;
                        next_sprites_[i][1] <<= 1;
                        /*if (idx == 0 && sp_c && c )*/ {
                            status_.bits.sprite0_hit = 1;
                        }
                        if (sp_c) {
                            p_color = sp_c ? mem_[0x3F00 + pal * 4 + sp_c ] : mem_[0x3F00];
                        }
                    }
                }
            }
#endif
            nametable_[ y*256+x ] = p_color;
        }
        else if ( tick_ < 321 ) {
#if 1
            // fetch sprites for the next scanline
            uint16_t baseAddr = ctrl_.bits.sprite_pattern ? 0x1000 : 0;
            int j = 0;
            for ( int i = 0; j < 8 && i < 64; i++ ) {
                uint8_t sy   = oam_[ i * 4 + 0 ];
                uint8_t sx   = oam_[ i * 4 + 3 ];
                if ( sy > 0xef ) {
                    continue;
                }
                if ( scanline_ >= sy + 8 || scanline_ < sy ) {
                    continue;
                }
                memcpy( &oam2_[j*4], &oam_[i*4], 4 );
                next_sprites_[j][0] = mem_[ baseAddr + (i<<4) + (scanline_-sy) + 0 ];
                next_sprites_[j][1] = mem_[ baseAddr + (i<<4) + (scanline_-sy) + 8 ];
                sprite_x_[j] = sx;
                j++;
            }
            n_next_sprites_ = j;
#endif
        }
        else if ( tick_ < 337 ) {
        }
    }
#endif
    

    if ( (tick_ == 0) && (scanline_ == 240 ) ) {
        // copy temporary screen to screen
        memcpy( &screen_[0], &nametable_[0], 32*30*64 );
        
        {
            uint8_t *rgb;
            int pitch;
            SDL_LockTexture( screen_tex_, NULL, (void**)&rgb, &pitch );
            for ( size_t i = 0; i < 30*32*8*8; i++ ) {
                rgb[i*3+0] = NesPalette[screen_[i]][0];
                rgb[i*3+1] = NesPalette[screen_[i]][1];
                rgb[i*3+2] = NesPalette[screen_[i]][2];
            }
            SDL_UnlockTexture( screen_tex_ );
        }
            
        SDL_RenderClear(renderer_);
        SDL_RenderCopy(renderer_, screen_tex_, NULL, NULL);
        SDL_RenderPresent(renderer_);
    }
}

void PPU::tick()
{
    frame();

    tick_ = (tick_ + 1) % 341;
    if (tick_ == 0 ) {
        scanline_ = (scanline_ + 1) % 262;
    }

    if ( (tick_ == 1) && (scanline_ == 241) ) {
        status_.bits.vblank = 1;
        if ( ctrl_.bits.nmi ) {
            cpu_->triggerNMI();
        }
    }

    // pre-render
    if ( scanline_ == 261 ) {
        if ( tick_ == 1 ) {
            status_.bits.vblank = 0;
            status_.bits.sprite0_hit = 0;
        }
    }
}

uint8_t PPU::read( uint16_t addr ) const
{
    if ( addr > 7 ) {
        printf("Trying to read to PPU register #%x\n", addr );
        throw OutOfBoundAddress();
    }
    if ( addr == PPUStatus ) {
        uint8_t c = status_.raw;
        status_.bits.vblank = 0;
        return c;
    }
    else if ( addr == PPUData ) {
        uint8_t b = mem_[ ppuaddr % mem_.size() ];
        if ( ppuaddr == 0x3F10 || ppuaddr == 0x3F14 || ppuaddr == 0x3F18 || ppuaddr == 0x3F1C ) {
            b = mem_[ ppuaddr - 0x10 ];
        }
        ppuaddr = ppuaddr + (ctrl_.bits.vram_increment ? 32 : 1 );
        return b;
    }
    else if ( addr == OAMAddr ) {
        return oam_addr_;
    }
    else if ( addr == OAMData ) {
        return oam_[oam_addr_];
    }
    printf("read %04X\n", addr);
    //    std::cin.get();
    return 0;
}

void PPU::write( uint16_t addr, uint8_t val )
{
    if ( addr > 7 ) {
        printf("Trying to write to PPU register #%x\n", addr );
        throw OutOfBoundAddress();
    }
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
        if ( ppuaddr == 0x3F10 || ppuaddr == 0x3F14 || ppuaddr == 0x3F18 || ppuaddr == 0x3F1C ) {
            mem_[ ppuaddr - 0x10 ] = val;
        }
        else {
            mem_[ ppuaddr % mem_.size() ] = val;
        }
        ppuaddr = ppuaddr + (ctrl_.bits.vram_increment ? 32 : 1 );
    }
    else if ( addr == PPUScroll ) {
        //        printf("scroll: %d SL:%d tick:%d\n", val, scanline_, tick_);
        scroll_.raw = ( scroll_.raw << 8) | val;
    }
    else if ( addr == OAMAddr ) {
        oam_addr_ = val;
    }
    else if ( addr == OAMData ) {
        oam_[oam_addr_++] = val;
    }
    else {
        printf("write %04X <= %02X\n", addr, val);
        //        std::cin.get();
    }
}
