#pragma once

#include "SqliteError.h"

#include <tl/expected.hpp>

namespace mobmatter::common::persistence::sqlite {

template <typename TOk = void>
using Result = tl::expected<TOk, SqliteError>;

}
