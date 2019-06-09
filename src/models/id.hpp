#pragma once

#include <string>

namespace NAC {
    std::string GenerateID(uint16_t suffix = 0);

    static std::string MAX_ID = "ffffffffffffffffffffffffffffffffffff";
}
