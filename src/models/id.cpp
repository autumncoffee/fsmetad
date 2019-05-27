#include "id.hpp"
#include <random>
#include <time.h>

namespace NAC {
    std::string GenerateID(uint16_t attempt) {
        thread_local static std::random_device rd;
        thread_local static std::mt19937 g(rd());
        thread_local static std::uniform_int_distribution<uint64_t> dis(0, (999000000 - 1));

        uint64_t rnd(dis(g) + 1000000);
        uint64_t tm(time(nullptr));
        unsigned char buf [sizeof(rnd) + sizeof(tm) + sizeof(attempt)];

        memcpy(buf, &rnd, sizeof(rnd));
        memcpy(buf + sizeof(rnd), &tm, sizeof(tm));
        memcpy(buf + sizeof(rnd) + sizeof(tm), &attempt, sizeof(attempt));

        std::string out;
        out.reserve(sizeof(buf) * 2);

        static const char charTable[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

        for (size_t i = 0; i < sizeof(buf); ++i) {
            out += charTable[buf[i] >> 4];
            out += charTable[buf[i] & 0xf];
        }

        return out;
    }
}
