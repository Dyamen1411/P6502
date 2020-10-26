#ifndef P6502_H
#define P6502_H

#include "bus.h"
#include "opcodes.h"

#define RESET_VECTOR 0xFFFC
#define IRQ_VECTOR 0xFFFE

class P6502 {
    public:
	P6502();
	~P6502();
    
    public:
        typedef void (P6502::*function)();
	void step();

    private:
	void initializeRegisters();

	// Addressing modes
	void initializeAddressingModeTable();
	void am_ACC();
	void am_ABS();
	void am_ABX();
	void am_ABY();
	void am_IMM();
	void am_IMP();
	void am_IND();
	void am_INX();
	void am_INY();
	void am_REL();
	void am_ZPG();
	void am_ZPX();
	void am_ZPY();
	
	// Operations
	void initializeOperationTable();
	void op_ADC();
	void op_AND();
	void op_ASL();
	void op_BCC();
	void op_BCS();
	void op_BEQ();
	void op_BIT();
	void op_BMI();
	void op_BNE();
	void op_BPL();
	void op_BRK();
	void op_BVC();
	void op_BVS();
	void op_CLC();
	void op_CLD();
	void op_CLI();
	void op_CLV();
	void op_CMP();
	void op_CPX();
	void op_CPY();
	void op_DEC();
	void op_DEX();
	void op_DEY();
	void op_EOR();
	void op_INC();
	void op_INX();
	void op_INY();
	void op_JMP();
	void op_JSR();
	void op_LDA();
	void op_LDX();
	void op_LDY();
	void op_LSR();
	void op_NOP();
	void op_ORA();
	void op_PHA();
	void op_PHP();
	void op_PLA();
	void op_PLP();
	void op_ROL();
	void op_ROR();
	void op_RTI();
	void op_RTS();
	void op_SBC();
	void op_SEC();
	void op_SED();
	void op_SEI();
	void op_STA();
	void op_STX();
	void op_STY();
	void op_TAX();
	void op_TAY();
	void op_TSX();
	void op_TXA();
	void op_TXS();
	void op_TYA();

	void initializeDecodeTable();
	
	// Returns 1 if can't write stack
	BYTE pushOnStack(const BYTE &data);
	BYTE pullFromStack();

    private:
	Bus * m_bus;

	function * m_addressing_modes; // 13
	function * m_operations; // 55
	
	BYTE * m_decode_table;
	
	WORD m_operand;
	BYTE m_is_raw_value; // 0 if operand is an address
	BYTE m_is_accumulator; // 1 is result must be stored in the accumulator

	BYTE m_extra_cycles;

	BYTE m_a;
	BYTE m_x;
	BYTE m_y;

	BYTE m_stack_pointer;
	BYTE m_status_register;

	WORD m_program_counter;
};

#endif
