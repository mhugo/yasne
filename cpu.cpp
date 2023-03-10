#include <stdexcept>
#include <iostream>
#include <iomanip>

#include <boost/format.hpp>

#include "cpu.hpp"

const char * InstructionDefinition::MnemonicString[] = 
{
    "Illegal",
    "PHP",
    "CLC",
    "PLP",
    "SEC",
    "PHA",
    "CLI",
    "PLA",
    "SEI",
    "DEY",
    "TYA",
    "TAY",
    "CLV",
    "INY",
    "CLD",
    "INX",
    "SED",
    "TXA",
    "TXS",
    "TAX",
    "TSX",
    "DEX",
    "NOP",
    "BRK",
    "BPL",
    "JSR",
    "BMI",
    "RTI",
    "BVC",
    "RTS",
    "BVS",
    "BCC",
    "BCS",
    "BNE",
    "BEQ",
    "ORA",
    "AND",
    "EOR",
    "ADC",
    "STA",
    "LDA",
    "CMP",
    "SBC",
    "ASL",
    "ROL",
    "LSR",
    "ROR",
    "STX",
    "LDX",
    "DEC",
    "INC",
    "BIT",
    "JMP",
    "STY",
    "LDY",
    "CPY",
    "CPX",
    "LAX",
    "SAX",
    "DCP",
    "ISC",
    "RLA",
    "RRA",
    "SLO",
    "SRE"
};

const char * AddressingString[] = 
{
    "NONE",
    "IMMEDIATE",
    "ZERO_PAGE",
    "ACCUMULATOR",
    "ABSOLUTE",
    "RELATIVE",
    "INDIRECT",
    "ABSOLUTE_X",
    "ABSOLUTE_Y",
    "ZERO_PAGE_X",
    "ZERO_PAGE_Y",
    "INDIRECT_X",
    "INDIRECT_Y"
};

void InstructionDefinition::initTable()
{
    InstructionDefinition* table = InstructionDefinition::table();

#define DEF_INSTRUCTION( opcode, mnemonic, addressing, ncycles ) table[opcode] = InstructionDefinition( opcode, mnemonic, addressing, 0, ncycles )

    DEF_INSTRUCTION(0x00, MNEMONIC_BRK, ADDRESSING_NONE, 7);
    DEF_INSTRUCTION(0x01, MNEMONIC_ORA, ADDRESSING_INDIRECT_X, 6);
    DEF_INSTRUCTION(0x02, MNEMONIC_ASL, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0x03, MNEMONIC_SLO, ADDRESSING_INDIRECT_X, 8);
    DEF_INSTRUCTION(0x04, MNEMONIC_NOP, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0x05, MNEMONIC_ORA, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0x06, MNEMONIC_ASL, ADDRESSING_ZERO_PAGE, 5);
    DEF_INSTRUCTION(0x07, MNEMONIC_SLO, ADDRESSING_ZERO_PAGE, 5);
    DEF_INSTRUCTION(0x08, MNEMONIC_PHP, ADDRESSING_NONE, 3);
    DEF_INSTRUCTION(0x09, MNEMONIC_ORA, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0x0a, MNEMONIC_ASL, ADDRESSING_ACCUMULATOR, 2);
    DEF_INSTRUCTION(0x0b, MNEMONIC_ILL, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x0c, MNEMONIC_NOP, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0x0d, MNEMONIC_ORA, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0x0e, MNEMONIC_ASL, ADDRESSING_ABSOLUTE, 6);
    DEF_INSTRUCTION(0x0f, MNEMONIC_SLO, ADDRESSING_ABSOLUTE, 6);
    DEF_INSTRUCTION(0x10, MNEMONIC_BPL, ADDRESSING_RELATIVE, 2);
    DEF_INSTRUCTION(0x11, MNEMONIC_ORA, ADDRESSING_INDIRECT_Y, 5);
    DEF_INSTRUCTION(0x12, MNEMONIC_NOP, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x13, MNEMONIC_SLO, ADDRESSING_INDIRECT_Y, 8);
    DEF_INSTRUCTION(0x14, MNEMONIC_NOP, ADDRESSING_ZERO_PAGE_X, 4);
    DEF_INSTRUCTION(0x15, MNEMONIC_ORA, ADDRESSING_ZERO_PAGE_X, 4);
    DEF_INSTRUCTION(0x16, MNEMONIC_ASL, ADDRESSING_ZERO_PAGE_X, 6);
    DEF_INSTRUCTION(0x17, MNEMONIC_SLO, ADDRESSING_ZERO_PAGE_X, 6);
    DEF_INSTRUCTION(0x18, MNEMONIC_CLC, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x19, MNEMONIC_ORA, ADDRESSING_ABSOLUTE_Y, 4);
    DEF_INSTRUCTION(0x1a, MNEMONIC_NOP, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x1b, MNEMONIC_SLO, ADDRESSING_ABSOLUTE_Y, 7);
    DEF_INSTRUCTION(0x1c, MNEMONIC_NOP, ADDRESSING_ABSOLUTE_X, 4);
    DEF_INSTRUCTION(0x1d, MNEMONIC_ORA, ADDRESSING_ABSOLUTE_X, 4);
    DEF_INSTRUCTION(0x1e, MNEMONIC_ASL, ADDRESSING_ABSOLUTE_X, 7);
    DEF_INSTRUCTION(0x1f, MNEMONIC_SLO, ADDRESSING_ABSOLUTE_X, 7);
    DEF_INSTRUCTION(0x20, MNEMONIC_JSR, ADDRESSING_ABSOLUTE, 6);
    DEF_INSTRUCTION(0x21, MNEMONIC_AND, ADDRESSING_INDIRECT_X, 6);
    DEF_INSTRUCTION(0x22, MNEMONIC_ROL, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0x23, MNEMONIC_RLA, ADDRESSING_INDIRECT_X, 8);
    DEF_INSTRUCTION(0x24, MNEMONIC_BIT, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0x25, MNEMONIC_AND, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0x26, MNEMONIC_ROL, ADDRESSING_ZERO_PAGE, 5);
    DEF_INSTRUCTION(0x27, MNEMONIC_RLA, ADDRESSING_ZERO_PAGE, 5);
    DEF_INSTRUCTION(0x28, MNEMONIC_PLP, ADDRESSING_NONE, 4);
    DEF_INSTRUCTION(0x29, MNEMONIC_AND, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0x2a, MNEMONIC_ROL, ADDRESSING_ACCUMULATOR, 2);
    DEF_INSTRUCTION(0x2b, MNEMONIC_ILL, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x2c, MNEMONIC_BIT, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0x2d, MNEMONIC_AND, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0x2e, MNEMONIC_ROL, ADDRESSING_ABSOLUTE, 6);
    DEF_INSTRUCTION(0x2f, MNEMONIC_RLA, ADDRESSING_ABSOLUTE, 6);
    DEF_INSTRUCTION(0x30, MNEMONIC_BMI, ADDRESSING_RELATIVE, 2);
    DEF_INSTRUCTION(0x31, MNEMONIC_AND, ADDRESSING_INDIRECT_Y, 5);
    DEF_INSTRUCTION(0x32, MNEMONIC_NOP, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x33, MNEMONIC_RLA, ADDRESSING_INDIRECT_Y, 8);
    DEF_INSTRUCTION(0x34, MNEMONIC_NOP, ADDRESSING_ZERO_PAGE_X, 4);
    DEF_INSTRUCTION(0x35, MNEMONIC_AND, ADDRESSING_ZERO_PAGE_X, 4);
    DEF_INSTRUCTION(0x36, MNEMONIC_ROL, ADDRESSING_ZERO_PAGE_X, 6);
    DEF_INSTRUCTION(0x37, MNEMONIC_RLA, ADDRESSING_ZERO_PAGE_X, 6);
    DEF_INSTRUCTION(0x38, MNEMONIC_SEC, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x39, MNEMONIC_AND, ADDRESSING_ABSOLUTE_Y, 4);
    DEF_INSTRUCTION(0x3a, MNEMONIC_NOP, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x3b, MNEMONIC_RLA, ADDRESSING_ABSOLUTE_Y, 7);
    DEF_INSTRUCTION(0x3c, MNEMONIC_NOP, ADDRESSING_ABSOLUTE_X, 4);
    DEF_INSTRUCTION(0x3d, MNEMONIC_AND, ADDRESSING_ABSOLUTE_X, 4);
    DEF_INSTRUCTION(0x3e, MNEMONIC_ROL, ADDRESSING_ABSOLUTE_X, 7);
    DEF_INSTRUCTION(0x3f, MNEMONIC_RLA, ADDRESSING_ABSOLUTE_X, 7);
    DEF_INSTRUCTION(0x40, MNEMONIC_RTI, ADDRESSING_NONE, 6);
    DEF_INSTRUCTION(0x41, MNEMONIC_EOR, ADDRESSING_INDIRECT_X, 6);
    DEF_INSTRUCTION(0x42, MNEMONIC_LSR, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0x43, MNEMONIC_SRE, ADDRESSING_INDIRECT_X, 8);
    DEF_INSTRUCTION(0x44, MNEMONIC_NOP, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0x45, MNEMONIC_EOR, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0x46, MNEMONIC_LSR, ADDRESSING_ZERO_PAGE, 5);
    DEF_INSTRUCTION(0x47, MNEMONIC_SRE, ADDRESSING_ZERO_PAGE, 5);
    DEF_INSTRUCTION(0x48, MNEMONIC_PHA, ADDRESSING_NONE, 3);
    DEF_INSTRUCTION(0x49, MNEMONIC_EOR, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0x4a, MNEMONIC_LSR, ADDRESSING_ACCUMULATOR, 2);
    DEF_INSTRUCTION(0x4b, MNEMONIC_ILL, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x4c, MNEMONIC_JMP, ADDRESSING_ABSOLUTE, 3);
    DEF_INSTRUCTION(0x4d, MNEMONIC_EOR, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0x4e, MNEMONIC_LSR, ADDRESSING_ABSOLUTE, 6);
    DEF_INSTRUCTION(0x4f, MNEMONIC_SRE, ADDRESSING_ABSOLUTE, 6);
    DEF_INSTRUCTION(0x50, MNEMONIC_BVC, ADDRESSING_RELATIVE, 2);
    DEF_INSTRUCTION(0x51, MNEMONIC_EOR, ADDRESSING_INDIRECT_Y, 5);
    DEF_INSTRUCTION(0x52, MNEMONIC_NOP, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x53, MNEMONIC_SRE, ADDRESSING_INDIRECT_Y, 8);
    DEF_INSTRUCTION(0x54, MNEMONIC_NOP, ADDRESSING_ZERO_PAGE_X, 4);
    DEF_INSTRUCTION(0x55, MNEMONIC_EOR, ADDRESSING_ZERO_PAGE_X, 4);
    DEF_INSTRUCTION(0x56, MNEMONIC_LSR, ADDRESSING_ZERO_PAGE_X, 6);
    DEF_INSTRUCTION(0x57, MNEMONIC_SRE, ADDRESSING_ZERO_PAGE_X, 6);
    DEF_INSTRUCTION(0x58, MNEMONIC_CLI, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x59, MNEMONIC_EOR, ADDRESSING_ABSOLUTE_Y, 4);
    DEF_INSTRUCTION(0x5a, MNEMONIC_NOP, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x5b, MNEMONIC_SRE, ADDRESSING_ABSOLUTE_Y, 7);
    DEF_INSTRUCTION(0x5c, MNEMONIC_NOP, ADDRESSING_ABSOLUTE_X, 4);
    DEF_INSTRUCTION(0x5d, MNEMONIC_EOR, ADDRESSING_ABSOLUTE_X, 4);
    DEF_INSTRUCTION(0x5e, MNEMONIC_LSR, ADDRESSING_ABSOLUTE_X, 7);
    DEF_INSTRUCTION(0x5f, MNEMONIC_SRE, ADDRESSING_ABSOLUTE_X, 7);
    DEF_INSTRUCTION(0x60, MNEMONIC_RTS, ADDRESSING_NONE, 6);
    DEF_INSTRUCTION(0x61, MNEMONIC_ADC, ADDRESSING_INDIRECT_X, 6);
    DEF_INSTRUCTION(0x62, MNEMONIC_ROR, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0x63, MNEMONIC_RRA, ADDRESSING_INDIRECT_X, 8);
    DEF_INSTRUCTION(0x64, MNEMONIC_NOP, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0x65, MNEMONIC_ADC, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0x66, MNEMONIC_ROR, ADDRESSING_ZERO_PAGE, 5);
    DEF_INSTRUCTION(0x67, MNEMONIC_RRA, ADDRESSING_ZERO_PAGE, 5);
    DEF_INSTRUCTION(0x68, MNEMONIC_PLA, ADDRESSING_NONE, 4);
    DEF_INSTRUCTION(0x69, MNEMONIC_ADC, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0x6a, MNEMONIC_ROR, ADDRESSING_ACCUMULATOR, 2);
    DEF_INSTRUCTION(0x6b, MNEMONIC_ILL, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x6c, MNEMONIC_JMP, ADDRESSING_INDIRECT, 5);
    DEF_INSTRUCTION(0x6d, MNEMONIC_ADC, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0x6e, MNEMONIC_ROR, ADDRESSING_ABSOLUTE, 6);
    DEF_INSTRUCTION(0x6f, MNEMONIC_RRA, ADDRESSING_ABSOLUTE, 6);
    DEF_INSTRUCTION(0x70, MNEMONIC_BVS, ADDRESSING_RELATIVE, 2);
    DEF_INSTRUCTION(0x71, MNEMONIC_ADC, ADDRESSING_INDIRECT_Y, 5);
    DEF_INSTRUCTION(0x72, MNEMONIC_NOP, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x73, MNEMONIC_RRA, ADDRESSING_INDIRECT_Y, 8);
    DEF_INSTRUCTION(0x74, MNEMONIC_NOP, ADDRESSING_ZERO_PAGE_X, 4);
    DEF_INSTRUCTION(0x75, MNEMONIC_ADC, ADDRESSING_ZERO_PAGE_X, 4);
    DEF_INSTRUCTION(0x76, MNEMONIC_ROR, ADDRESSING_ZERO_PAGE_X, 6);
    DEF_INSTRUCTION(0x77, MNEMONIC_RRA, ADDRESSING_ZERO_PAGE_X, 6);
    DEF_INSTRUCTION(0x78, MNEMONIC_SEI, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x79, MNEMONIC_ADC, ADDRESSING_ABSOLUTE_Y, 4);
    DEF_INSTRUCTION(0x7a, MNEMONIC_NOP, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x7b, MNEMONIC_RRA, ADDRESSING_ABSOLUTE_Y, 7);
    DEF_INSTRUCTION(0x7c, MNEMONIC_NOP, ADDRESSING_ABSOLUTE_X, 4);
    DEF_INSTRUCTION(0x7d, MNEMONIC_ADC, ADDRESSING_ABSOLUTE_X, 4);
    DEF_INSTRUCTION(0x7e, MNEMONIC_ROR, ADDRESSING_ABSOLUTE_X, 7);
    DEF_INSTRUCTION(0x7f, MNEMONIC_RRA, ADDRESSING_ABSOLUTE_X, 7);
    DEF_INSTRUCTION(0x80, MNEMONIC_NOP, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0x81, MNEMONIC_STA, ADDRESSING_INDIRECT_X, 6);
    DEF_INSTRUCTION(0x82, MNEMONIC_STX, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0x83, MNEMONIC_SAX, ADDRESSING_INDIRECT_X, 6);
    DEF_INSTRUCTION(0x84, MNEMONIC_STY, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0x85, MNEMONIC_STA, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0x86, MNEMONIC_STX, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0x87, MNEMONIC_SAX, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0x88, MNEMONIC_DEY, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x89, MNEMONIC_STA, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0x8a, MNEMONIC_TXA, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x8b, MNEMONIC_ILL, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x8c, MNEMONIC_STY, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0x8d, MNEMONIC_STA, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0x8e, MNEMONIC_STX, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0x8f, MNEMONIC_SAX, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0x90, MNEMONIC_BCC, ADDRESSING_RELATIVE, 2);
    DEF_INSTRUCTION(0x91, MNEMONIC_STA, ADDRESSING_INDIRECT_Y, 6);
    DEF_INSTRUCTION(0x92, MNEMONIC_NOP, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x93, MNEMONIC_ILL, ADDRESSING_NONE, 6);
    DEF_INSTRUCTION(0x94, MNEMONIC_STY, ADDRESSING_ZERO_PAGE_X, 4);
    DEF_INSTRUCTION(0x95, MNEMONIC_STA, ADDRESSING_ZERO_PAGE_X, 4);
    DEF_INSTRUCTION(0x96, MNEMONIC_STX, ADDRESSING_ZERO_PAGE_Y, 4);
    DEF_INSTRUCTION(0x97, MNEMONIC_SAX, ADDRESSING_ZERO_PAGE_Y, 4);
    DEF_INSTRUCTION(0x98, MNEMONIC_TYA, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x99, MNEMONIC_STA, ADDRESSING_ABSOLUTE_Y, 5);
    DEF_INSTRUCTION(0x9a, MNEMONIC_TXS, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0x9b, MNEMONIC_ILL, ADDRESSING_NONE, 5);
    DEF_INSTRUCTION(0x9c, MNEMONIC_STY, ADDRESSING_ABSOLUTE_X, 5);
    DEF_INSTRUCTION(0x9d, MNEMONIC_STA, ADDRESSING_ABSOLUTE_X, 5);
    DEF_INSTRUCTION(0x9e, MNEMONIC_STX, ADDRESSING_ABSOLUTE_Y, 5);
    DEF_INSTRUCTION(0x9f, MNEMONIC_ILL, ADDRESSING_NONE, 5);
    DEF_INSTRUCTION(0xa0, MNEMONIC_LDY, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0xa1, MNEMONIC_LDA, ADDRESSING_INDIRECT_X, 6);
    DEF_INSTRUCTION(0xa2, MNEMONIC_LDX, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0xa3, MNEMONIC_LAX, ADDRESSING_INDIRECT_X, 6);
    DEF_INSTRUCTION(0xa4, MNEMONIC_LDY, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0xa5, MNEMONIC_LDA, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0xa6, MNEMONIC_LDX, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0xa7, MNEMONIC_LAX, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0xa8, MNEMONIC_TAY, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xa9, MNEMONIC_LDA, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0xaa, MNEMONIC_TAX, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xab, MNEMONIC_ILL, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xac, MNEMONIC_LDY, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0xad, MNEMONIC_LDA, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0xae, MNEMONIC_LDX, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0xaf, MNEMONIC_LAX, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0xb0, MNEMONIC_BCS, ADDRESSING_RELATIVE, 2);
    DEF_INSTRUCTION(0xb1, MNEMONIC_LDA, ADDRESSING_INDIRECT_Y, 5);
    DEF_INSTRUCTION(0xb2, MNEMONIC_NOP, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xb3, MNEMONIC_LAX, ADDRESSING_INDIRECT_Y, 5);
    DEF_INSTRUCTION(0xb4, MNEMONIC_LDY, ADDRESSING_ZERO_PAGE_X, 4);
    DEF_INSTRUCTION(0xb5, MNEMONIC_LDA, ADDRESSING_ZERO_PAGE_X, 4);
    DEF_INSTRUCTION(0xb6, MNEMONIC_LDX, ADDRESSING_ZERO_PAGE_Y, 4);
    DEF_INSTRUCTION(0xb7, MNEMONIC_LAX, ADDRESSING_ZERO_PAGE_Y, 4);
    DEF_INSTRUCTION(0xb8, MNEMONIC_CLV, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xb9, MNEMONIC_LDA, ADDRESSING_ABSOLUTE_Y, 4);
    DEF_INSTRUCTION(0xba, MNEMONIC_TSX, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xbb, MNEMONIC_ILL, ADDRESSING_NONE, 4);
    DEF_INSTRUCTION(0xbc, MNEMONIC_LDY, ADDRESSING_ABSOLUTE_X, 4);
    DEF_INSTRUCTION(0xbd, MNEMONIC_LDA, ADDRESSING_ABSOLUTE_X, 4);
    DEF_INSTRUCTION(0xbe, MNEMONIC_LDX, ADDRESSING_ABSOLUTE_Y, 4);
    DEF_INSTRUCTION(0xbf, MNEMONIC_LAX, ADDRESSING_ABSOLUTE_Y, 4);
    DEF_INSTRUCTION(0xc0, MNEMONIC_CPY, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0xc1, MNEMONIC_CMP, ADDRESSING_INDIRECT_X, 6);
    DEF_INSTRUCTION(0xc2, MNEMONIC_DEC, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0xc3, MNEMONIC_DCP, ADDRESSING_INDIRECT_X, 8);
    DEF_INSTRUCTION(0xc4, MNEMONIC_CPY, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0xc5, MNEMONIC_CMP, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0xc6, MNEMONIC_DEC, ADDRESSING_ZERO_PAGE, 5);
    DEF_INSTRUCTION(0xc7, MNEMONIC_DCP, ADDRESSING_ZERO_PAGE, 5);
    DEF_INSTRUCTION(0xc8, MNEMONIC_INY, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xc9, MNEMONIC_CMP, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0xca, MNEMONIC_DEX, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xcb, MNEMONIC_ILL, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xcc, MNEMONIC_CPY, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0xcd, MNEMONIC_CMP, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0xce, MNEMONIC_DEC, ADDRESSING_ABSOLUTE, 6);
    DEF_INSTRUCTION(0xcf, MNEMONIC_DCP, ADDRESSING_ABSOLUTE, 6);
    DEF_INSTRUCTION(0xd0, MNEMONIC_BNE, ADDRESSING_RELATIVE, 2);
    DEF_INSTRUCTION(0xd1, MNEMONIC_CMP, ADDRESSING_INDIRECT_Y, 5);
    DEF_INSTRUCTION(0xd2, MNEMONIC_NOP, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xd3, MNEMONIC_DCP, ADDRESSING_INDIRECT_Y, 8);
    DEF_INSTRUCTION(0xd4, MNEMONIC_NOP, ADDRESSING_ZERO_PAGE_X, 4);
    DEF_INSTRUCTION(0xd5, MNEMONIC_CMP, ADDRESSING_ZERO_PAGE_X, 4);
    DEF_INSTRUCTION(0xd6, MNEMONIC_DEC, ADDRESSING_ZERO_PAGE_X, 6);
    DEF_INSTRUCTION(0xd7, MNEMONIC_DCP, ADDRESSING_ZERO_PAGE_X, 6);
    DEF_INSTRUCTION(0xd8, MNEMONIC_CLD, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xd9, MNEMONIC_CMP, ADDRESSING_ABSOLUTE_Y, 4);
    DEF_INSTRUCTION(0xda, MNEMONIC_NOP, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xdb, MNEMONIC_DCP, ADDRESSING_ABSOLUTE_Y, 7);
    DEF_INSTRUCTION(0xdc, MNEMONIC_NOP, ADDRESSING_ABSOLUTE_X, 4);
    DEF_INSTRUCTION(0xdd, MNEMONIC_CMP, ADDRESSING_ABSOLUTE_X, 4);
    DEF_INSTRUCTION(0xde, MNEMONIC_DEC, ADDRESSING_ABSOLUTE_X, 7);
    DEF_INSTRUCTION(0xdf, MNEMONIC_DCP, ADDRESSING_ABSOLUTE_X, 7);
    DEF_INSTRUCTION(0xe0, MNEMONIC_CPX, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0xe1, MNEMONIC_SBC, ADDRESSING_INDIRECT_X, 6);
    DEF_INSTRUCTION(0xe2, MNEMONIC_INC, ADDRESSING_IMMEDIATE, 3);
    DEF_INSTRUCTION(0xe3, MNEMONIC_ISC, ADDRESSING_INDIRECT_X, 8);
    DEF_INSTRUCTION(0xe4, MNEMONIC_CPX, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0xe5, MNEMONIC_SBC, ADDRESSING_ZERO_PAGE, 3);
    DEF_INSTRUCTION(0xe6, MNEMONIC_INC, ADDRESSING_ZERO_PAGE, 5);
    DEF_INSTRUCTION(0xe7, MNEMONIC_ISC, ADDRESSING_ZERO_PAGE, 5);
    DEF_INSTRUCTION(0xe8, MNEMONIC_INX, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xe9, MNEMONIC_SBC, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0xea, MNEMONIC_NOP, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xeb, MNEMONIC_SBC, ADDRESSING_IMMEDIATE, 2);
    DEF_INSTRUCTION(0xec, MNEMONIC_CPX, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0xed, MNEMONIC_SBC, ADDRESSING_ABSOLUTE, 4);
    DEF_INSTRUCTION(0xee, MNEMONIC_INC, ADDRESSING_ABSOLUTE, 6);
    DEF_INSTRUCTION(0xef, MNEMONIC_ISC, ADDRESSING_ABSOLUTE, 6);
    DEF_INSTRUCTION(0xf0, MNEMONIC_BEQ, ADDRESSING_RELATIVE, 2);
    DEF_INSTRUCTION(0xf1, MNEMONIC_SBC, ADDRESSING_INDIRECT_Y, 5);
    DEF_INSTRUCTION(0xf2, MNEMONIC_NOP, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xf3, MNEMONIC_ISC, ADDRESSING_INDIRECT_Y, 8);
    DEF_INSTRUCTION(0xf4, MNEMONIC_NOP, ADDRESSING_ZERO_PAGE_X, 4);
    DEF_INSTRUCTION(0xf5, MNEMONIC_SBC, ADDRESSING_ZERO_PAGE_X, 4);
    DEF_INSTRUCTION(0xf6, MNEMONIC_INC, ADDRESSING_ZERO_PAGE_X, 6);
    DEF_INSTRUCTION(0xf7, MNEMONIC_ISC, ADDRESSING_ZERO_PAGE_X, 6);
    DEF_INSTRUCTION(0xf8, MNEMONIC_SED, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xf9, MNEMONIC_SBC, ADDRESSING_ABSOLUTE_Y, 4);
    DEF_INSTRUCTION(0xfa, MNEMONIC_NOP, ADDRESSING_NONE, 2);
    DEF_INSTRUCTION(0xfb, MNEMONIC_ISC, ADDRESSING_ABSOLUTE_Y, 7);
    DEF_INSTRUCTION(0xfc, MNEMONIC_NOP, ADDRESSING_ABSOLUTE_X, 4);
    DEF_INSTRUCTION(0xfd, MNEMONIC_SBC, ADDRESSING_ABSOLUTE_X, 4);
    DEF_INSTRUCTION(0xfe, MNEMONIC_INC, ADDRESSING_ABSOLUTE_X, 7);
    DEF_INSTRUCTION(0xff, MNEMONIC_ISC, ADDRESSING_ABSOLUTE_X, 7);
                                                                        
#undef DEF_INSTRUCTION

    for ( int i = 0; i < 256; i++ ) {
        switch ( table[i].addressing )
        {
        case ADDRESSING_NONE:
        case ADDRESSING_ACCUMULATOR:
            table[i].nOperands = 0;
            break;
        case ADDRESSING_IMMEDIATE:
        case ADDRESSING_RELATIVE:
        case ADDRESSING_ZERO_PAGE:
        case ADDRESSING_ZERO_PAGE_X:
        case ADDRESSING_ZERO_PAGE_Y:
        case ADDRESSING_INDIRECT_X:
        case ADDRESSING_INDIRECT_Y:
            table[i].nOperands = 1;
            break;
        case ADDRESSING_INDIRECT:
        case ADDRESSING_ABSOLUTE:
        case ADDRESSING_ABSOLUTE_X:
        case ADDRESSING_ABSOLUTE_Y:
            table[i].nOperands = 2;
            break;
        }
    }
}

void CPU::reset()
{
    pc = (readMem8(0xfffd) << 8) | readMem8(0xfffc);
}

void CPU::triggerNMI()
{
    push( pc );
    pushByte( status );
    pc = (readMem8(0xfffb) << 8) | readMem8(0xfffa);
}

void CPU::doDMA( uint16_t startAddr )
{
    for ( int i = 0; i < 256; ++i ) {
        uint8_t v = readMem8( startAddr + i );
        writeMem8( 0x2004, v );
    }
    cycles += 513;
    // TODO add odd frame cycle
}

void CPU::addOnBus( uint16_t addr, BusDevice* dev, uint16_t offset )
{
    busDevice.insert( addr, dev, offset );
}

void CPU::addReadWatch( uint16_t adr )
{
    read_watch.insert( adr );
}
void CPU::addWriteWatch( uint16_t adr )
{
    write_watch.insert( adr );
}

Instruction CPU::decode( uint16_t pc ) const
{
    Instruction instr;

    instr.opcode = readMem8( pc, true );
    InstructionDefinition def = InstructionDefinition::table()[ instr.opcode ];
    instr.nOperands = def.nOperands;
    if ( instr.nOperands >= 1 ) {
        instr.operand1 = readMem8( pc + 1, true );
        if ( instr.nOperands == 2 ) {
            instr.operand2 = readMem8( pc + 2, true );
        }
    }
    return instr;
}

std::ostream& operator<<( std::ostream& ostr, const Instruction& instr )
{
    InstructionDefinition def = InstructionDefinition::table() [ instr.opcode ];

    ostr << std::hex << std::setfill('0') << std::setw(2) << (instr.opcode + 0);

    if ( def.nOperands >= 1 ) {
        ostr << " " << std::setfill('0') << std::setw(2) << instr.operand1 + 0;
        if ( def.nOperands == 2 ) {
            ostr << " " << std::setfill('0') << std::setw(2) << instr.operand2 + 0;
        }
    }
    if ( def.nOperands == 2 ) {
        ostr << "\t";
    }
    else {
        ostr << "\t\t";
    }

    ostr << InstructionDefinition::MnemonicString[def.mnemonic];
    if ( ! def.valid ) {
        ostr << std::dec << "ILLEGAL";
        return ostr;
    }

    switch ( def.addressing )
    {
    case InstructionDefinition::ADDRESSING_NONE:
        ostr << "\t";
        break;
    case InstructionDefinition::ADDRESSING_IMMEDIATE:
        ostr << "\t#$" << std::setfill('0') << std::setw(2) << (instr.operand1+0);
        break;
    case InstructionDefinition::ADDRESSING_ZERO_PAGE:
        ostr << "\tZP$" << std::setfill('0') << std::setw(2) << (instr.operand1+0);
        break;
    case InstructionDefinition::ADDRESSING_ACCUMULATOR:
        ostr << "\tA";
        break;
    case InstructionDefinition::ADDRESSING_RELATIVE: {
        int8_t rel = instr.operand1;
        if ( rel < 0 ) {
            ostr << "\t-" <<  std::setfill('0') << std::setw(2) << (-rel+0);
        }
        else {
            ostr << "\t" << ((rel>=0)?"+":"") << std::setfill('0') << std::setw(2) << (rel+0);
        }
        break;
    }
    case InstructionDefinition::ADDRESSING_ABSOLUTE: {
        uint16_t adr = (instr.operand2 << 8) + instr.operand1;
        ostr << "\t$" << std::setfill('0') << std::setw(4) << adr;
        break;
    }
    case InstructionDefinition::ADDRESSING_INDIRECT: {
        uint16_t adr = (instr.operand2 << 8) + instr.operand1;
        ostr << "\t($" << std::setfill('0') << std::setw(4) << adr << ")";
        break;
    }
    case InstructionDefinition::ADDRESSING_ABSOLUTE_X: {
        uint16_t adr = (instr.operand2 << 8) + instr.operand1;
        ostr << "\t$" << std::setfill('0') << std::setw(4) << adr << ",X";
        break;
    }
    case InstructionDefinition::ADDRESSING_ABSOLUTE_Y: {
        uint16_t adr = (instr.operand2 << 8) + instr.operand1;
        ostr << "\t$" << std::setfill('0') << std::setw(4) << adr << ",Y";
        break;
    }
    case InstructionDefinition::ADDRESSING_ZERO_PAGE_X:
        ostr << "\tZP$" << std::setfill('0') << std::setw(2) << instr.operand1+0 << ",X";
        break;
    case InstructionDefinition::ADDRESSING_ZERO_PAGE_Y:
        ostr << "\tZP$" << std::setfill('0') << std::setw(2) << instr.operand1+0 << ",Y";
        break;
    case InstructionDefinition::ADDRESSING_INDIRECT_X:
        ostr << "\t($" << std::setw(2) << std::setfill('0') << (instr.operand1+0) << ",X)";
        break;
    case InstructionDefinition::ADDRESSING_INDIRECT_Y:
        ostr << "\t($" << std::setw(2) << std::setfill('0') << (instr.operand1+0) << "),Y";
        break;
    }
    ostr << std::dec;
    return ostr;
}

uint8_t CPU::readMem8( uint16_t addr, bool quiet ) const
{
    if ( read_watch.find( addr ) != read_watch.end() ) {
        throw ReadWatchTriggered();
    }
    uint8_t v = busDevice.read( addr );
#if 0
    if ( !quiet ) {
        std::cout << "@" << std::setw(4) << std::setfill('0') << addr;
        std::cout << " => " << (v+0) << std::endl;
    }
#endif
    return v;
}

void CPU::writeMem8( uint16_t addr, uint8_t v, bool quiet )
{
    if ( addr >= 0x4020 ) {
        throw std::runtime_error( (boost::format("Can't write to %1%") % addr).str() );
    }
#if 0
    if ( !quiet ) {
        std::cout << "@" << std::setw(4) << std::setfill('0') << addr ;
        std::cout << " <= " << (v+0) << std::endl;
    }
#endif
    busDevice.write( addr, v );
    if ( write_watch.find( addr ) != write_watch.end() ) {
        throw WriteWatchTriggered();
    }
}

uint8_t CPU::resolveAddressing( const Instruction& instr )
{
    InstructionDefinition def = InstructionDefinition::table()[ instr.opcode ];

    switch ( def.addressing )
    {
    case InstructionDefinition::ADDRESSING_NONE:
        break;
    case InstructionDefinition::ADDRESSING_IMMEDIATE:
        return instr.operand1;
        break;
    case InstructionDefinition::ADDRESSING_ZERO_PAGE:
        return readMem8( instr.operand1 );
        break;
    case InstructionDefinition::ADDRESSING_ABSOLUTE:
        return readMem8( instr.operand1 + (instr.operand2 << 8) );
    case InstructionDefinition::ADDRESSING_ABSOLUTE_X: {
        uint16_t baseAddr = (instr.operand2 << 8) | instr.operand1;
        uint16_t newAddr = baseAddr + regX;
        cycles += ((newAddr & 0xFF00) ^ (baseAddr & 0xFF00)) ? 1 : 0;
        return readMem8( newAddr );
    }
    case InstructionDefinition::ADDRESSING_ABSOLUTE_Y: {
        uint16_t baseAddr = (instr.operand2 << 8) | instr.operand1;
        uint16_t newAddr = baseAddr + regY;
        cycles += ((newAddr & 0xFF00) ^ (baseAddr & 0xFF00)) ? 1 : 0;
        return readMem8( newAddr );
    }
    case InstructionDefinition::ADDRESSING_INDIRECT_X: {
        uint8_t pz = instr.operand1 + regX;
        uint16_t addr = readMem8( pz );
        pz++; // here is the page 0 wrap
        addr |= readMem8( pz ) << 8;
        return readMem8(addr);
    }   
    case InstructionDefinition::ADDRESSING_INDIRECT_Y: {
        uint8_t pz = instr.operand1;
        uint16_t addr = readMem8( pz );
        pz++; // optional page wrap here
        addr |= readMem8( pz ) << 8;
        uint16_t newAddr = addr + regY;
        cycles += ((newAddr & 0xFF00) ^ (addr & 0xFF00)) ? 1 : 0;
        return readMem8( newAddr );
    }   
    case InstructionDefinition::ADDRESSING_ZERO_PAGE_X: {
        uint8_t pz = instr.operand1;
        pz += regX; // wraps around
        return readMem8(pz);
    }   
    case InstructionDefinition::ADDRESSING_ZERO_PAGE_Y: {
        uint8_t pz = instr.operand1;
        pz += regY; // wraps around
        return readMem8(pz);
    }   
    default:
        throw std::runtime_error("Unsupported addressing");
    }
}

uint16_t CPU::resolveWAddressing( const Instruction& instr )
{
    InstructionDefinition def = InstructionDefinition::table()[ instr.opcode ];

    switch ( def.addressing )
    {
    case InstructionDefinition::ADDRESSING_ZERO_PAGE:
        return instr.operand1;
        break;
    case InstructionDefinition::ADDRESSING_ABSOLUTE: {
        uint16_t adr = instr.operand1 + (instr.operand2 << 8);
        return adr;
        break;
    }
    case InstructionDefinition::ADDRESSING_ABSOLUTE_X: {
        uint16_t adr = instr.operand1 + (instr.operand2 << 8) + regX;
        return adr;
        break;
    }
    case InstructionDefinition::ADDRESSING_ABSOLUTE_Y: {
        uint16_t adr = instr.operand1 + (instr.operand2 << 8) + regY;
        return adr;
        break;
    }
    case InstructionDefinition::ADDRESSING_INDIRECT_X: {
        uint8_t pz = instr.operand1 + regX;
        uint16_t addr = readMem8( pz );
        pz++; // here is the page 0 wrap
        addr |= readMem8( pz ) << 8;
        return addr;
    }
    case InstructionDefinition::ADDRESSING_INDIRECT_Y: {
        uint8_t pz = instr.operand1;
        uint16_t addr = readMem8( pz );
        pz++; // optional page wrap here
        addr |= readMem8( pz ) << 8;
        addr += regY;
        return addr;
    }   
    case InstructionDefinition::ADDRESSING_ZERO_PAGE_X: {
        uint8_t adr = instr.operand1 + regX;
        return adr;
    }
    case InstructionDefinition::ADDRESSING_ZERO_PAGE_Y: {
        uint8_t adr = instr.operand1 + regY;
        return adr;
    }
    default:
        throw std::runtime_error("Unsupported W addressing ");
    }
    return 0;
}

void CPU::push( uint16_t v )
{
    pushByte( v >> 8 );
    pushByte( v & 0xff );
}

void CPU::pushByte( uint8_t v )
{
    writeMem8( 0x100 + sp, v );
    sp--;
}

uint16_t CPU::pop()
{
    uint16_t v = 0;
    v = popByte();
    v |= popByte() << 8;
    return v;
}

uint8_t CPU::popByte()
{
    uint8_t v;
    sp ++;
    v = readMem8( 0x100 + sp );
    return v;
}

void CPU::updateStatus( uint8_t v )
{
    updateZStatus( v );
    updateNStatus( v );
}

void CPU::updateVStatus( uint8_t v ) 
{
    if ( v & 0x40 ) {
        status |= FLAG_V_MASK;
    }
    else {
        status &= (0xFF-FLAG_V_MASK);
    }
}

void CPU::updateZStatus( uint8_t v )
{
    if ( v == 0 ) {
        status |= FLAG_Z_MASK;
    }
    else {
        status &= (0xFF-FLAG_Z_MASK);
    }
}

void CPU::updateNStatus( uint8_t v )
{
    if ( v & 0x80 ) {
        status |= FLAG_N_MASK;
    }
    else {
        status &= (0xFF-FLAG_N_MASK);
    }
}

uint8_t CPU::instr_dec( const Instruction& instr )
{
    // decrement memory
    uint16_t addr = resolveWAddressing( instr );
    uint8_t v = readMem8( addr );
    v--;
    writeMem8( addr, v );
    updateZStatus( v );
    updateNStatus( v );
    return v;
}

uint8_t CPU::instr_inc( const Instruction& instr )
{
    // increment memory
    uint16_t addr = resolveWAddressing( instr );
    uint8_t v = readMem8( addr );
    v++;
    writeMem8( addr, v );
    updateZStatus( v );
    updateNStatus( v );
    return v;
}

void CPU::instr_cmp( const Instruction& instr, uint8_t mem )
{
        // CMP regA  operand
        int16_t d = regA - mem;
        if ( d >= 0 ) {
            status |= FLAG_C_MASK;
        }
        else
        {
            status &= (0xFF - FLAG_C_MASK);
        }
        updateZStatus(d);
        updateNStatus(d);
}

void CPU::instr_sbc( const Instruction& instr, uint8_t mem )
{
    // substract with not carry A-M-(1-C)
    uint16_t s = regA - mem - ((status & FLAG_C_MASK) ? 0 : 1);
    int16_t s2 = (int8_t)regA - (int8_t)mem - ((status & FLAG_C_MASK) ? 0 : 1);
    if ( s <= 0xFF ) {
        status |= FLAG_C_MASK;
    }
    else
    {
        status &= (0xFF - FLAG_C_MASK);
    }
    regA = s & 0xFF;
    updateZStatus(regA);
    updateNStatus(regA);
    if ( (s2 < -128) || (s2 > 127) ) {
        status |= FLAG_V_MASK;
    }
    else {
        status &= (0xFF - FLAG_V_MASK);
    }
}

void CPU::instr_adc( const Instruction& instr, uint8_t mem )
{
    // add with carry A+M+C
    uint16_t s = regA + mem + ((status & FLAG_C_MASK) ? 1 : 0);
    int16_t s2 = (int8_t)regA + (int8_t)mem + ((status & FLAG_C_MASK) ? 1 : 0);
    if ( s > 0xFF ) {
        status |= FLAG_C_MASK;
    }
    else
    {
        status &= (0xFF - FLAG_C_MASK);
    }
    regA = s & 0xFF;
    updateZStatus(regA);
    updateNStatus(regA);
    if ( (s2 < -128) || (s2 > 127) ) {
        status |= FLAG_V_MASK;
    }
    else {
        status &= (0xFF - FLAG_V_MASK);
    }
}

uint8_t CPU::instr_asl( const Instruction& instr, const InstructionDefinition& def )
{
    // arithmetic shift left
    bool isAccu = def.addressing == InstructionDefinition::ADDRESSING_ACCUMULATOR;
    uint8_t v = regA;
    uint16_t addr;
    if ( !isAccu ) {
        addr = resolveWAddressing( instr );
        v = readMem8( addr );
    }
    uint16_t t = v << 1;
    if ( t & 0x100 ) {
        status |= FLAG_C_MASK;
    }
    else {
        status &= ~FLAG_C_MASK;
    }
    v = t & 0xFF;
    if ( isAccu ) {
        regA = v;
    }
    else {
        writeMem8( addr, v );
    }
    updateNStatus( v );
    updateZStatus( v );
    return v;
}

uint8_t CPU::instr_rol( const Instruction& instr, const InstructionDefinition& def )
{
    // rotate left
    bool isAccu = def.addressing == InstructionDefinition::ADDRESSING_ACCUMULATOR;
    uint8_t v = regA;
    uint16_t addr;
    if ( !isAccu ) {
        addr = resolveWAddressing( instr );
        v = readMem8( addr );
    }
    int oldCarry = (status & FLAG_C_MASK) ? 1:0;
    uint16_t t = v << 1;
    if ( t & 0x100 ) {
        status |= FLAG_C_MASK;
    }
    else {
        status &= ~FLAG_C_MASK;
    }
    v = (t & 0xFF) | oldCarry;
    if ( isAccu ) {
        regA = v;
    }
    else {
        writeMem8( addr, v );
    }
    updateNStatus( v );
    updateZStatus( v );
    return v;
}

uint8_t CPU::instr_ror( const Instruction& instr, const InstructionDefinition& def )
{
    // rotate right
    bool isAccu = def.addressing == InstructionDefinition::ADDRESSING_ACCUMULATOR;
    uint8_t v = regA;
    uint16_t addr;
    if ( !isAccu ) {
        addr = resolveWAddressing( instr );
        v = readMem8( addr );
    }
    uint8_t t = (((status&FLAG_C_MASK)?1:0) << 7) | (v >> 1);
    if ( v & 1 ) {
        status |= FLAG_C_MASK;
    }
    else {
        status &= ~FLAG_C_MASK;
    }
    v = t;
    if ( isAccu ) {
        regA = v;
    }
    else {
        writeMem8( addr, v );
    }
    updateNStatus( v );
    updateZStatus( v );
    return v;
}
uint8_t CPU::instr_lsr( const Instruction& instr, const InstructionDefinition& def )
{
    // logical shift right
    bool isAccu = def.addressing == InstructionDefinition::ADDRESSING_ACCUMULATOR;
    uint8_t v = regA;
    uint16_t addr;
    if ( !isAccu ) {
        addr = resolveWAddressing( instr );
        v = readMem8( addr );
    }
    if ( v & 0x1 ) {
        status |= FLAG_C_MASK;
    }
    else {
        status &= ~FLAG_C_MASK;
    }
    v = v >> 1;
    if ( isAccu ) {
        regA = v;
    }
    else {
        writeMem8( addr, v );
    }
    updateNStatus( v );
    updateZStatus( v );
    return v;
}
void CPU::instr_ora( const Instruction& instr, uint8_t mem )
{
    // OR regA & operand
    regA = regA | mem;
    updateZStatus(regA);
    updateNStatus(regA);
}

void CPU::instr_eor( const Instruction& instr, uint8_t mem )
{
    // XOR regA & operand
    regA = regA ^ mem;
    updateZStatus(regA);
    updateNStatus(regA);
}
void CPU::instr_and( const Instruction& instr, uint8_t mem )
{
    // AND regA & operand
    regA = regA & mem;
    updateZStatus(regA);
    updateNStatus(regA);
}

inline void CPU::branchTo( uint16_t addr )
{
    // add +1 cycle on a page crossing
    cycles += ((pc & 0xFF00) ^ (addr & 0xFF00)) ? 2 : 1;
    pc = addr;
}

void CPU::execute( const Instruction& instr )
{
    InstructionDefinition def = InstructionDefinition::table()[ instr.opcode ];

    if ( !def.valid ) {
        std::cout << "ILLEGAL instruction !" << std::endl;
    }

    cycles += def.nCycles;

    switch ( def.mnemonic )
    {
    case InstructionDefinition::MNEMONIC_ILL: {
        throw std::runtime_error("Illegal instruction!");
        break;
    }
    case InstructionDefinition::MNEMONIC_JMP: {
        uint16_t adr = (instr.operand2 << 8) + instr.operand1;
        if ( def.addressing == InstructionDefinition::ADDRESSING_INDIRECT ) {
            uint16_t t = adr;
            // the low byte of the indirect pointer wraps around
            uint16_t t2 = (t & 0xFF00) | (uint8_t((t&0xff)+1));
            adr = readMem8( t ) | (readMem8( t2 ) << 8);
        }
        pc = adr;
        break;
    }
    case InstructionDefinition::MNEMONIC_LDA: {
        uint8_t src = resolveAddressing( instr );
        regA = src;
        updateStatus( regA );
        break;
    }
    case InstructionDefinition::MNEMONIC_LDX: {
        uint8_t src = resolveAddressing( instr );
        regX = src;
        updateStatus( regX );
        break;
    }
    case InstructionDefinition::MNEMONIC_LDY: {
        uint8_t src = resolveAddressing( instr );
        regY = src;
        updateStatus( regY );
        break;
    }
    case InstructionDefinition::MNEMONIC_STA: {
        uint16_t target = resolveWAddressing( instr );
        uint8_t src = regA;
        writeMem8( target, src);
        break;
    }
    case InstructionDefinition::MNEMONIC_STX: {
        uint16_t target = resolveWAddressing( instr );
        uint8_t src = regX;
        writeMem8( target, src );
        break;
    }
    case InstructionDefinition::MNEMONIC_STY: {
        uint16_t target = resolveWAddressing( instr );
        uint8_t src = regY;
        writeMem8( target, src );
        break;
    }
    case InstructionDefinition::MNEMONIC_JSR: {
        // jump to sub routine
        uint16_t adr = (instr.operand2 << 8) + instr.operand1;
        push( pc - 1 );
        pc = adr;
        break;
    }
    case InstructionDefinition::MNEMONIC_RTS: {
        // return from subroutine
        pc = pop() + 1;
        break;
    }
    case InstructionDefinition::MNEMONIC_RTI: {
        // return from interrupt
        status = popByte();
        // no B flag, it is considered 0
        status &= (0xFF-FLAG_B_MASK);
        // bit 5 is always 1
        status |= FLAG_X_MASK;
        pc = pop();
        break;
    }
    case InstructionDefinition::MNEMONIC_BCS: {
        // branch if carry set
        uint16_t adr = pc + (int8_t)(instr.operand1 + 0);
        if ( status & FLAG_C_MASK ) {
            branchTo( adr );
        }
        break;
    }
    case InstructionDefinition::MNEMONIC_BCC: {
        // branch if carry clear
        uint16_t adr = pc + (int8_t)(instr.operand1 + 0);
        if ( (status & FLAG_C_MASK) == 0 ) {
            branchTo( adr );
        }
        break;
    }
    case InstructionDefinition::MNEMONIC_BEQ: {
        // branch if zero
        uint16_t adr = pc + (int8_t)(instr.operand1 + 0);
        if ( status & FLAG_Z_MASK ) {
            branchTo( adr );
        }
        break;
    }
    case InstructionDefinition::MNEMONIC_BNE: {
        // branch if non zero
        uint16_t adr = pc + (int8_t)(instr.operand1 + 0);
        if ( (status & FLAG_Z_MASK) == 0 ) {
            branchTo( adr );
        }
        break;
    }
    case InstructionDefinition::MNEMONIC_BVS: {
        // branch if V set
        uint16_t adr = pc + (int8_t)(instr.operand1 + 0);
        if ( status & FLAG_V_MASK ) {
            branchTo( adr );
        }
        break;
    }
    case InstructionDefinition::MNEMONIC_BVC: {
        // branch if V clear
        uint16_t adr = pc + (int8_t)(instr.operand1 + 0);
        if ( (status & FLAG_V_MASK) == 0 ) {
            branchTo( adr );
        }
        break;
    }
    case InstructionDefinition::MNEMONIC_BPL: {
        // branch if PLUS
        uint16_t adr = pc + (int8_t)(instr.operand1 + 0);
        if ( (status & FLAG_N_MASK) == 0 ) {
            branchTo( adr );
        }
        break;
    }
    case InstructionDefinition::MNEMONIC_BMI: {
        // branch if Minus
        uint16_t adr = pc + (int8_t)(instr.operand1 + 0);
        if ( status & FLAG_N_MASK ) {
            branchTo( adr );
        }
        break;
    }
    case InstructionDefinition::MNEMONIC_SEC: {
        // set carry
        status |= FLAG_C_MASK;
        break;
    }
    case InstructionDefinition::MNEMONIC_CLC: {
        // clear carry
        status &= (0xFF - FLAG_C_MASK);
        break;
    }
    case InstructionDefinition::MNEMONIC_SED: {
        // set decimal flag (used ??)
        status |= FLAG_D_MASK;
        break;
    }
    case InstructionDefinition::MNEMONIC_CLD: {
        // clear decimal flag (used ??)
        status &= (0xFF-FLAG_D_MASK);
        break;
    }
    case InstructionDefinition::MNEMONIC_CLV: {
        // clear overflow flag
        status &= (0xFF-FLAG_V_MASK);
        break;
    }
    case InstructionDefinition::MNEMONIC_SEI: {
        // set interrupt disable
        status |= FLAG_I_MASK;
        break;
    }
    case InstructionDefinition::MNEMONIC_CLI: {
        // set interrupt enable
        status &= (0xFF-FLAG_I_MASK);
        break;
    }
    case InstructionDefinition::MNEMONIC_BIT: {
        // bit test
        uint8_t src = resolveAddressing( instr );
        
        uint8_t r = src & regA;
        updateZStatus( r );
        updateNStatus( src );
        updateVStatus( src );
        break;
    }
    case InstructionDefinition::MNEMONIC_PHP: {
        // push processor flags
        uint8_t st = status;
        // no B flag, it is considered 1
        st |= FLAG_B_MASK;
        // bit 5 is always 1 when pushed
        st |= FLAG_X_MASK;
        pushByte( st );
        break;
    }
    case InstructionDefinition::MNEMONIC_PHA: {
        // push accumulator
        pushByte( regA );
        break;
    }
    case InstructionDefinition::MNEMONIC_PLA: {
        // pull to accumulator
        regA = popByte();
        updateZStatus( regA );
        updateNStatus( regA );
        break;
    }
    case InstructionDefinition::MNEMONIC_PLP: {
        // pull to status
        status = popByte();
        // no B flag, it is considered 0
        status &= (0xFF-FLAG_B_MASK);
        // bit 5 is always 1
        status |= FLAG_X_MASK;
        break;
    }
    case InstructionDefinition::MNEMONIC_AND: {
        instr_and( instr, resolveAddressing( instr ) );
        break;
    }
    case InstructionDefinition::MNEMONIC_ORA: {
        instr_ora( instr, resolveAddressing( instr ) );
        break;
    }
    case InstructionDefinition::MNEMONIC_EOR: {
        instr_eor( instr, resolveAddressing( instr ) );
        break;
    }
    case InstructionDefinition::MNEMONIC_CMP: {
        instr_cmp( instr, resolveAddressing( instr ) );
        break;
    }
    case InstructionDefinition::MNEMONIC_CPX: {
        // CMP regX  operand
        uint8_t mem = resolveAddressing( instr );
        int16_t d = regX - mem;
        if ( d >= 0 ) {
            status |= FLAG_C_MASK;
        }
        else
        {
            status &= (0xFF - FLAG_C_MASK);
        }
        updateZStatus(d);
        updateNStatus(d);
        break;
    }
     case InstructionDefinition::MNEMONIC_CPY: {
        // CMP regY  operand
        uint8_t mem = resolveAddressing( instr );
        int16_t d = regY - mem;
        if ( d >= 0 ) {
            status |= FLAG_C_MASK;
        }
        else
        {
            status &= (0xFF - FLAG_C_MASK);
        }
        updateZStatus(d);
        updateNStatus(d);
        break;
    }
    case InstructionDefinition::MNEMONIC_ADC: {
        instr_adc( instr, resolveAddressing( instr ) );
        break;
    }
    case InstructionDefinition::MNEMONIC_SBC: {
        instr_sbc( instr, resolveAddressing( instr ) );
        break;
    }
    case InstructionDefinition::MNEMONIC_INC: {
        instr_inc( instr );
        break;
    }
    case InstructionDefinition::MNEMONIC_INX: {
        // increment X
        regX = regX + 1;
        updateZStatus( regX );
        updateNStatus( regX );
        break;
    }
    case InstructionDefinition::MNEMONIC_INY: {
        // increment Y
        regY = regY + 1;
        updateZStatus( regY );
        updateNStatus( regY );
        break;
    }
    case InstructionDefinition::MNEMONIC_DEC: {
        instr_dec( instr );
        break;
    }
    case InstructionDefinition::MNEMONIC_DEX: {
        // deccrement X
        regX = regX - 1;
        updateZStatus( regX );
        updateNStatus( regX );
        break;
    }
    case InstructionDefinition::MNEMONIC_DEY: {
        // decrement Y
        regY = regY - 1;
        updateZStatus( regY );
        updateNStatus( regY );
        break;
    }
    case InstructionDefinition::MNEMONIC_TAX: {
        // transfer A to X
        regX = regA;
        updateZStatus( regX );
        updateNStatus( regX );
        break;
    }
    case InstructionDefinition::MNEMONIC_TAY: {
        // transfer A to Y
        regY = regA;
        updateZStatus( regY );
        updateNStatus( regY );
        break;
    }
    case InstructionDefinition::MNEMONIC_TXA: {
        // transfer X to A
        regA = regX;
        updateZStatus( regA );
        updateNStatus( regA );
        break;
    }
    case InstructionDefinition::MNEMONIC_TYA: {
        // transfer Y to A
        regA = regY;
        updateZStatus( regA );
        updateNStatus( regA );
        break;
    }
    case InstructionDefinition::MNEMONIC_TSX: {
        // transfer SP to X
        regX = sp;
        updateZStatus( regX );
        updateNStatus( regX );
        break;
    }
    case InstructionDefinition::MNEMONIC_TXS: {
        // transfer X to SP
        sp = regX;
        break;
    }
    case InstructionDefinition::MNEMONIC_LSR: {
        instr_lsr( instr, def );
        break;
    }
    case InstructionDefinition::MNEMONIC_ASL: {
        instr_asl( instr, def );
        break;
    }
    case InstructionDefinition::MNEMONIC_ROR: {
        instr_ror( instr, def );
        break;
    }
    case InstructionDefinition::MNEMONIC_ROL: {
        instr_rol( instr, def );
        break;
    }
    case InstructionDefinition::MNEMONIC_LAX: {
        // LDX then TXA ??
        uint8_t src = resolveAddressing( instr );
        regA = regX = src;
        updateStatus( regA );
        break;
    }
    case InstructionDefinition::MNEMONIC_SAX: {
        // store A and X
        uint16_t target = resolveWAddressing( instr );
        writeMem8( target, regA & regX );
        break;
    }
    case InstructionDefinition::MNEMONIC_DCP: {
        // DEC then CMP
        instr_cmp( instr, instr_dec( instr ) );
        break;
    }
    case InstructionDefinition::MNEMONIC_ISC: {
        // INC then SBC
        instr_sbc( instr, instr_inc( instr ) );
        break;
    }
    case InstructionDefinition::MNEMONIC_SLO: {
        // ASL then ORA
        instr_ora( instr, instr_asl( instr, def ) );
        break;
    }
    case InstructionDefinition::MNEMONIC_RLA: {
        // ROL then AND
        instr_and( instr, instr_rol( instr, def ) );
        break;
    }
    case InstructionDefinition::MNEMONIC_SRE: {
        // LSR and EOR
        instr_eor( instr, instr_lsr( instr, def ) );
        break;
    }
    case InstructionDefinition::MNEMONIC_RRA: {
        // ROR then ADC
        instr_adc( instr, instr_ror( instr, def ) );
        break;
    }
    case InstructionDefinition::MNEMONIC_NOP: {
        // Do nothing, but resolve addressing (for cycles)
        resolveAddressing( instr );
        break;
    }
    default:
        std::cout << "Not implemented!\n";
        throw NotImplemented();
    }
}
