#ifndef DEVICE_INTERFACE_H
#define DEVICE_INTERFACE_H

#include "P6502_utils.h"

typedef BYTE (*device_function_pointer)(const WORD &, const BYTE &);

class DeviceInterface {
    public:
	DeviceInterface(device_function_pointer write, device_function_pointer read, 
		const WORD &low_range, const WORD &high_range);
	~DeviceInterface();
	
	// Returns 1 if the device as access to this address
	BYTE isInRange(const WORD &address) const;
	// Returns one if both devices can access the same addresses
	BYTE overlapsWithDevice(DeviceInterface * other) const;
	
	BYTE read(const WORD &address) const;
	BYTE write(const WORD &address, const BYTE &data) const;

    private:
	device_function_pointer m_read;
	device_function_pointer m_write;
	WORD m_low_range;
	WORD m_high_range;
};

#endif
