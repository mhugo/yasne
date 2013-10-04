#include <iostream>
#include <iomanip>
#include "cpu.hpp"

const char * InstructionDefinition::MnemonicString[56] = 
{
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
    "CPX"
};

void InstructionDefinition::initTable()
{
    InstructionDefinition* table = InstructionDefinition::table();

    for ( int i = 0; i < 256; i++ ) {
        table[i].opcode = i;

        // opcode = aaabbbcc
        int cc = i & 3;
        int aaa = i >> 5;
        int bbb = (i >> 2) & 7;

        std::cout << std::hex << i << " " << (i & 0x0F) << " cc = " << cc << " aaa = " << aaa << std::dec << std::endl;
        // else

        table[i].valid = true;
        table[i].addressing = ADDRESSING_NONE;
        switch (i)
        {
        case 0x08:
            table[i].mnemonic = MNEMONIC_PHP;
            break;
        case 0x18:
            table[i].mnemonic = MNEMONIC_CLC;
            break;
        case 0x28:
            table[i].mnemonic = MNEMONIC_PLP;
            break;
        case 0x38:
            table[i].mnemonic = MNEMONIC_SEC;
            break;
        case 0x48:
            table[i].mnemonic = MNEMONIC_PHA;
            break;
        case 0x58:
            table[i].mnemonic = MNEMONIC_CLI;
            break;
        case 0x68:
            table[i].mnemonic = MNEMONIC_PLA;
            break;
        case 0x78:
            table[i].mnemonic = MNEMONIC_SEI;
            break;
        case 0x88:
            table[i].mnemonic = MNEMONIC_DEY;
            break;
        case 0x98:
            table[i].mnemonic = MNEMONIC_TYA;
            break;
        case 0xA8:
            table[i].mnemonic = MNEMONIC_TAY;
            break;
        case 0xB8:
            table[i].mnemonic = MNEMONIC_CLV;
            break;
        case 0xC8:
            table[i].mnemonic = MNEMONIC_INY;
            break;
        case 0xD8:
            table[i].mnemonic = MNEMONIC_CLD;
            break;
        case 0xE8:
            table[i].mnemonic = MNEMONIC_INX;
            break;
        case 0xF8:
            table[i].mnemonic = MNEMONIC_SED;
            break;
        case 0x8A:
            table[i].mnemonic = MNEMONIC_TXA;
            break;
        case 0x9A:
            table[i].mnemonic = MNEMONIC_TXS;
            break;
        case 0xAA:
            table[i].mnemonic = MNEMONIC_TAX;
            break;
        case 0xBA:
            table[i].mnemonic = MNEMONIC_TSX;
            break;
        case 0xCA:
            table[i].mnemonic = MNEMONIC_DEX;
            break;
        case 0xEA:
            table[i].mnemonic = MNEMONIC_NOP;
            break;
        default:
            table[i].valid = false;
        }

        if ( !table[i].valid && (i & 0x0F) == 0 ) { // branching instructions
            table[i].valid = true;
            int xx = i >> 4;
            std::cout << std::hex << xx << std::endl;
            table[i].addressing = ADDRESSING_RELATIVE;
            switch (xx) {
            case 0:
                table[i].mnemonic = MNEMONIC_BRK;
                table[i].addressing = ADDRESSING_NONE;
                break;
            case 1:
                table[i].mnemonic = MNEMONIC_BPL;
                break;
            case 2:
                // JSR abs
                table[i].mnemonic = MNEMONIC_JSR;
                table[i].addressing = ADDRESSING_ABSOLUTE;
                break;
            case 3:
                // JMP abs
                table[i].mnemonic = MNEMONIC_BMI;
                break;
            case 4:
                table[i].mnemonic = MNEMONIC_RTI;
                table[i].addressing = ADDRESSING_NONE;
                break;
            case 5:
                table[i].mnemonic = MNEMONIC_BVC;
                break;
            case 6:
                table[i].mnemonic = MNEMONIC_RTS;
                table[i].addressing = ADDRESSING_NONE;
                break;
            case 7:
                table[i].mnemonic = MNEMONIC_BVS;
                break;
            case 9:
                table[i].mnemonic = MNEMONIC_BCC;
                break;
            case 0xb:
                table[i].mnemonic = MNEMONIC_BCS;
                break;
            case 0xd:
                table[i].mnemonic = MNEMONIC_BNE;
                break;
            case 0xf:
                table[i].mnemonic = MNEMONIC_BEQ;
                break;
            default:
                table[i].valid = false;
            }
        }
        else if ( !table[i].valid && cc == 1 ) {
            table[i].valid = true;
            switch (aaa) {
            case 0:
                table[i].mnemonic = MNEMONIC_ORA;
                break;
            case 1:
                table[i].mnemonic = MNEMONIC_AND;
                break;
            case 2:
                table[i].mnemonic = MNEMONIC_EOR;
                break;
            case 3:
                table[i].mnemonic = MNEMONIC_ADC;
                break;
            case 4:
                table[i].mnemonic = MNEMONIC_STA;
                break;
            case 5:
                table[i].mnemonic = MNEMONIC_LDA;
                break;
            case 6:
                table[i].mnemonic = MNEMONIC_CMP;
                break;
            case 7:
                table[i].mnemonic = MNEMONIC_SBC;
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
        else if ( !table[i].valid && cc == 2 ) {
            table[i].valid = true;
            switch (aaa) {
            case 0:
                table[i].mnemonic = MNEMONIC_ASL;
                break;
            case 1:
                table[i].mnemonic = MNEMONIC_ROL;
                break;
            case 2:
                table[i].mnemonic = MNEMONIC_LSR;
                break;
            case 3:
                table[i].mnemonic = MNEMONIC_ROR;
                break;
            case 4:
                table[i].mnemonic = MNEMONIC_STX;
                break;
            case 5:
                table[i].mnemonic = MNEMONIC_LDX;
                break;
            case 6:
                table[i].mnemonic = MNEMONIC_DEC;
                break;
            case 7:
                table[i].mnemonic = MNEMONIC_INC;
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
                if ( table[i].mnemonic == MNEMONIC_STX ) {
                    adr = ADDRESSING_ZERO_PAGE_Y;
                }
                else {
                    adr = ADDRESSING_ZERO_PAGE_X;
                }
                break;
            case 7:
                if ( table[i].mnemonic == MNEMONIC_LDX ) {
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
        else if ( !table[i].valid && cc == 0 ) {
            table[i].valid = true;
            switch (aaa) {
            case 0:
                table[i].valid = false;
                break;
            case 1:
                table[i].mnemonic = MNEMONIC_BIT;
                break;
            case 2:
                table[i].mnemonic = MNEMONIC_JMP;
                break;
            case 3:
                // JMP abs
                table[i].mnemonic = MNEMONIC_JMP;
                break;
            case 4:
                table[i].mnemonic = MNEMONIC_STY;
                break;
            case 5:
                table[i].mnemonic = MNEMONIC_LDY;
                break;
            case 6:
                table[i].mnemonic = MNEMONIC_CPY;
                break;
            case 7:
                table[i].mnemonic = MNEMONIC_CPX;
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
        ostr << "\taccumulator";
        break;
    case InstructionDefinition::ADDRESSING_RELATIVE: {
        int8_t rel = instr.operand1;
        ostr << "\t" << std::setfill('0') << std::setw(2) << (rel+0);
        break;
    }
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

uint8_t CPU::resolveAddressing( const Instruction& instr )
{
    InstructionDefinition def = InstructionDefinition::table()[ instr.opcode ];

    switch ( def.addressing )
    {
    case InstructionDefinition::ADDRESSING_IMMEDIATE:
        return instr.operand1;
        break;
    case InstructionDefinition::ADDRESSING_ZERO_PAGE:
        return memory[ instr.operand1 ];
        break;
    }
}

uint8_t* CPU::resolveWAddressing( const Instruction& instr )
{
    InstructionDefinition def = InstructionDefinition::table()[ instr.opcode ];

    switch ( def.addressing )
    {
    case InstructionDefinition::ADDRESSING_ZERO_PAGE:
        return memory + instr.operand1;
        break;
    }
    return 0;
}

void CPU::transfer( uint8_t* target, uint8_t src )
{
    *target = src;
}


void CPU::push( uint16_t v )
{
    sp -= 2;
    memcpy(memory + sp, &v, 2 );
}

void CPU::pushByte( uint8_t v )
{
    sp--;
    memcpy(memory + sp, &v, 1 );
}

uint16_t CPU::pop()
{
    uint16_t v;
    memcpy( &v, memory + sp, 2 );
    if ( sp < 0xFF ) {
        sp += 2;
    }
    return v;
}

uint8_t CPU::popByte()
{
    uint8_t v;
    memcpy( &v, memory + sp, 1 );
    if ( sp < 0xFF ) {
        sp ++;
    }
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

void CPU::execute( const Instruction& instr )
{
    InstructionDefinition def = InstructionDefinition::table()[ instr.opcode ];

    switch ( def.mnemonic )
    {
    case InstructionDefinition::MNEMONIC_JMP: {
        uint16_t adr = (instr.operand2 << 8) + instr.operand1;
        pc = adr;
        break;
    }
    case InstructionDefinition::MNEMONIC_LDA: {
        uint8_t src = resolveAddressing( instr );
        uint8_t *target = &regA;
        transfer( target, src );
        updateStatus( *target );
        break;
    }
    case InstructionDefinition::MNEMONIC_LDX: {
        uint8_t src = resolveAddressing( instr );
        uint8_t *target = &regX;
        transfer( target, src );
        updateStatus( *target );
        break;
    }
    case InstructionDefinition::MNEMONIC_LDY: {
        uint8_t src = resolveAddressing( instr );
        uint8_t *target = &regY;
        transfer( target, src );
        updateStatus( *target );
        break;
    }
    case InstructionDefinition::MNEMONIC_STA: {
        uint8_t *target = resolveWAddressing( instr );
        uint8_t src = regA;
        transfer( target, src );
        break;
    }
    case InstructionDefinition::MNEMONIC_STX: {
        uint8_t *target = resolveWAddressing( instr );
        uint8_t src = regX;
        transfer( target, src );
        break;
    }
    case InstructionDefinition::MNEMONIC_STY: {
        uint8_t *target = resolveWAddressing( instr );
        uint8_t src = regY;
        transfer( target, src );
        break;
    }
    case InstructionDefinition::MNEMONIC_JSR: {
        // jump to sub routine
        uint16_t adr = (instr.operand2 << 8) + instr.operand1;
        push( pc );
        pc = adr;
        break;
    }
    case InstructionDefinition::MNEMONIC_RTS: {
        // return from subroutine
        pc = pop();
        break;
    }
    case InstructionDefinition::MNEMONIC_BCS: {
        // branch if carry set
        uint16_t adr = pc + (int8_t)(instr.operand1 + 0);
        if ( status & FLAG_C_MASK ) {
            pc = adr;
        }
        break;
    }
    case InstructionDefinition::MNEMONIC_BCC: {
        // branch if carry clear
        uint16_t adr = pc + (int8_t)(instr.operand1 + 0);
        if ( (status & FLAG_C_MASK) == 0 ) {
            pc = adr;
        }
        break;
    }
    case InstructionDefinition::MNEMONIC_BEQ: {
        // branch if zero
        uint16_t adr = pc + (int8_t)(instr.operand1 + 0);
        if ( status & FLAG_Z_MASK ) {
            pc = adr;
        }
        break;
    }
    case InstructionDefinition::MNEMONIC_BNE: {
        // branch if non zero
        uint16_t adr = pc + (int8_t)(instr.operand1 + 0);
        if ( (status & FLAG_Z_MASK) == 0 ) {
            pc = adr;
        }
        break;
    }
    case InstructionDefinition::MNEMONIC_BVS: {
        // branch if V set
        uint16_t adr = pc + (int8_t)(instr.operand1 + 0);
        if ( status & FLAG_V_MASK ) {
            pc = adr;
        }
        break;
    }
    case InstructionDefinition::MNEMONIC_BVC: {
        // branch if V clear
        uint16_t adr = pc + (int8_t)(instr.operand1 + 0);
        if ( (status & FLAG_V_MASK) == 0 ) {
            pc = adr;
        }
        break;
    }
    case InstructionDefinition::MNEMONIC_BPL: {
        // branch if PLUS
        uint16_t adr = pc + (int8_t)(instr.operand1 + 0);
        if ( (status & FLAG_N_MASK) == 0 ) {
            pc = adr;
        }
        break;
    }
    case InstructionDefinition::MNEMONIC_BMI: {
        // branch if Minus
        uint16_t adr = pc + (int8_t)(instr.operand1 + 0);
        if ( status & FLAG_N_MASK ) {
            pc = adr;
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
        std::cout << "= " << (src+0) << std::endl;
        
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
        pushByte( st );
        break;
    }
    case InstructionDefinition::MNEMONIC_PLA: {
        // pull to accumulator
        regA = popByte();
        updateZStatus( regA );
        updateNStatus( regA );
        break;
    }
    case InstructionDefinition::MNEMONIC_AND: {
        // AND regA & operand
        uint8_t mem = resolveAddressing( instr );
        std::cout << "= " << (mem+0) << std::endl;
        regA = regA & mem;
        updateZStatus(regA);
        updateNStatus(regA);
        break;
    }
    case InstructionDefinition::MNEMONIC_CMP: {
        // CMP regA  operand
        uint8_t mem = resolveAddressing( instr );
        std::cout << "= " << (mem+0) << std::endl;
        uint8_t d = regA - mem;
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
    }
}
