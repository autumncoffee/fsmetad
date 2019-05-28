#pragma once

#include <memory>
#include "model.hpp"
#include <ac-common/str.hpp>

namespace NAC {
    class TFSMetaDBIteratorImpl;

    class TFSMetaDBIterator {
    public:
        TFSMetaDBIterator() = delete;
        TFSMetaDBIterator(const std::shared_ptr<void>&, int, TBlob&&);

        TFSMetaDBIterator(const TFSMetaDBIterator&) = delete;
        TFSMetaDBIterator(TFSMetaDBIterator&& right) {
            Impl = right.Impl;
            right.Impl = nullptr;
        }

        ~TFSMetaDBIterator();

        bool Next(TFSMetaDBModelBase& out);

    private:
        TFSMetaDBIteratorImpl* Impl;
    };
}
