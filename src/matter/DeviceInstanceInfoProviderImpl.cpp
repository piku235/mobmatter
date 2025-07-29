#include "DeviceInstanceInfoProviderImpl.h"

#include <lib/support/CodeUtils.h>

namespace mmbridge::matter {

void DeviceInstanceInfoProviderImpl::SetSerialNumber(const std::string& serialNumber)
{
    mSerialNumber = std::move(serialNumber);
}

CHIP_ERROR DeviceInstanceInfoProviderImpl::GetVendorName(char* buf, size_t bufSize)
{
    VerifyOrReturnError(bufSize >= sizeof(CHIP_DEVICE_CONFIG_DEVICE_VENDOR_NAME), CHIP_ERROR_BUFFER_TOO_SMALL);
    strcpy(buf, CHIP_DEVICE_CONFIG_DEVICE_VENDOR_NAME);
    return CHIP_NO_ERROR;
}

CHIP_ERROR DeviceInstanceInfoProviderImpl::GetVendorId(uint16_t& vendorId)
{
    vendorId = static_cast<uint16_t>(CHIP_DEVICE_CONFIG_DEVICE_VENDOR_ID);
    return CHIP_NO_ERROR;
}

CHIP_ERROR DeviceInstanceInfoProviderImpl::GetProductName(char* buf, size_t bufSize)
{
    VerifyOrReturnError(bufSize >= sizeof(CHIP_DEVICE_CONFIG_DEVICE_PRODUCT_NAME), CHIP_ERROR_BUFFER_TOO_SMALL);
    strcpy(buf, CHIP_DEVICE_CONFIG_DEVICE_PRODUCT_NAME);
    return CHIP_NO_ERROR;
}

CHIP_ERROR DeviceInstanceInfoProviderImpl::GetProductId(uint16_t& productId)
{
    productId = static_cast<uint16_t>(CHIP_DEVICE_CONFIG_DEVICE_PRODUCT_ID);
    return CHIP_NO_ERROR;
}

CHIP_ERROR DeviceInstanceInfoProviderImpl::GetSerialNumber(char* buf, size_t bufSize)
{
    VerifyOrReturnError(bufSize >= mSerialNumber.size() + 1, CHIP_ERROR_BUFFER_TOO_SMALL);
    strcpy(buf, mSerialNumber.c_str());
    return CHIP_NO_ERROR;
}

CHIP_ERROR DeviceInstanceInfoProviderImpl::GetHardwareVersion(uint16_t& hardwareVersion)
{
    hardwareVersion = static_cast<uint16_t>(CHIP_DEVICE_CONFIG_DEVICE_HARDWARE_VERSION);
    return CHIP_NO_ERROR;
}

CHIP_ERROR DeviceInstanceInfoProviderImpl::GetHardwareVersionString(char* buf, size_t bufSize)
{
    VerifyOrReturnError(bufSize >= sizeof(CHIP_DEVICE_CONFIG_DEVICE_HARDWARE_VERSION_STRING), CHIP_ERROR_BUFFER_TOO_SMALL);
    strcpy(buf, CHIP_DEVICE_CONFIG_DEVICE_HARDWARE_VERSION_STRING);
    return CHIP_NO_ERROR;
}

CHIP_ERROR DeviceInstanceInfoProviderImpl::GetSoftwareVersionString(char* buf, size_t bufSize)
{
    VerifyOrReturnError(bufSize >= sizeof(CHIP_DEVICE_CONFIG_DEVICE_SOFTWARE_VERSION_STRING), CHIP_ERROR_BUFFER_TOO_SMALL);
    strcpy(buf, CHIP_DEVICE_CONFIG_DEVICE_SOFTWARE_VERSION_STRING);
    return CHIP_NO_ERROR;
}

}
