#pragma once

#include <ac-library/websocket/parser/parser.hpp>

namespace NAC {
    struct TFileSyncFrameHeader {
        uint64_t Offset = 0;
        uint64_t Size = 0;
    };

    TFileSyncFrameHeader FileSyncFrameHeader(size_t size, size_t offset);

    NWebSocketParser::TFrame FileSyncFrameV1(const TFileSyncFrameHeader& header);

    static inline NWebSocketParser::TFrame FileSyncFrameV1(size_t size, size_t offset) {
        return FileSyncFrameV1(FileSyncFrameHeader(size, offset));
    }
}
