#include "utils.hpp"
#include <algorithm>
#include <ac-common/utils/htonll.hpp>

namespace NAC {
    TFileSyncFrameHeader FileSyncFrameHeader(size_t size, size_t offset) {
        return TFileSyncFrameHeader {
            .Offset = offset,
            .Size = std::min((size - offset), (size_t)(20 * 1024 * 1024)),
        };
    }

    NWebSocketParser::TFrame FileSyncFrameV1(const TFileSyncFrameHeader& header) {
        const uint64_t chunkSize(hton(header.Size));
        const uint64_t offset_(hton(header.Offset));

        NWebSocketParser::TFrame frame;
        frame.IsFin = true;
        frame.Opcode = 2;
        frame.Payload.Append(sizeof(offset_), (const char*)&offset_);
        frame.Payload.Append(sizeof(chunkSize), (const char*)&chunkSize);

        return frame;
    }
}
