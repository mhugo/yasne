#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

#include "nes_file_importer.hpp"
#include "cpu.hpp"
#include "ppu.hpp"

#define MEM_SIZE 65536
uint8_t* memory;

int main( int argc, char *argv[] )
{
    iNESHeader header;

    if ( argc < 2 ) {
        std::cerr << "Arguments: nes_file [log_file]" << std::endl;
        return 1;
    }
    bool testMode = argc > 2;

    std::string nesFilePath = argv[1];
    std::ifstream nesFile( nesFilePath.c_str() );

    nesFile.read( (char*)&header, sizeof( header ) );

    std::cout << header << std::endl;

    const uint16_t baseAddr = 0xC000;
    std::vector<uint8_t> mem(MEM_SIZE);
    memory = &mem[0];
    memset( memory, 0xFF, MEM_SIZE );

    std::vector<uint8_t> rom(16384);
    nesFile.read( (char*)&rom[0], 16384 );

    InstructionDefinition::initTable();

    CPU cpu;

    cpu.pc = baseAddr;
    //    cpu.pc = (memory[0xfffd] << 8) | memory[0xfffc];
    cpu.sp = 0xFD;
    cpu.status = 0x24;
    cpu.regA = 0;
    cpu.regX = 0;
    cpu.regY = 0;
    cpu.memory = memory;
    cpu.cycles = 0;

    //    cpu.addWriteWatch( 0 );

    ROM romDevice( 16384, &rom[0] );
    RAM ramDevice( 2048 );
    PPU ppu;

    cpu.addOnBus( 0x0000, &ramDevice, 0x0000 );
    cpu.addOnBus( 0x0800, &ramDevice, 0x0800 );
    cpu.addOnBus( 0x1000, &ramDevice, 0x1000 );
    cpu.addOnBus( 0x1800, &ramDevice, 0x1800 );
    cpu.addOnBus( 0xC000, &romDevice, 0xC000 );
    for ( int i = 0; i < 0x2000 / 8; i += 8 ) {
        cpu.addOnBus( 0x2000+i, &ppu, 0x2000+i );
    }

    bool stepByStep = false;

    // compare to log file
    std::ifstream logFile;
    if ( testMode ) {
        std::string logFilePath = argv[2];
        logFile.open( logFilePath.c_str() );
    }

    while ( true ) {
        // expected processor state in testMode
        unsigned int addr, regA, regX, regY, regP, regSP;
        if ( testMode ) {
            // log line
            std::string line;
            std::getline( logFile, line );
            if ( logFile.eof() ) {
                break;
            }
            
            // line
            // address on 4 characters
            // cpu state starting at character 48
            std::string log_pc_str = line.substr(0,4);
            std::string reg_a_str = line.substr(50,2);
            std::string reg_x_str = line.substr(55,2);
            std::string reg_y_str = line.substr(60,2);
            std::string reg_p_str = line.substr(65,2);
            std::string reg_sp_str = line.substr(71,2);
            sscanf( log_pc_str.c_str(), "%04X", &addr );
            sscanf( reg_a_str.c_str(), "%04X", &regA );
            sscanf( reg_x_str.c_str(), "%04X", &regX );
            sscanf( reg_y_str.c_str(), "%04X", &regY );
            sscanf( reg_p_str.c_str(), "%04X", &regP );
            sscanf( reg_sp_str.c_str(), "%04X", &regSP );
        }

        Instruction instr = cpu.decode( cpu.pc );
        uint16_t cpu_pc = cpu.pc;

        printf("%04X\t", cpu.pc);
        std::cout << instr;

        printf("\tA:%02X X:%02X Y:%02X P:%02X SP:%02X CPU:%d\n", cpu.regA, cpu.regX, cpu.regY, cpu.status, cpu.sp, cpu.cycles );

        if ( testMode ) {
            if ( (cpu_pc != addr ) ||
                 (cpu.regA != regA) ||
                 (cpu.regX != regX) ||
                 (cpu.regY != regY) ||
                 (cpu.status != regP ) ||
                 (cpu.sp != regSP )) {
                printf("Wrong status!\n");
                printf("Expected: %04X A:%02X X:%02X Y:%02X P:%02X SP:%02X\n", addr, regA, regX, regY, regP, regSP );
                break;
            }
        }
        

        cpu.pc += instr.nOperands + 1;
        bool pause = false;
        try {
            cpu.execute( instr );
        }
        catch ( CPU::ReadWatchTriggered& ) {
            std::cout << "Read watch triggered" << std::endl;
            pause = true;
        }
        catch ( CPU::WriteWatchTriggered& ) {
            std::cout << "Write watch triggered" << std::endl;
            pause = true;
        }
        if ( stepByStep ) {
            pause = true;
        }
        if ( pause ) {
            std::cin.get();
        }

        ppu.frame();
    }
    std::cout << "End" << std::endl;

    return 0;
}
