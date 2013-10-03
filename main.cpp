#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

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
    std::vector<uint8_t> mem(MEM_SIZE);
    memory = &mem[0];
    memset( memory, 0xFF, MEM_SIZE );
    nesFile.read( (char*)(memory + baseAddr), 16384 );

    InstructionDefinition::initTable();

    CPU cpu;

    cpu.pc = baseAddr;
    cpu.sp = 0xFD;
    cpu.status = 0x24;
    cpu.regA = 0;
    cpu.regX = 0;
    cpu.regY = 0;

    // compare to log file
    std::ifstream logFile( logFilePath.c_str() );

    while ( true ) {
        // log line
        std::string line;
        std::getline( logFile, line );

        // line
        // address on 4 characters
        // cpu state starting at character 48
        std::string log_pc_str = line.substr(0,4);
        std::string reg_a_str = line.substr(50,2);
        std::string reg_x_str = line.substr(55,2);
        std::string reg_y_str = line.substr(60,2);
        std::string reg_p_str = line.substr(65,2);
        std::string reg_sp_str = line.substr(71,2);
        unsigned int addr, regA, regX, regY, regP, regSP;
        sscanf( log_pc_str.c_str(), "%04X", &addr );
        sscanf( reg_a_str.c_str(), "%02X", &regA );
        sscanf( reg_x_str.c_str(), "%02X", &regX );
        sscanf( reg_y_str.c_str(), "%02X", &regY );
        sscanf( reg_p_str.c_str(), "%02X", &regP );
        sscanf( reg_sp_str.c_str(), "%02X", &regSP );

        const uint8_t* pt = (const uint8_t*)(memory + cpu.pc);
        Instruction instr = Instruction::decode( pt );
        uint16_t cpu_pc = cpu.pc;

        std::cout << std::hex << std::setfill('0') << std::setw(4) << cpu.pc << "\t" << std::dec;
        std::cout << instr;

        std::cout << "\tA:" << (cpu.regA+0) << " X:" << (cpu.regX+0) << " Y:" << (cpu.regY+0) << " P:" << (cpu.status+0) << " SP:" << (cpu.sp+0);
        std::cout << std::endl;

        if ( (cpu_pc != addr ) ||
             (cpu.regA != regA) ||
             (cpu.regX != regX) ||
             (cpu.regY != regY) ||
             (cpu.status != regP ) ||
             (cpu.sp != regSP )) {
            std::cout << "Wrong status !" << std::endl;
            std::cout << "Expected : " << addr << " A:"<<regA << " X:" << regX << " Y:" << regY << " P:" << regP << " SP:" << regSP << std::endl;
            break;
        }

        cpu.pc += instr.nOperands + 1;
        cpu.execute( instr );
    }
}
