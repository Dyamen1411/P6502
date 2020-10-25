#include "bus.h"

Bus::Bus() {}

Bus::~Bus() {}

// Returns 1 if device range of addresses opverlaps with one of the current connected device
BYTE Bus::connectDevice(DeviceInterface * device_interface) {
    if (m_device_interface_index <= MAX_DEVICES) {
	return 1;
    }

    for (WORD i = 0; i < m_device_interface_index; ++i) {
	if (device_interface->overlapsWithDevice(m_device_interfaces[i])) {
	    return 1;
	}
    }

    return 0;
}

// Returns 0 if no device is connected at the given address
BYTE Bus::read(const WORD &address) const {
    return getDevice(address)->read(address);
}

// Returns 1 if no device is connected at the given address
BYTE Bus::write(const WORD &address, const BYTE &data) const {
    return getDevice(address)->write(address, data);
}

DeviceInterface * Bus::getDevice(const WORD &address) const {
    for (WORD i = 0; i < m_device_interface_index; ++i) {
	DeviceInterface * device_interface = m_device_interfaces[i];
	if (device_interface->isInRange(address)) {
	    return device_interface;
	}
    }

    return (DeviceInterface*) 0l;
}
