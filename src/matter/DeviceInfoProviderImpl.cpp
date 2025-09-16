#include "DeviceInfoProviderImpl.h"

#include <lib/support/CHIPMemString.h>
#include <lib/support/CodeUtils.h>

#include <cstring>

using namespace chip;
using namespace chip::DeviceLayer;

namespace mobmatter::matter {

DeviceInfoProvider::SupportedLocalesIterator* DeviceInfoProviderImpl::IterateSupportedLocales()
{
    return chip::Platform::New<SupportedLocalesIteratorImpl>();
}

size_t DeviceInfoProviderImpl::SupportedLocalesIteratorImpl::Count()
{
    // {("en-US")}
    return 1;
}

bool DeviceInfoProviderImpl::SupportedLocalesIteratorImpl::Next(CharSpan& output)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    const char* activeLocalePtr = nullptr;

    VerifyOrReturnError(mIndex < 1, false);

    switch (mIndex) {
    case 0:
        activeLocalePtr = "en-US";
        break;
    default:
        err = CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND;
        break;
    }

    if (err == CHIP_NO_ERROR) {
        VerifyOrReturnError(std::strlen(activeLocalePtr) <= kMaxActiveLocaleLength, false);

        chip::Platform::CopyString(mActiveLocaleBuf, kMaxActiveLocaleLength + 1, activeLocalePtr);
        output = CharSpan::fromCharString(mActiveLocaleBuf);

        mIndex++;

        return true;
    }

    return false;
}

}
