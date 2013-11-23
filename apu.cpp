#include "apu.hpp"

APU::APU( CPU* cpu, Controller* controller ) : cpu_(cpu), controller_(controller) {}
APU::~APU() {}

uint8_t APU::read( uint16_t addr ) const
{
    if ( addr == 0x16 ) {
        return controller_->readPressed( 0 ) ? 1 : 0;
    }
    else if ( addr == 0x17 ) {
        return controller_->readPressed( 1 ) ? 1 : 0;
    }
    if ( addr > 0x17 ) {
        printf("Trying to read APU register #%x\n", addr );
        throw OutOfBoundAddress();
    }
    return 0;
}

void APU::write( uint16_t addr, uint8_t val )
{
    if ( addr == 0x16 ) {
        controller_->setStrobe( val & 1 );
        return;
    }
    if ( addr > 0x17 ) {
        printf("Trying to write APU register #%x\n", addr );
        throw OutOfBoundAddress();
    }
}
