#ifndef BUS_H
#define BUS_H

#include "device_interface.h"
#include "P6502_utils.h"

#define MAX_DEVICES 10

class Bus {
    public:
	Bus();
	~Bus();

	// Returns 1 if device range of addresses opverlaps with one of the current connected device	
	BYTE connectDevice(DeviceInterface * device);
	// Returns 0 if no device is connected at the given address
	BYTE read(const WORD &address) const;
	// Returns 1 if no device is connected at the given address
	BYTE write(const WORD &address, const BYTE &data) const;
    
    private:
	DeviceInterface * getDevice(const WORD &addresss) const;

	DeviceInterface * m_device_interfaces[MAX_DEVICES];
	WORD m_device_interface_index;
};

#endif
