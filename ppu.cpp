#include <stdexcept>

#include "ppu.hpp"

PPU::PPU()
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

void PPU::frame()
{
    SDL_Event e;
    if ( SDL_PollEvent(&e) ) {
        if ( e.type == SDL_QUIT ) {
            exit(0);
        }
    }
    SDL_RenderClear(renderer_);
    //    SDL_RenderCopy(renderer_, tex, NULL, NULL);
    SDL_RenderPresent(renderer_);
}

uint8_t PPU::read( uint16_t addr ) const
{
    return 0;
}

void PPU::write( uint16_t addr, uint8_t val )
{
}
