#include <stdint.h>
#include <ostream>

//      Header (16 bytes)
//     Trainer, if present (0 or 512 bytes)
//     PRG ROM data (16384 * x bytes)
//     CHR ROM data, if present (8192 * y bytes)
//     PlayChoice INST-ROM, if present (0 or 8192 bytes)
//     PlayChoice PROM, if present (16 bytes Data, 16 bytes CounterOut) (this is often missing, see PC10 ROM-Images for details) 

// Some ROM-Images additionally contain a 128-byte (or sometimes 127-byte) title at the end of the file.

// The format of the header is as follows:

//     0-3: Constant $4E $45 $53 $1A ("NES" followed by MS-DOS end-of-file)
//     4: Size of PRG ROM in 16 KB units
//     5: Size of CHR ROM in 8 KB units (Value 0 means the board uses CHR RAM)
//     6: Flags 6
//     7: Flags 7
//     8: Size of PRG RAM in 8 KB units (Value 0 infers 8 KB for compatibility; see PRG RAM circuit)
//     9: Flags 9
//     10: Flags 10 (unofficial)
//     11-15: Zero filled 

struct iNESHeader
{
    char constant[4];
    uint8_t PRGRomSize; // in 16 kB units
    uint8_t CHRRomSize; // in 8 kB units
    uint8_t flags6;
    uint8_t flags7;
    uint8_t PRGRamSize; // in 8 kB units
    uint8_t flags9;
    uint8_t flags10;
    uint8_t padding[5];
};

std::ostream& operator<<( std::ostream&, const iNESHeader& );
