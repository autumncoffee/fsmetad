#pragma once

#include <string>
#include <functional>

namespace NAC {
    class TFSMetaDBFieldBase {
    public:
        std::string Name;
        size_t Index;

    public:
        virtual std::string Format() const;
        virtual std::function<void*()> Load(void*, size_t, const void*) const;
        virtual size_t DumpedSize(void*, const void*) const;
        virtual void Dump(void*, const void*) const;
    };

    template<typename T>
    class TFSMetaDBField : public TFSMetaDBFieldBase {
    public:
        using TValue = T;

    public:
        TValue Value;
    };

    using TFSMetaDBUIntField = TFSMetaDBField<uint64_t>;
    using TFSMetaDBStringField = TFSMetaDBField<std::string>;
}
