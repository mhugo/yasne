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

    if ( argc < 3 ) {
        std::cerr << "Arguments: nes_file log_file" << std::endl;
        return 1;
    }

    std::string nesFilePath = argv[1];
    std::string logFilePath = argv[2];
    std::ifstream nesFile( nesFilePath.c_str() );

    nesFile.read( (char*)&header, sizeof( header ) );

    std::cout << header << std::endl;

    const uint16_t baseAddr = 0xC000;
    memory = new uint8_t[MEM_SIZE];
    nesFile.read( (char*)(memory + baseAddr), 16384 );

    InstructionDefinition::initTable();

    CPU cpu;

    cpu.pc = baseAddr;
    cpu.sp = 0xFD;
    cpu.status = 0x24;

    // compare to log file
    std::ifstream logFile( logFilePath.c_str() );

    //    while ( pc < memory + MEM_SIZE ) {
    for ( int i = 0; i < 50; i++ ) {
        const uint8_t* pt = (const uint8_t*)(memory + cpu.pc);
        Instruction instr = Instruction::decode( pt );

        std::cout << std::hex << std::setfill('0') << std::setw(4) << cpu.pc << "\t" << std::dec;
        std::cout << instr;
        std::cout << "\tA:" << (cpu.regA+0) << " X:" << (cpu.regX+0) << " Y:" << (cpu.regY+0) << " P:" << (cpu.status+0);
        std::cout << std::endl;
        cpu.pc += instr.nOperands + 1;
        cpu.execute( instr );

        // log line
        std::string line;
        std::getline( logFile, line );

        {
            // line
            // address on 4 characters
            // cpu state starting at character 48
            std::string log_pc_str = line.substr(0,4);
            std::string reg_a_str = line.substr(50,2);
            std::string reg_x_str = line.substr(55,2);
            std::string reg_y_str = line.substr(60,2);
            std::string reg_p_str = line.substr(65,2);
            unsigned int addr, regA, regX, regY, regP;
            sscanf( log_pc_str.c_str(), "%04X", &addr );
            sscanf( reg_a_str.c_str(), "%04X", &regA );
            sscanf( reg_x_str.c_str(), "%04X", &regX );
            sscanf( reg_y_str.c_str(), "%04X", &regY );
            sscanf( reg_p_str.c_str(), "%04X", &regP );
            std::cout << addr << " " << reg_a_str << " " << reg_x_str << " " << reg_y_str << " " << reg_p_str << std::endl;
        }
    }
}