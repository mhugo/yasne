#include <iostream>
#include <iomanip>
#include "cpu.hpp"

void InstructionDefinition::initTable()
{
    InstructionDefinition* table = InstructionDefinition::table();

    for ( int i = 0; i < 256; i++ ) {
        table[i].opcode = i;
        table[i].valid = false;

        // opcode = aaabbbcc
        int cc = i & 3;
        int aaa = i >> 5;
        int bbb = (i >> 2) & 7;

        std::cout << std::hex << i << " " << (i & 0x0F) << " cc = " << cc << " aaa = " << aaa << std::dec << std::endl;
        if ( (i & 0x0F) == 0 ) { // branching instructions
            table[i].valid = true;
            int xx = i >> 4;
            std::cout << std::hex << xx << std::endl;
            table[i].addressing = ADDRESSING_NONE;
            switch (xx) {
            case 0:
                table[i].mnemonic = "BRK";
                break;
            case 1:
                table[i].mnemonic = "BPL";
                break;
            case 2:
                // JSR abs
                table[i].mnemonic = "JSR";
                break;
            case 3:
                // JMP abs
                table[i].mnemonic = "BMI";
                break;
            case 4:
                table[i].mnemonic = "RTI";
                break;
            case 5:
                table[i].mnemonic = "BVC";
                break;
            case 6:
                table[i].mnemonic = "RTS";
                break;
            case 7:
                table[i].mnemonic = "BVS";
                break;
            case 9:
                table[i].mnemonic = "BCC";
                break;
            case 0xb:
                table[i].mnemonic = "BCS";
                break;
            case 0xd:
                table[i].mnemonic = "BNE";
                break;
            case 0xf:
                table[i].mnemonic = "BEQ";
                break;
            default:
                table[i].valid = false;
            }
        }
        else if ( cc == 1 ) {
            table[i].valid = true;
            switch (aaa) {
            case 0:
                table[i].mnemonic = "ORA";
                break;
            case 1:
                table[i].mnemonic = "AND";
                break;
            case 2:
                table[i].mnemonic = "EOR";
                break;
            case 3:
                table[i].mnemonic = "ADC";
                break;
            case 4:
                table[i].mnemonic = "STA";
                break;
            case 5:
                table[i].mnemonic = "LDA";
                break;
            case 6:
                table[i].mnemonic = "CMP";
                break;
            case 7:
                table[i].mnemonic = "SBC";
                break;
            }

            Addressing adr;
            switch ( bbb ) {
            case 0:
                adr = ADDRESSING_INDIRECT_X;
                break;
            case 1:
                adr = ADDRESSING_ZERO_PAGE;
                break;
            case 2:
                adr = ADDRESSING_IMMEDIATE;
                break;
            case 3:
                adr = ADDRESSING_ABSOLUTE;
                break;
            case 4:
                adr = ADDRESSING_INDIRECT_Y;
                break;
            case 5:
                adr = ADDRESSING_ZERO_PAGE_X;
                break;
            case 6:
                adr = ADDRESSING_ABSOLUTE_X;
                break;
            case 7:
                adr = ADDRESSING_ABSOLUTE_Y;
                break;
            }
            table[i].addressing = adr;
        }
        else if ( cc == 2 ) {
            table[i].valid = true;
            switch (aaa) {
            case 0:
                table[i].mnemonic = "ASL";
                break;
            case 1:
                table[i].mnemonic = "ROL";
                break;
            case 2:
                table[i].mnemonic = "LSR";
                break;
            case 3:
                table[i].mnemonic = "ROR";
                break;
            case 4:
                table[i].mnemonic = "STX";
                break;
            case 5:
                table[i].mnemonic = "LDX";
                break;
            case 6:
                table[i].mnemonic = "DEC";
                break;
            case 7:
                table[i].mnemonic = "INC";
                break;
            }
            Addressing adr;
            switch ( bbb ) {
            case 0:
                adr = ADDRESSING_IMMEDIATE;
                break;
            case 1:
                adr = ADDRESSING_ZERO_PAGE;
                break;
            case 2:
                adr = ADDRESSING_ACCUMULATOR;
                break;
            case 3:
                adr = ADDRESSING_ABSOLUTE;
                break;
            case 5:
                if ( table[i].mnemonic == "STX" ) {
                    adr = ADDRESSING_ZERO_PAGE_Y;
                }
                else {
                    adr = ADDRESSING_ZERO_PAGE_X;
                }
                break;
            case 7:
                if ( table[i].mnemonic == "LDX" ) {
                    adr = ADDRESSING_ABSOLUTE_Y;
                }
                else {
                    adr = ADDRESSING_ABSOLUTE_X;
                }
                break;
            default:
                adr = ADDRESSING_NONE;
                break;
            }
            table[i].addressing = adr;
        }
        else if ( cc == 0 ) {
            table[i].valid = true;
            switch (aaa) {
            case 0:
                table[i].valid = false;
                break;
            case 1:
                table[i].mnemonic = "BIT";
                break;
            case 2:
                table[i].mnemonic = "JMP";
                break;
            case 3:
                // JMP abs
                table[i].mnemonic = "JMP";
                break;
            case 4:
                table[i].mnemonic = "STY";
                break;
            case 5:
                table[i].mnemonic = "LDY";
                break;
            case 6:
                table[i].mnemonic = "CPY";
                break;
            case 7:
                table[i].mnemonic = "CPX";
                break;
            }
            Addressing adr;
            switch ( bbb ) {
            case 0:
                adr = ADDRESSING_IMMEDIATE;
                break;
            case 1:
                adr = ADDRESSING_ZERO_PAGE;
                break;
            case 3:
                adr = ADDRESSING_ABSOLUTE;
                break;
            case 5:
                adr = ADDRESSING_ZERO_PAGE_X;
                break;
            case 7:
                adr = ADDRESSING_ABSOLUTE_X;
                break;
            default:
                adr = ADDRESSING_NONE;
            }
            table[i].addressing = adr;
        }
        
        // else
        if ( ! table[i].valid ) {
            table[i].valid = true;
            table[i].addressing = ADDRESSING_NONE;
            switch (i)
            {
            case 0x08:
                table[i].mnemonic = "PHP";
                break;
            case 0x18:
                table[i].mnemonic = "CLC";
                break;
            case 0x28:
                table[i].mnemonic = "PLP";
                break;
            case 0x38:
                table[i].mnemonic = "SEC";
                break;
            case 0x48:
                table[i].mnemonic = "PHA";
                break;
            case 0x58:
                table[i].mnemonic = "CLI";
                break;
            case 0x68:
                table[i].mnemonic = "PLA";
                break;
            case 0x78:
                table[i].mnemonic = "SEI";
                break;
            case 0x88:
                table[i].mnemonic = "DEY";
                break;
            case 0x98:
                table[i].mnemonic = "TYA";
                break;
            case 0xA8:
                table[i].mnemonic = "TAY";
                break;
            case 0xB8:
                table[i].mnemonic = "CLV";
                break;
            case 0xC8:
                table[i].mnemonic = "INY";
                break;
            case 0xD8:
                table[i].mnemonic = "CLD";
                break;
            case 0xE8:
                table[i].mnemonic = "INX";
                break;
            case 0xF8:
                table[i].mnemonic = "SED";
                break;
            case 0x8A:
                table[i].mnemonic = "TXA";
                break;
            case 0x9A:
                table[i].mnemonic = "TXS";
                break;
            case 0xAA:
                table[i].mnemonic = "TAX";
                break;
            case 0xBA:
                table[i].mnemonic = "TSX";
                break;
            case 0xCA:
                table[i].mnemonic = "DEX";
                break;
            case 0xEA:
                table[i].mnemonic = "NOP";
                break;
            default:
                table[i].valid = false;
            }
        }

        switch ( table[i].addressing )
        {
        case ADDRESSING_NONE:
        case ADDRESSING_ACCUMULATOR:
            table[i].nOperands = 0;
            break;
        case ADDRESSING_IMMEDIATE:
        case ADDRESSING_ZERO_PAGE:
        case ADDRESSING_ZERO_PAGE_X:
        case ADDRESSING_ZERO_PAGE_Y:
            table[i].nOperands = 1;
            break;
        case ADDRESSING_INDIRECT:
        case ADDRESSING_ABSOLUTE:
        case ADDRESSING_ABSOLUTE_X:
        case ADDRESSING_ABSOLUTE_Y:
        case ADDRESSING_INDIRECT_X:
        case ADDRESSING_INDIRECT_Y:
            table[i].nOperands = 2;
            break;
        }
        if ( ! table[i].valid ) {
            table[i].nOperands = 0;
        }
    }
}

Instruction Instruction::decode( const uint8_t* adr )
{
    Instruction instr;
    
    memcpy( &instr.opcode, adr, 1 );
    InstructionDefinition def = InstructionDefinition::table()[ instr.opcode ];
    instr.nOperands = def.nOperands;
    if ( instr.nOperands >= 1 ) {
        memcpy( &instr.operand1, adr + 1, 1 );
        if ( instr.nOperands == 2 ) {
            memcpy( &instr.operand2, adr + 2, 1 );
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

    ostr << def.mnemonic;
    if ( ! def.valid ) {
        ostr << std::dec << "ILLEGAL";
        return ostr;
    }

    switch ( def.addressing )
    {
    case InstructionDefinition::ADDRESSING_IMMEDIATE:
        ostr << "\timm";
        break;
    case InstructionDefinition::ADDRESSING_ZERO_PAGE:
        ostr << "\tzero_page";
        break;
    case InstructionDefinition::ADDRESSING_ACCUMULATOR:
        ostr << "\taccumulator";
        break;
    case InstructionDefinition::ADDRESSING_ABSOLUTE: {
        uint16_t adr = (instr.operand2 << 8) + instr.operand1;
        ostr << "\t$" << std::setfill('0') << std::setw(4) << adr;
        break;
    }
    case InstructionDefinition::ADDRESSING_INDIRECT:
        ostr << "\tindirect";
        break;
    case InstructionDefinition::ADDRESSING_ABSOLUTE_X:
        ostr << "\tabsolute_x";
        break;
    case InstructionDefinition::ADDRESSING_ABSOLUTE_Y:
        ostr << "\tabsolute_y";
        break;
    case InstructionDefinition::ADDRESSING_ZERO_PAGE_X:
        ostr << "\tzero_page_x";
        break;
    case InstructionDefinition::ADDRESSING_ZERO_PAGE_Y:
        ostr << "\tzero_page_y";
        break;
    case InstructionDefinition::ADDRESSING_INDIRECT_X:
        ostr << "\tindirect_x";
        break;
    case InstructionDefinition::ADDRESSING_INDIRECT_Y:
        ostr << "\tzero_page_y";
        break;
    }
    return ostr;
}
