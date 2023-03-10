#include <readline/readline.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

#include "SDL.h"

#include "nes_file_importer.hpp"
#include "cpu.hpp"
#include "ppu.hpp"
#include "apu.hpp"
#include "controller.hpp"

#define MEM_SIZE 65536
uint8_t* memory;

void print_context( CPU& cpu, uint16_t base, int n )
{
    uint16_t addr = base;
    for ( int i = 0; i < n ; i++ ) {
        Instruction instr = cpu.decode( addr );

        printf("%04X\t", addr);
        std::cout << instr << std::endl;
        addr += instr.nOperands + 1;
    }
}

class Command
{
public:
    Command( const std::string& line )
    {
        parse( line );
    }

    std::string name() const { return name_; }

    size_t n_args() const { return args_.size(); }

    std::string arg( size_t i ) const { return args_[i]; }

private:
    void parse( const std::string& line )
    {
        std::istringstream istr(line);
        std::string token;
        for ( size_t i = 0; getline( istr, token, ' ' ); i++ ) {
            if ( i == 0 ) {
                name_ = token;
            }
            else {
                args_.push_back( token );
            }
        }
    }
    
    std::string name_;
    std::vector<std::string> args_;
};

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

    std::vector<uint8_t> rom(16384 * header.PRGRomSize);
    nesFile.read( (char*)&rom[0], 16384 * header.PRGRomSize);

    InstructionDefinition::initTable();

    // init SDL
    SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS );

    CPU cpu;

    cpu.pc = baseAddr;
    cpu.sp = 0xFD;
    cpu.status = 0x24;
    cpu.regA = 0;
    cpu.regX = 0;
    cpu.regY = 0;
    cpu.memory = memory;
    cpu.cycles = 0;

    ROM romDevice( rom.size(), &rom[0] );
    RAM ramDevice( 2048 );
    Controller controller;
    PPU ppu( &cpu );
    APU apu( &cpu, &controller );

    cpu.addOnBus( 0x0000, &ramDevice, 0x0000 );
    cpu.addOnBus( 0x0800, &ramDevice, 0x0800 );
    cpu.addOnBus( 0x1000, &ramDevice, 0x1000 );
    cpu.addOnBus( 0x1800, &ramDevice, 0x1800 );
    uint16_t romAddr = 0x10000 - rom.size();
    cpu.addOnBus( romAddr, &romDevice, romAddr );
    for ( int i = 0; i < 0x2000 / 8; i += 8 ) {
        cpu.addOnBus( 0x2000+i, &ppu, 0x2000+i );
    }
    cpu.addOnBus( 0x4000, &apu, 0x4000 );

    // load CHR
    nesFile.read( (char*)&ppu.memory()[0], 8192 );

    cpu.reset();

    bool stepMode = true;

    // compare to log file
    std::ifstream logFile;
    if ( testMode ) {
        std::string logFilePath = argv[2];
        logFile.open( logFilePath.c_str() );
    }

    bool pause = false;
    uint16_t breakAddr = 0;
    bool breakMode = false;
    bool breakOnFrame = false;
    while ( true ) {

        SDL_Event e;
        if ( SDL_PollEvent(&e) ) {
            if ( e.type == SDL_QUIT ) {
                break;
            }
            else if ( e.type == SDL_KEYDOWN || e.type == SDL_KEYUP ) {
                SDL_KeyboardEvent* ke = (SDL_KeyboardEvent*)(&e);
                bool pressed = ke->state == SDL_PRESSED;
                if ( ke->keysym.sym == SDLK_ESCAPE ) {
                    break;
                }
                else if ( ke->keysym.sym == SDLK_RETURN ) {
                    controller.setState( 0, Controller::StartButton, pressed );
                }
                else if ( ke->keysym.sym == SDLK_SPACE ) {
                    controller.setState( 0, Controller::SelectButton, pressed );
                }
                else if ( ke->keysym.sym == SDLK_RIGHT ) {
                    controller.setState( 0, Controller::RightButton, pressed );
                }
                else if ( ke->keysym.sym == SDLK_LEFT ) {
                    controller.setState( 0, Controller::LeftButton, pressed );
                }
                else if ( ke->keysym.sym == SDLK_UP ) {
                    controller.setState( 0, Controller::UpButton, pressed );
                }
                else if ( ke->keysym.sym == SDLK_DOWN ) {
                    controller.setState( 0, Controller::DownButton, pressed );
                }
                else if ( ke->keysym.sym == SDLK_a ) {
                    controller.setState( 0, Controller::AButton, pressed );
                }
                else if ( ke->keysym.sym == SDLK_b ) {
                    controller.setState( 0, Controller::BButton, pressed );
                }
                else if ( ke->keysym.sym == SDLK_d && pressed ) {
                    pause = true;
                }
            }
        }

        // expected processor state in testMode
        unsigned int addr, regA, regX, regY, regP, regSP, cyc;
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
            std::string cyc_str = line.substr(78,3);
            sscanf( log_pc_str.c_str(), "%04X", &addr );
            sscanf( reg_a_str.c_str(), "%04X", &regA );
            sscanf( reg_x_str.c_str(), "%04X", &regX );
            sscanf( reg_y_str.c_str(), "%04X", &regY );
            sscanf( reg_p_str.c_str(), "%04X", &regP );
            sscanf( reg_sp_str.c_str(), "%04X", &regSP );
            sscanf( cyc_str.c_str(), "%d", &cyc );
        }

        if ( stepMode ) {
            pause = true;
        }
        if ( breakMode && breakAddr == cpu.pc ) {
            printf("Breakpoint\n");
            pause = true;
        }
        if ( breakOnFrame && ppu.ticks() == 0 && ppu.scanline() == 0 ) {
            printf("Break on PPU frame\n");
            pause = true;
        }
        if ( pause ) {
            print_context( cpu, cpu.pc, 4 );

            printf("\tA:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%d\n", cpu.regA, cpu.regX, cpu.regY, cpu.status, cpu.sp, ppu.ticks() );


            bool doContinue = false;
            do {
                doContinue = false;
                std::string line( readline( "(dbg) " ) );
                Command command( line );
                switch ( command.name()[0] )
                {
                case 's': // step
                    stepMode = true;
                    pause = false;
                    break;
                case 'c': // continue
                    stepMode = false;
                    pause = false;
                    break;
                case 'b': { // breakpoint
                    if ( command.name() == "bf" ) { // break on next video frame
                        breakOnFrame = true;
                    }
                    else if ( command.name() == "bdf" ) { // disable break on frame
                        breakOnFrame = false;
                    }
                    else if ( command.name() == "bd" ) { // disable breakpoint
                        breakMode = false;
                    }
                    else {
                        breakMode = true;
                        unsigned int b;
                        sscanf( command.arg(0).c_str(), "%04X", &b );
                        breakAddr = b;
                    }
                    doContinue = true;
                    break;
                }
                case 'v': // related to video
                    if ( command.name() == "vdump" ) {
                        if ( command.n_args() == 1 ) {
                            std::string out_file( command.arg(0) );
                            ppu.dump_mem( out_file );
                        }
                    }
                    else {
                        ppu.print_context();
                    }
                    doContinue = true;
                    break;
                case 'p': {
                    doContinue = true;
                    // display a sprite
                    if ( command.n_args() == 0 ) {
                        break;
                    }
                    int s;
                    uint8_t sprite[8*8];
                    sscanf( command.arg(0).c_str(), "%x", &s );
                    ppu.get_sprite( s, sprite );
                    for ( int y = 0; y < 8; ++y ) {
                        for ( int x = 0; x < 8; ++x ) {
                            uint8_t c = sprite[y*8+x];
                            printf("%02X", c);
                        }
                        printf("\n");
                    }
                    
                    break;
                }
                // set game controller state
                case 'k': {
                    if ( command.n_args() == 0 ) {
                        controller.print( std::cout );
                        doContinue = true;
                        break;
                    }
                    int s;
                    sscanf( command.arg(1).c_str(), "%d", &s );
                    std::string keyName = command.arg(0);
                    std::transform( keyName.begin(), keyName.end(), keyName.begin(), tolower );
                    int key;
                    if ( keyName == "a" ) {
                        key = Controller::AButton;
                    }
                    else if ( keyName == "b" ) {
                        key = Controller::BButton;
                    }
                    else if ( keyName == "select" ) {
                        key = Controller::SelectButton;
                    }
                    else if ( keyName == "start" ) {
                        key = Controller::StartButton;
                    }
                    else if ( keyName == "up" ) {
                        key = Controller::UpButton;
                    }
                    else if ( keyName == "down" ) {
                        key = Controller::DownButton;
                    }
                    else if ( keyName == "left" ) {
                        key = Controller::LeftButton;
                    }
                    else if ( keyName == "right" ) {
                        key = Controller::RightButton;
                    }
                    controller.setState( 0, key, s == 1 );
                    controller.print( std::cout );
                    doContinue = true;
                    break;
                }
                case 'w': // watch
                    if ( command.n_args() < 2 ) {
                        printf("w addr r|w: add read/write watch\n");
                        doContinue = true;
                        break;
                    }
                    unsigned int addr;
                    sscanf( command.arg(0).c_str(), "%04X", &addr );
                    if ( command.arg(1)[0] == 'r' ) {
                        cpu.addReadWatch( addr );
                    }
                    else {
                        cpu.addWriteWatch( addr );
                    }
                    doContinue = true;
                    break;
                case 'q':
                    return 0;
                    break;
                }
            } while ( doContinue );
        }

        uint16_t cpu_pc = cpu.pc;
        Instruction instr = cpu.decode( cpu.pc );
        
        if ( testMode ) {
            if ( (cpu_pc != addr ) ||
                 (cpu.regA != regA) ||
                 (cpu.regX != regX) ||
                 (cpu.regY != regY) ||
                 (cpu.status != regP ) ||
                 (cpu.sp != regSP ) ||
                 (ppu.ticks() != cyc  )) {
                printf("Wrong status!\n");
                printf("Expected: %04X A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%d\n", addr, regA, regX, regY, regP, regSP, cyc );
                break;
            }
        }

        cpu.cycles = 0;
        cpu.pc += instr.nOperands + 1;
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
        while ( cpu.cycles-- ) {
            // ppu cycle
            ppu.tick();
            ppu.tick();
            ppu.tick();
        }
    }
    std::cout << "End" << std::endl;

    return 0;
}
