#include "field.hpp"
#include <wiredtiger.h>

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

    std::function<void*()> TFSMetaDBFieldBase::Load(void* session, size_t size, const void* data) const {
        if (const auto* ptr = dynamic_cast<const TFSMetaDBStringField*>(this)) {
            return [val = std::string((const char*)data, size)](){
                return (void*)&val;
            };
        }

        if (const auto* ptr = dynamic_cast<const TFSMetaDBUIntField*>(this)) {
            uint64_t val;
            wiredtiger_struct_unpack((WT_SESSION*)session, data, size, "Q", &val);

            return [val](){
                return (void*)&val;
            };
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
