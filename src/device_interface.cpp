#include "device_interface.h"

DeviceInterface::DeviceInterface(device_function_pointer read, device_function_pointer write,
	const WORD &low_range, const WORD &high_range) {
    m_read = read;
    m_write = write;
    m_low_range = low_range;
    m_high_range = high_range;
}

DeviceInterface::~DeviceInterface() {

}

BYTE DeviceInterface::isInRange(const WORD &address) const {
    return m_low_range <= address && address <= m_high_range;
}

BYTE DeviceInterface::overlapsWithDevice(DeviceInterface * other) const {
    return isInRange(other->m_low_range) || isInRange(other->m_high_range);
}

BYTE DeviceInterface::read(const WORD &address) const {
    return (*m_read)(address, 0);
}

BYTE DeviceInterface::write(const WORD &address, const BYTE &data) const {
    return (*m_write)(address, data);
}
