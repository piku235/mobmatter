#pragma once

#include <platform/DeviceInfoProvider.h>

namespace mmbridge::matter {

class DeviceInfoProviderImpl : public chip::DeviceLayer::DeviceInfoProvider {
public:
    DeviceInfoProviderImpl() = default;
    ~DeviceInfoProviderImpl() override { }

    // Iterators
    chip::DeviceLayer::DeviceInfoProvider::FixedLabelIterator* IterateFixedLabel(chip::EndpointId endpoint) override { return nullptr; }
    chip::DeviceLayer::DeviceInfoProvider::UserLabelIterator* IterateUserLabel(chip::EndpointId endpoint) override { return nullptr; }
    chip::DeviceLayer::DeviceInfoProvider::SupportedLocalesIterator* IterateSupportedLocales() override;
    chip::DeviceLayer::DeviceInfoProvider::SupportedCalendarTypesIterator* IterateSupportedCalendarTypes() override { return nullptr; }

protected:
    class SupportedLocalesIteratorImpl : public SupportedLocalesIterator {
    public:
        SupportedLocalesIteratorImpl() = default;
        size_t Count() override;
        bool Next(chip::CharSpan& output) override;
        void Release() override { chip::Platform::Delete(this); }

    private:
        size_t mIndex = 0;
        char mActiveLocaleBuf[chip::DeviceLayer::kMaxActiveLocaleLength + 1];
    };

    CHIP_ERROR SetUserLabelLength(chip::EndpointId endpoint, size_t val) override { return CHIP_ERROR_NOT_IMPLEMENTED; }
    CHIP_ERROR GetUserLabelLength(chip::EndpointId endpoint, size_t& val) override { return CHIP_ERROR_NOT_IMPLEMENTED; }
    CHIP_ERROR SetUserLabelAt(chip::EndpointId endpoint, size_t index, const UserLabelType& userLabel) override { return CHIP_ERROR_NOT_IMPLEMENTED; }
    CHIP_ERROR DeleteUserLabelAt(chip::EndpointId endpoint, size_t index) override { return CHIP_ERROR_NOT_IMPLEMENTED; }
};

}
