#pragma once

#include "field.hpp"
#include <memory>
#include <vector>
#include <utility>
#include <functional>
#include <string>
#include <unordered_map>
#include <ac-common/str.hpp>

namespace NAC {
    template<typename TKey_, typename TValue_>
    class TFSMetaDBModelDescr {
    public:
        using TKey = TKey_;
        using TValue = TValue_;

        static std::string DBName;
    };
}

using __TACModelFieldMap = std::unordered_map<std::string, size_t>;
using __TACModelFields = std::vector<std::unique_ptr<NAC::TFSMetaDBFieldBase>>;
using __TACModelData = std::vector<std::function<void*()>>;

#define AC_MODEL_BEGIN(cls) \
class cls : public TFSMetaDBModelBase { \
private: \
    using TSelf = cls; \
\
private: \
    static __TACModelFieldMap __ACModelFieldMap; \
    static __TACModelFields __ACModelFields; \
    static __TACModelData __ACModelData; \
\
protected: \
    const __TACModelFieldMap& __GetACModelFieldMap() const override { \
        return __ACModelFieldMap; \
    } \
\
    const __TACModelFields& __GetACModelFields() const override { \
        return __ACModelFields; \
    } \
\
    __TACModelData& __GetACModelData() override { \
        return __ACModelData; \
    } \
\
    const __TACModelData& __GetACModelData() const override { \
        return __ACModelData; \
    }

#define AC_MODEL_FIELD2(type, name, dbName) \
private: \
    static size_t __ACModelGet ## name ## Index() { \
        std::unique_ptr<TFSMetaDBFieldBase> ptr(new type); \
        ptr->Name = #dbName; \
        size_t index = ptr->Index = __ACModelFields.size(); \
\
        __ACModelFields.emplace_back(std::move(ptr)); \
        __ACModelFieldMap.emplace(#name, index); \
        __ACModelData.emplace_back(__TACModelData::value_type()); \
\
        return index; \
    } \
\
    static const size_t __ACModelFieldIndex ## name; \
\
public: \
    const type& Get ## name() const { \
        auto* ptr = (const type*)__ACModelGet(__ACModelFieldIndex ## name); \
\
        return (ptr ? *ptr : type()); \
    } \
\
    TSelf& Set ## name(const type& val) { \
        __ACModelSet(__ACModelFieldIndex ## name, [val]() { \
            return (void*)&val; \
        }); \
\
        return *this; \
    }

#define AC_MODEL_FIELD(type, name) AC_MODEL_FIELD2(type, name, name)

#define AC_MODEL_FIELD_IMPL(cls, name) \
    const size_t cls::__ACModelFieldIndex ## name = cls::__ACModelGet ## name ## Index();

#define AC_MODEL_END() \
private: \
    static std::string __ACModelBuildFullFormat() { \
        std::string out; \
\
        for (const auto& it : __ACModelFields) { \
            out += it->Format(); \
        } \
\
        return out; \
    } \
\
    static const std::string __ACModelFullFormat; \
\
protected: \
    const std::string& __ACModelGetFullFormat() const override { \
        return __ACModelFullFormat; \
    } \
\
public: \
    static const std::string& __ACModelGetFullFormatStatic() { \
        return __ACModelFullFormat; \
    } \
};

#define AC_MODEL_IMPL(cls) \
    __TACModelFieldMap cls::__ACModelFieldMap; \
    __TACModelFields cls::__ACModelFields; \
    __TACModelData cls::__ACModelData; \
    const std::string cls::__ACModelFullFormat = cls::__ACModelBuildFullFormat();

namespace NAC {
    class TFSMetaDBModelBase {
    public:
        void Load(void*, size_t, void*);
        TBlob Dump(void*) const;

    protected:
        virtual const __TACModelFieldMap& __GetACModelFieldMap() const = 0;
        virtual const __TACModelFields& __GetACModelFields() const = 0;
        virtual const std::string& __ACModelGetFullFormat() const = 0;
        virtual __TACModelData& __GetACModelData() = 0;
        virtual const __TACModelData& __GetACModelData() const = 0;

        void* __ACModelGet(size_t i) const {
            if (auto&& cb = __GetACModelData().at(i)) {
                return cb();

            } else {
                return nullptr;
            }
        }

        void __ACModelSet(size_t i, std::function<void*()>&& item) {
            __GetACModelData()[i] = std::move(item);
        }
    };
}
