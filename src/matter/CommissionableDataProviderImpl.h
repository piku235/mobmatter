#pragma once

#include <lib/core/CHIPError.h>
#include <lib/core/Optional.h>
#include <platform/CommissionableDataProvider.h>

#include <stdint.h>
#include <vector>

namespace mmbridge::matter {

class CommissionableDataProviderImpl : public chip::DeviceLayer::CommissionableDataProvider {
public:
    CommissionableDataProviderImpl() { }

    CHIP_ERROR Init();
    CHIP_ERROR GetSetupDiscriminator(uint16_t& setupDiscriminator) override;
    CHIP_ERROR SetSetupDiscriminator(uint16_t setupDiscriminator) override { return CHIP_ERROR_NOT_IMPLEMENTED; }
    CHIP_ERROR GetSpake2pIterationCount(uint32_t& iterationCount) override;
    CHIP_ERROR GetSpake2pSalt(chip::MutableByteSpan& saltBuf) override;
    CHIP_ERROR GetSpake2pVerifier(chip::MutableByteSpan& verifierBuf, size_t& outVerifierLen) override;
    CHIP_ERROR GetSetupPasscode(uint32_t& setupPasscode) override;
    CHIP_ERROR SetSetupPasscode(uint32_t setupPasscode) override { return CHIP_ERROR_NOT_IMPLEMENTED; }

private:
    bool mIsInitialized = false;
    std::vector<uint8_t> mSerializedPaseVerifier;
    std::vector<uint8_t> mPaseSalt;
};

}
