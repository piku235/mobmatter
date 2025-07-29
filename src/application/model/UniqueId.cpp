#include "UniqueId.h"

#include <cstdint>
#include <openssl/rand.h>
#include <openssl/crypto.h>

namespace mmbridge::application::model {

UniqueId UniqueId::random()
{
    uint8_t buf[kLength / 2];
    char hexstr[kLength + 1];

    RAND_bytes(buf, sizeof(buf));
    OPENSSL_buf2hexstr_ex(hexstr, sizeof(hexstr), nullptr, buf, sizeof(buf), '\0');

    return std::string(hexstr);
}

UniqueId::UniqueId(std::string value)
    : mValue(std::move(value))
{
}

}
