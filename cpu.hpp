#include <stdint.h>
#include <string.h>
#include <istream>
#include <string>

struct InstructionDefinition
{
    // raw opcode
    uint8_t opcode;

    // mnemonic
    std::string mnemonic;

    // addressing mode
    enum Addressing
    {
        ADDRESSING_NONE,
        ADDRESSING_IMMEDIATE,
        ADDRESSING_ZERO_PAGE,
        ADDRESSING_ACCUMULATOR,
        ADDRESSING_ABSOLUTE,
        ADDRESSING_INDIRECT,
        ADDRESSING_ABSOLUTE_X,
        ADDRESSING_ABSOLUTE_Y,
        ADDRESSING_ZERO_PAGE_X,
        ADDRESSING_ZERO_PAGE_Y,
        ADDRESSING_INDIRECT_X,
        ADDRESSING_INDIRECT_Y
    };
    Addressing addressing;

    // number of operands
    int nOperands;

    // valid ?
    bool valid;

    // get the global instruction definition table
    static InstructionDefinition* table()
    {
        static InstructionDefinition table_[256];
        return table_;
    }

    static void initTable();

    // default constructor
    InstructionDefinition() : valid( false ) {}

    InstructionDefinition( uint8_t opcode,
                           std::string mnemo,
                           Addressing addressing,
                           int nOperands ) :
        opcode(opcode),
        mnemonic(mnemo),
        addressing(addressing),
        nOperands(nOperands),
        valid( true )
    {
    }
};

struct Instruction
{
    // raw opcode
    uint8_t opcode;
    // number of operands
    int nOperands;
    // operands, if needed
    uint8_t operand1, operand2;
    // valid ?
    bool valid;

    Instruction() : valid( false ) {}

    static Instruction decode( const uint8_t* adr );
};

std::ostream& operator<<( std::ostream& ostr, const Instruction& instr );

struct CPU
{
    uint8_t regA, regX, regY;
    uint8_t status;
    // stack pointer;
    uint8_t sp;

    // pc
    uint16_t pc;

    uint8_t *memory;

    void execute( const Instruction& instr );
    void transfer( uint8_t* target, uint8_t src);
    uint8_t resolveAddressing( const Instruction& instr );
};
