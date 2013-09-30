#include <fstream>
#include <iostream>
#include <iomanip>

#include "nes_file_importer.hpp"
#include "cpu.hpp"

#define MEM_SIZE 65536
uint8_t* memory;

int main( int argc, char *argv[] )
{
    iNESHeader header;

    std::ifstream nesFile( argv[1] );

    nesFile.read( (char*)&header, sizeof( header ) );

    std::cout << header << std::endl;

    const uint16_t baseAddr = 0xC000;
    memory = new uint8_t[MEM_SIZE];
    nesFile.read( (char*)(memory + baseAddr), 16384 );

    InstructionDefinition::initTable();

    CPU cpu;

    cpu.pc = baseAddr;
    cpu.sp = 0xFD;
    //    while ( pc < memory + MEM_SIZE ) {
    for ( int i = 0; i < 20; i++ ) {
        const uint8_t* pt = (const uint8_t*)(memory + cpu.pc);
        Instruction instr = Instruction::decode( pt );

        std::cout << std::hex << std::setfill('0') << std::setw(4) << cpu.pc << "\t" << std::dec;
        std::cout << instr << std::endl;
        cpu.pc += instr.nOperands + 1;
        cpu.execute( instr );
    }
}
