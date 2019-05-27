#include "field.hpp"
#include <wiredtiger.h>
#include <stdio.h>

namespace NAC {
    std::string TFSMetaDBFieldBase::Format() const {
        if (const auto* ptr = dynamic_cast<const TFSMetaDBStringField*>(this)) {
            return std::string("S");
        }

        if (const auto* ptr = dynamic_cast<const TFSMetaDBUIntField*>(this)) {
            return std::string("Q");
        }

        return std::string();
    }

    std::function<void*()> TFSMetaDBFieldBase::Load(void* stream) const {
        if (const auto* ptr = dynamic_cast<const TFSMetaDBStringField*>(this)) {
            const char* data;
            int result = wiredtiger_unpack_str((WT_PACK_STREAM*)stream, &data);

            if (result == 0) {
                return [val = std::string(data)](){
                    return (void*)&val;
                };

            } else {
                dprintf(
                    2,
                    "Failed to unpack string: %s\n",
                    wiredtiger_strerror(result)
                );
            }
        }

        if (const auto* ptr = dynamic_cast<const TFSMetaDBUIntField*>(this)) {
            uint64_t val;
            int result = wiredtiger_unpack_uint((WT_PACK_STREAM*)stream, &val);

            if (result == 0) {
                return [val](){
                    return (void*)&val;
                };

            } else {
                dprintf(
                    2,
                    "Failed to unpack uint: %s\n",
                    wiredtiger_strerror(result)
                );
            }
        }

        return std::function<void*()>();
    }

    size_t TFSMetaDBFieldBase::DumpedSize(void* session, const void* value) const {
        size_t out = 0;

        if (const auto* ptr = dynamic_cast<const TFSMetaDBStringField*>(this)) {
            wiredtiger_struct_size((WT_SESSION*)session, &out, "S", ((const std::string*)value)->data());
        }

        if (const auto* ptr = dynamic_cast<const TFSMetaDBUIntField*>(this)) {
            wiredtiger_struct_size((WT_SESSION*)session, &out, "Q", *(const uint64_t*)value);
        }

        return out;
    }

    void TFSMetaDBFieldBase::Dump(void* stream, const void* value) const {
        if (const auto* ptr = dynamic_cast<const TFSMetaDBStringField*>(this)) {
            wiredtiger_pack_str((WT_PACK_STREAM*)stream, ((const std::string*)value)->data());
        }

        if (const auto* ptr = dynamic_cast<const TFSMetaDBUIntField*>(this)) {
            wiredtiger_pack_uint((WT_PACK_STREAM*)stream, *(const uint64_t*)value);
        }
    }
}
