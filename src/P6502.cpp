#include "opcodes.h"
#include "P6502.h"

#define A(NAME) (&P6502::am_##NAME)
#define O(NAME) (&P6502::op_##NAME)
#define FA(NAME) void P6502::am_##NAME()
#define FO(NAME) void P6502::op_##NAME()

P6502::P6502() {
    m_bus = new Bus();
    initializeAddressingModeTable();
    initializeOperationTable();
    initializeDecodeTable();
    
    initializeRegisters();
}

P6502::~P6502() {
    delete m_bus;
    delete[] m_addressing_modes;
    delete[] m_operations;
    delete[] m_decode_table;
}

void P6502::step() {
    m_extra_cycles = 0;
    WORD op = m_bus->read(++m_program_counter) * 3;
    BYTE attributes = m_decode_table[op+0];
    BYTE cycles = (BYTE) (attributes & 0xF);
    m_is_raw_value = (BYTE) ((attributes & 0x7F) >> 4);
    m_is_accumulator = (BYTE) (attributes >> 7);
    (this->*m_addressing_modes[m_decode_table[op+1]])();
    (this->*m_operations[m_decode_table[op+2]])();
    cycles += m_extra_cycles;

    // wait $(cycles) cycles
}

void P6502::initializeAddressingModeTable() {
    m_addressing_modes = new function[13] {
	A(ACC),A(ABS),A(ABX),A(ABY),A(IMM),A(IMP), 
	A(IND),A(INX),A(INY),A(REL),A(ZPG),A(ZPX),
	A(ZPY),
    };
}

void P6502::initializeOperationTable() {
    m_operations = new function[55] {
	O(ADC),O(AND),O(ASL),O(BCC),O(BCS),O(BEQ),
	O(BIT),O(BNE),O(BPL),O(BRK),O(BVC),O(BVS),
	O(CLC),O(CLD),O(CLI),O(CLV),O(CMP),O(CPX),
	O(CPY),O(DEC),O(DEX),O(DEY),O(EOR),O(INC),
	O(INX),O(INY),O(JMP),O(JSR),O(LDA),O(LDX),
	O(LDY),O(LSR),O(NOP),O(ORA),O(PHA),O(PHP),
	O(PLA),O(PLP),O(ROL),O(ROR),O(RTI),O(RTS),
	O(SBC),O(SEC),O(SED),O(SEI),O(STA),O(STX),
	O(STY),O(TAX),O(TAY),O(TSX),O(TXA),O(TXS),
	O(TYA),
    };
}

void P6502::initializeDecodeTable() {
    
}

void P6502::initializeRegisters() {
    m_a = 0;
    m_x = 0;
    m_y = 0;
    m_stack_pointer = 0xFF;
    m_status_register = 0xff;
    m_program_counter = (WORD) (m_bus->read(RESET_VECTOR) | (m_bus->read(RESET_VECTOR + 1)));
}

// Returns 1 if can't write to stack
BYTE P6502::pushOnStack(const BYTE &data) {
    --m_stack_pointer; 
    if (m_bus->write(m_stack_pointer+1, data)) {
	return 1;
    }
    return 0;
}

BYTE P6502::pullFromStack() {
    ++m_stack_pointer;
    return m_bus->read(m_stack_pointer+1);
}

// Addressing modes
FA(ACC) {
    m_operand = m_a & 0xFF;
    ++m_program_counter;
}

FA(ABS) {
    WORD effective_address = (WORD) (m_bus->read(m_program_counter) | (m_bus->read(m_program_counter + 1) << 8));
    m_operand = effective_address;
    m_program_counter += 2;
}

FA(ABX) {
    WORD effective_address = (WORD) (m_bus->read(m_program_counter) | (m_bus->read(m_program_counter + 1) << 8));
    effective_address += m_x;
    if ((effective_address & 0xFF) < m_x) {
	++m_extra_cycles;
    }
    m_operand = effective_address;
    m_program_counter += 2;
}

FA(ABY) {
    WORD effective_address = (WORD) (m_bus->read(m_program_counter) | (m_bus->read(m_program_counter + 1) << 8));
    effective_address += m_y;
    if ((effective_address & 0xFF) < m_y) {
	++m_extra_cycles;
    }
    m_operand = effective_address;
    m_program_counter += 2;
}

FA(IMM) {
    m_operand = (WORD) (m_bus->read(m_program_counter) & 0xFF);
    ++m_program_counter;
}

FA(IMP) {}

FA(IND) {
    WORD address = (WORD) (m_bus->read(m_program_counter) | (m_bus->read(m_program_counter + 1) << 8));
    WORD effective_address = (WORD) (m_bus->read(address) | (m_bus->read(address + 1) << 8));
    m_operand = effective_address;
    m_program_counter += 2;
}

FA(INX) {
    WORD address = (WORD) ((m_bus->read(m_program_counter) + m_x) & 0xFF);
    WORD effective_address = (WORD) (m_bus->read(address) | (m_bus->read(address + 1) << 8));
    m_operand = effective_address;
    ++m_program_counter;
}

FA(INY) {
    WORD address = (WORD) (m_bus->read(m_program_counter)& 0xFF);
    WORD effective_address = (WORD) ((m_bus->read(address) | (m_bus->read(address + 1) << 8)) + m_y);
    m_operand = effective_address;
    ++m_program_counter;
}

FA(REL) {
    WORD pointer_offset = (WORD) (m_bus->read(m_program_counter) & 0xFF);
    m_operand = pointer_offset;
    ++m_program_counter;
}

FA(ZPG) {
    WORD effective_address = (WORD) (m_bus->read(m_program_counter) & 0xFF);
    m_operand = effective_address;
    ++m_program_counter;
}

FA(ZPX) {
    WORD effective_address = (WORD) ((m_bus->read(m_program_counter) + m_x) & 0xFF);
    m_operand = effective_address;
    ++m_program_counter;   
}

FA(ZPY) {
    WORD effective_address = (WORD) ((m_bus->read(m_program_counter) + m_y) & 0xFF);
    m_operand = effective_address;
    ++m_program_counter;
}

// Operations
FO(ADC) {
    BYTE b = m_is_raw_value ? (BYTE) (m_operand & 0xFF) : m_bus->read(m_operand);
    WORD result = (WORD) (m_a + b + (m_status_register & 1));
    m_a = (BYTE) (result & 0xFF);

    m_status_register &= 0b00111100; // N V Z C
    m_status_register |= (BYTE) ((result & 0x100) >> 8); // C
    m_status_register |= (BYTE) (result & 0x80); // N
    m_status_register |= (BYTE) (((m_a ^ result) & (b ^ result) & 0x80) >> 1); // V
    m_status_register |= (BYTE) (result == 0 ? 2 : 0); // Z
}

FO(AND) {
    BYTE b = m_is_raw_value ? (BYTE) (m_operand & 0xFF) : m_bus->read(m_operand);
    BYTE result = (BYTE) (m_a & b);
    m_a = result;

    m_status_register &= 0b01111101; // N Z
    m_status_register |= (BYTE) (result & 0x80); // N
    m_status_register |= (BYTE) (result == 0 ? 2 : 0); // Z
}

FO(ASL) {
    BYTE b = m_is_raw_value ? (BYTE) (m_operand & 0xFF) : m_bus->read(m_operand);
    BYTE result = (BYTE) (b << 1);
    if (m_is_accumulator) {
	m_a = result;
    } else if(m_bus->write(m_operand, result)) {
	    // TODO: Handle error !
    }

    m_status_register &= 0b01111100; // N Z C
    m_status_register |= (BYTE) ((result & 0x100) >> 8); // C
    m_status_register |= (BYTE) (result & 0x80); // N
    m_status_register |= (BYTE) (result == 0 ? 2 : 0); // Z
}

FO(BCC) {
    BYTE b = (BYTE) m_operand;
    if (~m_status_register & 0x01) {
	WORD test = (WORD) (b + (m_program_counter & 0xFF));
	if (test & 0x100) {
	    ++m_extra_cycles;
	}
	m_program_counter += (WORD) (((b & 0x80) ? 0xFF00 : 0x0000) | b);
    }
}

FO(BCS) {
    BYTE b = (BYTE) m_operand;
    if (m_status_register & 0x01) {
	WORD test = (WORD) (b + (m_program_counter & 0xFF));
	if (test & 0x100) {
	    ++m_extra_cycles;
	}
	m_program_counter += (WORD) (((b & 0x80) ? 0xFF00 : 0x0000) | b);
    }
}

FO(BEQ) {
    BYTE b = (BYTE) m_operand;
    if (~m_status_register & 0x02) {
	WORD test = (WORD) (b + (m_program_counter & 0xFF));
	if (test & 0x100) {
	    ++m_extra_cycles;
	}
	m_program_counter += (WORD) (((b & 0x80) ? 0xFF00 : 0x0000) | b);
    }
}

FO(BIT) {
    BYTE b = m_bus->read(m_operand);

    m_status_register &= 0b00111101; // N V Z
    m_status_register |= b & 0xC0; // N V
    m_status_register |= (BYTE) ((b & m_a) == 0 ? 2 : 0); // Z
}

FO(BMI) {
    BYTE b = (BYTE) m_operand;
    if (m_status_register & 0x80) {
	WORD test = (WORD) (b + (m_program_counter & 0xFF));
	if (test & 0x100) {
	    ++m_extra_cycles;
	}
	m_program_counter += (WORD) (((b & 0x80) ? 0xFF00 : 0x0000) | b);
    }
}

FO(BNE) {
    BYTE b = (BYTE) m_operand;
    if (m_status_register & 0x02) {
	WORD test = (WORD) (b + (m_program_counter & 0xFF));
	if (test & 0x100) {
	    ++m_extra_cycles;
	}
	m_program_counter += (WORD) (((b & 0x80) ? 0xFF00 : 0x0000) | b);
    }
}

FO(BPL) {
    BYTE b = (BYTE) m_operand;
    if (~m_status_register & 0x80) {
	WORD test = (WORD) (b + (m_program_counter & 0xFF));
	if (test & 0x100) {
	    ++m_extra_cycles;
	}
	m_program_counter += (WORD) (((b & 0x80) ? 0xFF00 : 0x0000) | b);
    }
}

FO(BRK) {
    ++m_program_counter;
    if(pushOnStack((BYTE) (m_program_counter & 0xFF00) >> 8)
	    | pushOnStack((BYTE) (m_program_counter & 0xFF))
	    | pushOnStack(m_status_register)) {
	// TODO: Handle error !
    }

    m_program_counter = (WORD) (m_bus->read(IRQ_VECTOR) | (m_bus->read(IRQ_VECTOR + 1) << 8));
}

FO(BVC) {
    BYTE b = (BYTE) m_operand;
    if (~m_status_register & 0x40) {
	WORD test = (WORD) (b + (m_program_counter & 0xFF));
	if (test & 0x100) {
	    ++m_extra_cycles;
	}
	m_program_counter += (WORD) (((b & 0x80) ? 0xFF00 : 0x0000) | b);
    }
}

FO(BVS) {
    BYTE b = (BYTE) m_operand;
    if (m_status_register & 0x40) {
	WORD test = (WORD) (b + (m_program_counter & 0xFF));
	if (test & 0x100) {
	    ++m_extra_cycles;
	}
	m_program_counter += (WORD) (((b & 0x80) ? 0xFF00 : 0x0000) | b);
    }
}

FO(CLC) {
    m_status_register &= 0b11111110; // C
}

FO(CLD) {
    m_status_register &= 0b11110111; // D
}

FO(CLI) {
    m_status_register &= 0b11111011; // I
}

FO(CLV) {
    m_status_register &= 0b10111111; // V
}

FO(CMP) {
    BYTE b = (BYTE) (m_is_raw_value ? m_operand : m_bus->read(m_operand));
    BYTE test = m_a - b;

    m_status_register &= 0b01111100; // N Z C
    m_status_register |= (BYTE) ((test & 0x100) >> 8); // C
    m_status_register |= (BYTE) (test & 0x80); // N
    m_status_register |= (BYTE) (test == 0 ? 2 : 0); // Z   
}

FO(CPX) {
    BYTE b = (BYTE) (m_is_raw_value ? m_operand : m_bus->read(m_operand));
    BYTE test = m_x - b;

    m_status_register &= 0b01111100; // N Z C
    m_status_register |= (BYTE) ((test & 0x100) >> 8); // C
    m_status_register |= (BYTE) (test & 0x80); // N
    m_status_register |= (BYTE) (test == 0 ? 2 : 0); // Z   

}

FO(CPY) {
    BYTE b = (BYTE) (m_is_raw_value ? m_operand : m_bus->read(m_operand));
    BYTE test = m_y - b;

    m_status_register &= 0b01111100; // N Z C
    m_status_register |= (BYTE) ((test & 0x100) >> 8); // C
    m_status_register |= (BYTE) (test & 0x80); // N
    m_status_register |= (BYTE) (test == 0 ? 2 : 0); // Z
}

FO(DEC) {
    BYTE value = m_bus->read(m_operand);
    --value;
    if(m_bus->write(m_operand, value)) {
	// TODO: Handle error !
    }

    m_status_register &= 0b01111101;
    m_status_register |= (BYTE) (value & 0x80); // N
    m_status_register |= (BYTE) (value == 0 ? 2 : 0); // Z
}

FO(DEX) {
    --m_x;

    m_status_register &= 0b01111101;
    m_status_register |= (BYTE) (m_x & 0x80); // N
    m_status_register |= (BYTE) (m_x == 0 ? 2 : 0); // Z
}

FO(DEY) {
    --m_y;

    m_status_register &= 0b01111101;
    m_status_register |= (BYTE) (m_y & 0x80); // N
    m_status_register |= (BYTE) (m_y == 0 ? 2 : 0); // Z
}

FO(EOR) {
    BYTE b = (BYTE) (m_is_raw_value ? m_operand : m_bus->read(m_operand));
    m_a ^= b;

    m_status_register &= 0b01111101;
    m_status_register |= (BYTE) (m_a & 0x80); // N
    m_status_register |= (BYTE) (m_a == 0 ? 2 : 0); // Z
}

FO(INC) {
    BYTE value = m_bus->read(m_operand);
    ++value;
    if(m_bus->write(m_operand, value)) {
	// TODO: Handle error !
    }

    m_status_register &= 0b01111101;
    m_status_register |= (BYTE) (value & 0x80); // N
    m_status_register |= (BYTE) (value == 0 ? 2 : 0); // Z
}

FO(INX) {
    ++m_x;

    m_status_register &= 0b01111101;
    m_status_register |= (BYTE) (m_x & 0x80); // N
    m_status_register |= (BYTE) (m_x == 0 ? 2 : 0); // Z
}

FO(INY) {
    ++m_y;

    m_status_register &= 0b01111101;
    m_status_register |= (BYTE) (m_y & 0x80); // N
    m_status_register |= (BYTE) (m_y == 0 ? 2 : 0); // Z
}

FO(JMP) {
    m_program_counter = m_operand;
}

FO(JSR) {
    BYTE hi = (BYTE) ((m_program_counter & 0xFF00) >> 8);
    BYTE lo = (BYTE) (m_program_counter & 0xFF);
    
    if (pushOnStack(hi) | pushOnStack(lo)) {
	// TODO: Handle error !
    }

    m_program_counter = m_operand;
}

FO(LDA) {
    BYTE value = (BYTE) (m_is_raw_value ? m_operand : m_bus->read(m_operand));
    m_a = value;

    m_status_register &= 0b01111101; // N Z
    m_status_register |= (BYTE) (value & 0x80); // N
    m_status_register |= (BYTE) (value == 0 ? 2 : 0); // Z
}

FO(LDX) {
    BYTE value = (BYTE) (m_is_raw_value ? m_operand : m_bus->read(m_operand));
    m_x = value;

    m_status_register &= 0b01111101; // N Z
    m_status_register |= (BYTE) (value & 0x80); // N
    m_status_register |= (BYTE) (value == 0 ? 2 : 0); // Z
}

FO(LDY) {
    BYTE value = (BYTE) (m_is_raw_value ? m_operand : m_bus->read(m_operand));
    m_y = value;

    m_status_register &= 0b01111101; // N Z
    m_status_register |= (BYTE) (value & 0x80); // N
    m_status_register |= (BYTE) (value == 0 ? 2 : 0); // Z
}

FO(LSR) {
    BYTE b = m_is_raw_value ? (BYTE) (m_operand & 0xFF) : m_bus->read(m_operand);
    BYTE result = (BYTE) (b >> 1);
    if (m_is_accumulator) {
	m_a = result;
    } else if(m_bus->write(m_operand, result)) {
	    // TODO: Handle error !
    }

    m_status_register &= 0b01111100; // N Z C
    m_status_register |= (BYTE) ((result & 0x100) >> 8); // C
    m_status_register |= (BYTE) (result & 0x80); // N
    m_status_register |= (BYTE) (result == 0 ? 2 : 0); // Z
}

FO(NOP) {}

FO(ORA) {
    BYTE b = (BYTE) (m_is_raw_value ? m_operand : m_bus->read(m_operand));
    m_a |= b;
    m_status_register &= 0b01111101;
    m_status_register |= (BYTE) (m_a & 0x80); // N
    m_status_register |= (BYTE) (m_a == 0 ? 2 : 0); // Z
}

FO(PHA) {
    if (pushOnStack(m_a)) {
	// TODO: Handle exception !
    }
}

FO(PHP) {
    if (pushOnStack(m_status_register)) {
	// TODO: Handle exception !
    }
}

FO(PLA) {
    m_a = pullFromStack();

    m_status_register &= 0b01111101; // N Z
    m_status_register |= (BYTE) (m_a & 0x80); // N
    m_status_register |= (BYTE) (m_a == 0 ? 2 : 0); // Z
}

FO(PLP) {
    m_status_register = pullFromStack();
}

FO(ROL) {
    BYTE value = m_bus->read(m_operand);
    value = (BYTE) ((value << 1) | ((value >> 7) & 1));
    if (m_bus->write(m_operand, value)) {
	// TODO: Handle exeption !
    }

    m_status_register &= 0b01111100; // N Z C
    m_status_register |= (BYTE) (m_a & 0x80); // N
    m_status_register |= (BYTE) (m_a == 0 ? 2 : 0); // Z
    m_status_register |= (BYTE) (value & 1); // C
}

FO(ROR) {
    BYTE value = m_bus->read(m_operand);
    value = (BYTE) ((value >> 1) | ((value << 7) & 0x80));
    if (m_bus->write(m_operand, value)) {
	// TODO: Handle exeption !
    }

    m_status_register &= 0b01111100; // N Z C
    m_status_register |= (BYTE) (m_a & 0x80); // N
    m_status_register |= (BYTE) (m_a == 0 ? 2 : 0); // Z
    m_status_register |= (BYTE) (value & 1); // C
}

FO(RTI) {
    m_status_register = pullFromStack();
    BYTE address_low_byte = pullFromStack();
    BYTE address_high_byte = pullFromStack();

    m_program_counter = (WORD) (address_low_byte | ((address_high_byte << 8) & 0xFF00));
}

FO(RTS) {
    BYTE address_low_byte = pullFromStack();
    BYTE address_high_byte = pullFromStack();

    m_program_counter = (WORD) (address_low_byte | ((address_high_byte << 8) & 0xFF00));
}

FO(SBC) {
    BYTE b = m_is_raw_value ? (BYTE) (m_operand & 0xFF) : m_bus->read(m_operand);
    WORD result = (WORD) (m_a - b + 1 + ~(m_status_register & 1));
    m_a = (BYTE) (result & 0xFF);

    m_status_register &= 0b00111100; // N V Z C
    m_status_register |= (BYTE) ((result & 0x100) >> 8); // C
    m_status_register |= (BYTE) (result & 0x80); // N
    m_status_register |= (BYTE) (((m_a ^ result) & (b ^ result) & 0x80) >> 1); // V
    m_status_register |= (BYTE) (result == 0 ? 2 : 0); // Z
}

FO(SEC) {
    m_status_register |= 0x01;
}

FO(SED) {
    m_status_register |= 0x08;
}

FO(SEI) {
    m_status_register |= 0x04;
}

FO(STA) {
    if (m_bus->write(m_operand, m_a)) {
	// TODO: Handle exception !
    }
}

FO(STX) {
    if (m_bus->write(m_operand, m_x)) {
	// TODO: Handle exception !
    }
}

FO(STY) {
    if (m_bus->write(m_operand, m_y)) {
	// TODO: Handle exception !
    }
}

FO(TAX) {
    m_x = m_a;
}

FO(TAY) {
    m_y = m_a;
}

FO(TSX) {
    m_x = m_status_register;
}

FO(TXA) {
    m_x = m_a;
}

FO(TXS) {
    m_status_register = m_x;
}

FO(TYA) {
    m_a = m_y;
}
