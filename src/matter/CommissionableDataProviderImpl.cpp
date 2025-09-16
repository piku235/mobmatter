#include "CommissionableDataProviderImpl.h"

#include <crypto/CHIPCryptoPAL.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/Span.h>
#include <lib/support/logging/CHIPLogging.h>

#include <cstring>

using namespace chip::Crypto;

namespace {

constexpr uint32_t kPaseIterationCount = kSpake2p_Min_PBKDF_Iterations;

CHIP_ERROR GeneratePaseSalt(std::vector<uint8_t>& spake2pSaltVector)
{
    constexpr size_t kSaltLen = kSpake2p_Max_PBKDF_Salt_Length;
    spake2pSaltVector.resize(kSaltLen);
    return DRBG_get_bytes(spake2pSaltVector.data(), spake2pSaltVector.size());
}

} // namespace

namespace mobmatter::matter {

CHIP_ERROR CommissionableDataProviderImpl::Init()
{
    VerifyOrReturnError(mIsInitialized == false, CHIP_ERROR_UNINITIALIZED);

    CHIP_ERROR err;
    std::vector<uint8_t> spake2pSalt;

    ChipLogProgress(Support, "Generating PASE salt.");
    err = GeneratePaseSalt(spake2pSalt);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(Support, "Failed to generate PASE salt: %" CHIP_ERROR_FORMAT, err.Format());
        return err;
    }

    Spake2pVerifier passcodeVerifier;
    std::vector<uint8_t> serializedPasscodeVerifier(kSpake2p_VerifierSerialized_Length);
    chip::MutableByteSpan saltSpan { spake2pSalt.data(), spake2pSalt.size() };
    err = passcodeVerifier.Generate(kPaseIterationCount, saltSpan, CHIP_DEVICE_CONFIG_DEVICE_SETUP_PASSCODE);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(Support, "Failed to generate PASE verifier from passcode: %" CHIP_ERROR_FORMAT, err.Format());
        return err;
    }

    chip::MutableByteSpan verifierSpan { serializedPasscodeVerifier.data(), serializedPasscodeVerifier.size() };
    err = passcodeVerifier.Serialize(verifierSpan);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(Support, "Failed to serialize PASE verifier from passcode: %" CHIP_ERROR_FORMAT, err.Format());
        return err;
    }

    mSerializedPaseVerifier = std::move(serializedPasscodeVerifier);
    mPaseSalt = std::move(spake2pSalt);
    mIsInitialized = true;

    return CHIP_NO_ERROR;
}

CHIP_ERROR CommissionableDataProviderImpl::GetSetupDiscriminator(uint16_t& setupDiscriminator)
{
    VerifyOrReturnError(mIsInitialized == true, CHIP_ERROR_UNINITIALIZED);
    setupDiscriminator = CHIP_DEVICE_CONFIG_DEVICE_SETUP_DISCRIMINATOR;
    return CHIP_NO_ERROR;
}

CHIP_ERROR CommissionableDataProviderImpl::GetSpake2pIterationCount(uint32_t& iterationCount)
{
    VerifyOrReturnError(mIsInitialized == true, CHIP_ERROR_UNINITIALIZED);
    iterationCount = kPaseIterationCount;
    return CHIP_NO_ERROR;
}

CHIP_ERROR CommissionableDataProviderImpl::GetSpake2pSalt(chip::MutableByteSpan& saltBuf)
{
    VerifyOrReturnError(mIsInitialized == true, CHIP_ERROR_UNINITIALIZED);

    VerifyOrReturnError(saltBuf.size() >= kSpake2p_Max_PBKDF_Salt_Length, CHIP_ERROR_BUFFER_TOO_SMALL);
    memcpy(saltBuf.data(), mPaseSalt.data(), mPaseSalt.size());
    saltBuf.reduce_size(mPaseSalt.size());

    return CHIP_NO_ERROR;
}

CHIP_ERROR CommissionableDataProviderImpl::GetSpake2pVerifier(chip::MutableByteSpan& verifierBuf, size_t& outVerifierLen)
{
    VerifyOrReturnError(mIsInitialized == true, CHIP_ERROR_UNINITIALIZED);

    // By now, serialized verifier from Init should be correct size
    VerifyOrReturnError(mSerializedPaseVerifier.size() == kSpake2p_VerifierSerialized_Length, CHIP_ERROR_INTERNAL);

    outVerifierLen = mSerializedPaseVerifier.size();
    VerifyOrReturnError(verifierBuf.size() >= outVerifierLen, CHIP_ERROR_BUFFER_TOO_SMALL);
    memcpy(verifierBuf.data(), mSerializedPaseVerifier.data(), mSerializedPaseVerifier.size());
    verifierBuf.reduce_size(mSerializedPaseVerifier.size());

    return CHIP_NO_ERROR;
}

CHIP_ERROR CommissionableDataProviderImpl::GetSetupPasscode(uint32_t& setupPasscode)
{
    VerifyOrReturnError(mIsInitialized == true, CHIP_ERROR_UNINITIALIZED);
    setupPasscode = CHIP_DEVICE_CONFIG_DEVICE_SETUP_PASSCODE;
    return CHIP_NO_ERROR;
}

}
