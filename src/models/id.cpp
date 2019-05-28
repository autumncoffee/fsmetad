#include "id.hpp"
#include <random>
#include <time.h>
#include <string.h>
#include <limits>
#include <ac-common/utils/htonll.hpp>

namespace NAC {
    std::string GenerateID(uint16_t attempt) {
        thread_local static std::random_device rd;
        thread_local static std::mt19937 g(rd());
        thread_local static std::uniform_int_distribution<uint64_t> dis(0, (std::numeric_limits<uint64_t>::max() - 1));

        uint64_t rnd(dis(g));
        uint64_t tm(time(nullptr));
        unsigned char buf [sizeof(tm) + sizeof(rnd) + sizeof(attempt)];

        rnd = htonll(rnd);
        tm = htonll(tm);
        attempt = htons(attempt);

        memcpy(buf, &tm, sizeof(tm));
        memcpy(buf + sizeof(tm), &rnd, sizeof(rnd));
        memcpy(buf + sizeof(tm) + sizeof(rnd), &attempt, sizeof(attempt));

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
