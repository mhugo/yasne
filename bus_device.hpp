#ifndef JANE_BUS_DEVICE
#define JANE_BUS_DEVICE

///
/// BusDevice : a device plugged on the CPU buses
/// Must have a method to read and a method to write
class BusDevice
{
public:
    virtual uint8_t read( uint16_t addr ) const = 0;
    virtual void write( uint16_t addr, uint8_t val ) = 0;

    // exception thrown by out-of-range address access
    struct OutOfBoundAddress {};
};

#endif
