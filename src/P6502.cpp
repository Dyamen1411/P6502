#include "opcodes.h"
#include "P6502.h"

#define A(NAME) (&P6502::am_##NAME)
#define FA(NAME) void P6502::am_##NAME()
#define FO(NAME) void P6502::op_##NAME()

P6502::P6502() {
    m_bus = new Bus();
    initializeAddressingModes();
    initializeOperations();
    initializeDecodeTable();
    
    m_program_counter = 0;
    resetProgramCounter();
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
    BYTE cycles = m_decode_table[op+0];
    (this->*m_addressing_modes[m_decode_table[op+1]])();
    (this->*m_operations[m_decode_table[op+2]])();
    cycles += m_extra_cycles;

    // wait $(cycles) cycles
}

void P6502::initializeAddressingModes() {
    m_addressing_modes = new function[13] {
	A(ACC), A(ABS), A(ABX), A(ABY), A(IMM), A(IMP), 
	A(IND), A(INX), A(INY), A(REL), A(ZPG), A(ZPX),
	A(ZPY),
    };
}

void P6502::initializeOperations() {

}

void P6502::initializeDecodeTable() {

}

void P6502::resetProgramCounter() {
    m_program_counter = (WORD) (m_bus->read(RESET_VECTOR) | (m_bus->read(RESET_VECTOR + 1)));
}

// addressing modes
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

FA(IMP) {
    m_operand = 0;
}

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
