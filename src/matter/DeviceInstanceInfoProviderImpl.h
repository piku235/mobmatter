#pragma once

#include <platform/DeviceInstanceInfoProvider.h>
#include <string>

namespace mobmatter::matter {

class DeviceInstanceInfoProviderImpl final : public chip::DeviceLayer::DeviceInstanceInfoProvider {
public:
    void SetSerialNumber(const std::string& serialNumber);
    CHIP_ERROR GetVendorName(char* buf, size_t bufSize) override;
    CHIP_ERROR GetVendorId(uint16_t& vendorId) override;
    CHIP_ERROR GetProductName(char* buf, size_t bufSize) override;
    CHIP_ERROR GetProductId(uint16_t& productId) override;
    CHIP_ERROR GetPartNumber(char* buf, size_t bufSize) override { return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE; }
    CHIP_ERROR GetProductURL(char* buf, size_t bufSize) override { return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE; };
    CHIP_ERROR GetProductLabel(char* buf, size_t bufSize) override { return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE; };
    CHIP_ERROR GetSerialNumber(char* buf, size_t bufSize) override;
    CHIP_ERROR GetManufacturingDate(uint16_t& year, uint8_t& month, uint8_t& day) override { return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE; };
    CHIP_ERROR GetHardwareVersion(uint16_t& hardwareVersion) override;
    CHIP_ERROR GetHardwareVersionString(char* buf, size_t bufSize) override;
    CHIP_ERROR GetRotatingDeviceIdUniqueId(chip::MutableByteSpan& uniqueIdSpan) override { return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE; };

private:
    std::string mSerialNumber;
};

}
