#ifndef P6502_H
#define P6502_H

class P6502 {
    public:
	P6502();
	~P6502();
    
    public:
	void step();

    private:
	// Addressing modes
	
	// Operations

    private:
	Bus m_bus;

};

#endif
