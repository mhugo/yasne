#include <stdexcept>
#include <iostream>
#include <fstream>
#include "ppu.hpp"
#include "cpu.hpp"

std::ostream& operator<<( std::ostream& ostr, const PPU::Address& adr )
{
    adr.print(ostr);
    return ostr;
}
std::ostream& operator<<( std::ostream& ostr, const PPU::Mask& adr )
{
    adr.print(ostr);
    return ostr;
}
std::ostream& operator<<( std::ostream& ostr, const PPU::Status& adr )
{
    adr.print(ostr);
    return ostr;
}
std::ostream& operator<<( std::ostream& ostr, const PPU::Controller& adr )
{
    adr.print(ostr);
    return ostr;
}
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
                       ppuaddr_t( 0 ),
                       cpu_( cpu ),
                       oam_addr_( 0 ),
                       write_low_addr_( 0 ),
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
    std::cout << "ctrl: " << ctrl_ << std::endl;
    std::cout << "mask:" << mask_ << std::endl;
    std::cout << "status:" << status_ << std::endl;
    std::cout << "ppuaddr:" << ppuaddr << " ppuaddr_t:" << ppuaddr_t << std::endl;
    std::cout << "tick:" << tick_ << " SL:" << scanline_ << std::endl;

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

void PPU::dump_mem( const std::string& out_file ) const
{
    uint16_t pattern = ctrl_.bits.background_pattern ? 0x1000 : 0;
    uint16_t nametable = (ctrl_.bits.nametable << 10) | 0x2000;
    std::string chr_file = out_file + ".chr";
    std::string pal_file = out_file + ".pal";
    std::string nam_file = out_file + ".nam";
    std::ofstream of_chr( chr_file.c_str() );
    of_chr.write( (char*)&mem_[pattern], 0x1000 );
    of_chr.close();
    std::ofstream of_nam( nam_file.c_str() );
    of_nam.write( (char*)&mem_[nametable], 0x400 );
    of_nam.close();
    std::ofstream of_pal( pal_file.c_str() );
    of_pal.write( (char*)&mem_[0x3F00], 16 );
    of_pal.close();
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
    // pre-render scanline
    if ( scanline_ == 261 ) {
        // fecth two first tiles of the next scanline (0)
    }
    else if ( scanline_ >= 0 && scanline_ <= 239 ) {
        if ( ! mask_.bits.show_background ) {
            return;
        }
        // visible scanline
        if ( tick_ < 256 ) {
            int x = tick_;
            int y = scanline_;
            int x16 = ppuaddr.bits.coarse_x / 2;
            int y16 = ppuaddr.bits.coarse_y / 2;

            //            std::cout << "x:" << x << " cx:" << (ppuaddr.bits.coarse_x) << " fx:" << ppuaddr.bits.fine_x << " px:" << (ppuaddr.bits.coarse_x * 8 + ppuaddr.bits.fine_x)<< std::endl;

            uint16_t tile_addr = 0x2000 | (ppuaddr.raw & 0x03FF);

            //uint16_t tile_addr = 0x2000 | ((y/8)<<5) | (x/8);
            //            std::cout << std::hex << tile_addr << std::dec << std::endl;
            uint16_t attr_addr = 0x23C0 |
                (ppuaddr.bits.nametable << 10) |
                ((ppuaddr.bits.coarse_y / 4) << 3) |
                (ppuaddr.bits.coarse_x / 4);

            uint8_t nametable_byte = mem_[ tile_addr ];
            uint8_t pal = mem_[ attr_addr ];

            uint16_t bg_addr = ctrl_.bits.background_pattern ? 0x1000 : 0;
            uint8_t tile_l = mem_[ bg_addr + (nametable_byte<<4)+ppuaddr.bits.fine_y+0 ];
            uint8_t tile_h = mem_[ bg_addr + (nametable_byte<<4)+ppuaddr.bits.fine_y+8 ];

            // bg color
            int d = 7 - ppuaddr.bits.fine_x;
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

#if 0
            if ( mask_.bits.show_sprites ) {
                // sprites
                for ( int i = 0; i < n_next_sprites_; i++ ) {
                    if ( sprite_x_[i] > 0 ) {
                        sprite_x_[i]--;
                    }
                    else if ( sprite_x_[i] >= -7 ) {
                        
                        // TODO deal with 8x16 sprites
                        //                        if ( ! ctrl_.bits.sprites_are_8x16 ) {
                        uint8_t idx = oam2_[ i * 4 + 1 ];
                        uint8_t att = oam2_[ i * 4 + 2 ];

                        // Sprite attributes
                        // 76543210
                        // ||||||||
                        // ||||||++- Palette (4 to 7) of sprite
                        // |||+++--- Unimplemented
                        // ||+------ Priority (0: in front of background; 1: behind background)
                        // |+------- Flip sprite horizontally
                        // +-------- Flip sprite vertically
                        
                        uint8_t pal = (att & 3) + 4;
                        bool h_flip = (att & 0x40);
                        bool v_flip = (att & 0x80);
                        uint8_t sp_c;
                        if ( h_flip ) {
                            sp_c = (next_sprites_[i][0] & 1) | ( (next_sprites_[i][1] & 1) << 1 );
                            next_sprites_[i][0] >>= 1;
                            next_sprites_[i][1] >>= 1;
                        }
                        else {
                            sp_c = ((next_sprites_[i][0] & 0x80) >> 7) | ( ((next_sprites_[i][1] & 0x80) >> 7) << 1 );
                            next_sprites_[i][0] <<= 1;
                            next_sprites_[i][1] <<= 1;
                        }
                        if (idx == 0 && sp_c && c ) {
                            status_.bits.sprite0_hit = 1;
                        }
                        if (sp_c) {
                            p_color = sp_c ? mem_[0x3F00 + pal * 4 + sp_c ] : mem_[0x3F00];
                        }
                        sprite_x_[i]--;
                    }
                }
            }
#endif
            screen_[ y*256+x ] = p_color;

            // increment x
            if ( ppuaddr.bits.fine_x < 7 ) {
                ppuaddr.bits.fine_x ++;
            }
            else {
                ppuaddr.bits.fine_x = 0;
                if ( ppuaddr.bits.coarse_x < 31 ) {
                    ppuaddr.bits.coarse_x ++;
                }
                else {
                    ppuaddr.bits.coarse_x = 0;
                    // switch nametable 2000 <-> 2400, 2800 <-> 2C00
                    ppuaddr.bits.nametable ^= 1;
                }
            }

            //            render();
        }
        else if ( tick_ < 321 ) {
            if ( tick_ == 256 ) {
                //                std::cout << "y:" << scanline_ << " cy:" << (ppuaddr.bits.coarse_y) << " fy:" << ppuaddr.bits.fine_y << " py:" << (ppuaddr.bits.coarse_y * 8 + ppuaddr.bits.fine_y)<< std::endl;

            render();
                // increment Y
                if ( ppuaddr.bits.fine_y < 7 ) {
                    ppuaddr.bits.fine_y ++;
                }
                else {
                    ppuaddr.bits.fine_y = 0;
                    if ( ppuaddr.bits.coarse_y == 29 ) {
                        ppuaddr.bits.coarse_y = 0;
                        //                        ppuaddr.bits.nametable ^= 2;
                    }
                    else if ( ppuaddr.bits.coarse_y == 31 ) {
                        ppuaddr.bits.coarse_y = 0;
                    }
                    else {
                        ppuaddr.bits.coarse_y ++;
                    }
                }
            }
            else if ( tick_ == 257 ) {
                // copy t to v (horizontal part)
                //v: ....F.. ...EDCBA = t: ....F.. ...EDCBA
                ppuaddr.raw |= ppuaddr_t.raw & 0x041F;
            }
            else if ( tick_ >= 280 && tick_ <= 304 ) {
                //v: IHGF.ED CBA..... = t: IHGF.ED CBA.....
                ppuaddr.raw |= ppuaddr_t.raw & 0x7BE0;
            }
#if 0
            // fetch sprites for the next scanline
            uint16_t baseAddr = ctrl_.bits.sprite_pattern ? 0x1000 : 0;
            int j = 0;
            for ( int i = 0; (j < 8) && (i < 64); i++ ) {
                uint8_t sy   = oam_[ i * 4 + 0 ];
                uint8_t sx   = oam_[ i * 4 + 3 ];
                uint8_t idx  = oam_[ i * 4 + 1 ];
                uint8_t att  = oam_[ i * 4 + 2 ];
                if ( sy > 0xef ) {
                    continue;
                }
                if ( scanline_ >= sy + 8 || scanline_ < sy ) {
                    continue;
                }
                int dy = scanline_-sy;
                if (att & 0x80) { // vertical flip
                    dy = 7 - dy;
                }
                memcpy( &oam2_[j*4], &oam_[i*4], 4 );
                next_sprites_[j][0] = mem_[ baseAddr + (idx*16) + dy + 0 ];
                next_sprites_[j][1] = mem_[ baseAddr + (idx*16) + dy + 8 ];
                sprite_x_[j] = sx;
                j++;
            }
            n_next_sprites_ = j;
#endif
        }
        else if ( tick_ < 337 ) {
        }
    }
    
#if 1
    if ( (tick_ == 0) && (scanline_ == 240 ) ) {
        render();
    }
#endif
}

void PPU::render()
{
    // copy temporary screen to screen
    //    memcpy( &screen_[0], &nametable_[0], 32*30*64 );
        
    uint8_t *rgb;
    int pitch;
    SDL_LockTexture( screen_tex_, NULL, (void**)&rgb, &pitch );
    for ( size_t i = 0; i < 30*32*8*8; i++ ) {
        rgb[i*3+0] = NesPalette[screen_[i]][0];
        rgb[i*3+1] = NesPalette[screen_[i]][1];
        rgb[i*3+2] = NesPalette[screen_[i]][2];
    }
    SDL_UnlockTexture( screen_tex_ );

    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, screen_tex_, NULL, NULL);
    SDL_RenderPresent(renderer_);
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
        // reset toggle
        write_low_addr_ = 0;
        return c;
    }
    else if ( addr == PPUData ) {
        uint16_t addr = ppuaddr.raw & 0x3FFF;
        uint8_t b = mem_[ addr ];
        if ( addr == 0x3F10 || addr == 0x3F14 || addr == 0x3F18 || addr == 0x3F1C ) {
            b = mem_[ addr - 0x10 ];
        }
        ppuaddr.raw = ppuaddr.raw + (ctrl_.bits.vram_increment ? 32 : 1 );
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
        // select nametable address
        ppuaddr_t.bits.nametable = ctrl_.bits.nametable;
    }
    else if ( addr == PPUMask ) {
        mask_.raw = val;
    }
    else if ( addr == PPUAddr ) {
        //        std::cout << "set ppuaddr " << std::hex << int(val) << std::dec << std::endl;
        if ( write_low_addr_ == 1 ) {
            //t: ....... HGFEDCBA = d: HGFEDCBA
            //v                   = t
            ppuaddr_t.raw = (ppuaddr_t.raw & 0xFFFF00) | val;
            ppuaddr = ppuaddr_t;
        }
        else {
            //t: .FEDCBA ........ = d: ..FEDCBA
            //t: X...... ........ = 0
            ppuaddr_t.raw = ((val & 0x3F) << 8) | (ppuaddr_t.raw & 0xFFC0FF);
            ppuaddr_t.bits.fine_y &= 3;
        }
        write_low_addr_ ^= 1;
    }
    else if ( addr == PPUData ) {
        addr = ppuaddr.raw & 0x3FFF;
        if ( addr == 0x3F10 || addr == 0x3F14 || addr == 0x3F18 || addr == 0x3F1C ) {
            mem_[ addr - 0x10 ] = val;
        }
        else {
            mem_[ addr ] = val;
        }
        ppuaddr.raw = ppuaddr.raw + (ctrl_.bits.vram_increment ? 32 : 1 );
    }
    else if ( addr == PPUScroll ) {
        //        std::cout << "set ppuscroll " << std::hex << int(val) << std::dec << std::endl;
        if ( write_low_addr_ == 1 ) {
            //t: CBA..HG FED..... = d: HGFEDCBA
            ppuaddr_t.bits.coarse_y = val >> 3;
            ppuaddr_t.bits.fine_y = val & 7;
        }
        else {
            //t: ....... ...HGFED = d: HGFED...
            //x:              CBA = d: .....CBA
            ppuaddr_t.bits.coarse_x = val >> 3;
            ppuaddr_t.bits.fine_x = val & 7;
        }
        write_low_addr_ ^= 1;
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
