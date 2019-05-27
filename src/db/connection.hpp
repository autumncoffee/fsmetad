#pragma once

#include "session.hpp"
#include <string>

namespace NAC {
    class TFSMetaDBImpl;

    class TFSMetaDB {
    public:
        TFSMetaDB() = delete;
        TFSMetaDB(const std::string& path);

        TFSMetaDB(const TFSMetaDB&) = delete;
        TFSMetaDB(TFSMetaDB&& right) {
            Impl = right.Impl;
            right.Impl = nullptr;
        }

        ~TFSMetaDB();

        TFSMetaDBSession Open() const;

    private:
        TFSMetaDBImpl* Impl;
    };
}
