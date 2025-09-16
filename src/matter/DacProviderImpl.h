#pragma once

#include <credentials/DeviceAttestationCredsProvider.h>
#include <lib/core/CHIPError.h>
#include <lib/support/Span.h>

namespace mobmatter::matter {

class DacProviderImpl final : public chip::Credentials::DeviceAttestationCredentialsProvider {
public:
    CHIP_ERROR GetCertificationDeclaration(chip::MutableByteSpan& out_cd_buffer) override;
    CHIP_ERROR GetFirmwareInformation(chip::MutableByteSpan& out_firmware_info_buffer) override;
    CHIP_ERROR GetDeviceAttestationCert(chip::MutableByteSpan& out_dac_buffer) override;
    CHIP_ERROR GetProductAttestationIntermediateCert(chip::MutableByteSpan& out_pai_buffer) override;
    CHIP_ERROR SignWithDeviceAttestationKey(const chip::ByteSpan& message_to_sign, chip::MutableByteSpan& out_signature_buffer) override;
};

}
