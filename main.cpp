#include <fstream>
#include <iostream>

#include "nes_file_importer.hpp"
#include "cpu.hpp"

#define MEM_SIZE 16384
uint8_t memory[MEM_SIZE];

int main( int argc, char *argv[] )
{
    iNESHeader header;

    std::ifstream nesFile( "../nestest.nes" );

    nesFile.read( (char*)&header, sizeof( header ) );

    std::cout << header << std::endl;

    nesFile.read( (char*)memory, MEM_SIZE );

    InstructionDefinition::initTable();

    const uint8_t *pc = memory;
    //    while ( pc < memory + MEM_SIZE ) {
    for ( int i = 0; i < 10; i++ ) {
        Instruction instr = Instruction::decode( pc );
        pc += instr.nOperands + 1;

        std::cout << instr << std::endl;
    }
}
