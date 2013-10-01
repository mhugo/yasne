#include <stdint.h>
#include <string.h>
#include <istream>
#include <string>

struct InstructionDefinition
{
    // raw opcode
    uint8_t opcode;

    // mnemonic
    enum Mnemonic
    {
        MNEMONIC_PHP,
        MNEMONIC_CLC,
        MNEMONIC_PLP,
        MNEMONIC_SEC,
        MNEMONIC_PHA,
        MNEMONIC_CLI,
        MNEMONIC_PLA,
        MNEMONIC_SEI,
        MNEMONIC_DEY,
        MNEMONIC_TYA,
        MNEMONIC_TAY,
        MNEMONIC_CLV,
        MNEMONIC_INY,
        MNEMONIC_CLD,
        MNEMONIC_INX,
        MNEMONIC_SED,
        MNEMONIC_TXA,
        MNEMONIC_TXS,
        MNEMONIC_TAX,
        MNEMONIC_TSX,
        MNEMONIC_DEX,
        MNEMONIC_NOP,
        MNEMONIC_BRK,
        MNEMONIC_BPL,
        MNEMONIC_JSR,
        MNEMONIC_BMI,
        MNEMONIC_RTI,
        MNEMONIC_BVC,
        MNEMONIC_RTS,
        MNEMONIC_BVS,
        MNEMONIC_BCC,
        MNEMONIC_BCS,
        MNEMONIC_BNE,
        MNEMONIC_BEQ,
        MNEMONIC_ORA,
        MNEMONIC_AND,
        MNEMONIC_EOR,
        MNEMONIC_ADC,
        MNEMONIC_STA,
        MNEMONIC_LDA,
        MNEMONIC_CMP,
        MNEMONIC_SBC,
        MNEMONIC_ASL,
        MNEMONIC_ROL,
        MNEMONIC_LSR,
        MNEMONIC_ROR,
        MNEMONIC_STX,
        MNEMONIC_LDX,
        MNEMONIC_DEC,
        MNEMONIC_INC,
        MNEMONIC_BIT,
        MNEMONIC_JMP,
        MNEMONIC_STY,
        MNEMONIC_LDY,
        MNEMONIC_CPY,
        MNEMONIC_CPX
    };
    Mnemonic mnemonic;

    static const char * MnemonicString[56];

    // addressing mode
    enum Addressing
    {
        ADDRESSING_NONE,
        ADDRESSING_IMMEDIATE,
        ADDRESSING_ZERO_PAGE,
        ADDRESSING_ACCUMULATOR,
        ADDRESSING_ABSOLUTE,
        ADDRESSING_RELATIVE,
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
                           Mnemonic mnemo,
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

#define FLAG_N_MASK (1<<7)
#define FLAG_V_MASK (1<<6)
#define FLAG_X_MASK (1<<5)
#define FLAG_B_MASK (1<<4)
#define FLAG_D_MASK (1<<3)
#define FLAG_I_MASK (1<<2)
#define FLAG_Z_MASK (1<<1)
#define FLAG_C_MASK (1<<0)

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

    // update status based on a stored value
    void updateStatus( uint8_t v );

    void push( uint16_t v );
    uint16_t pop();
};