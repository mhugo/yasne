#include "nes_file_importer.hpp"

std::ostream& operator<<( std::ostream& ostr, const iNESHeader& header )
{
    ostr << "PRG ROM Size: " << header.PRGRomSize + 0;
}
