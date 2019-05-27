#pragma once

#include <string>

namespace NAC {
    class TFSMetaDBSessionImpl;

    class TFSMetaDBSession {
    public:
        TFSMetaDBSession() = delete;
        TFSMetaDBSession(void*);

        TFSMetaDBSession(const TFSMetaDBSession&) = delete;
        TFSMetaDBSession(TFSMetaDBSession&& right) {
            Impl = right.Impl;
            right.Impl = nullptr;
        }

        ~TFSMetaDBSession();

    private:
        void CreateTable(
            const std::string&,
            const std::string&,
            const std::string&
        );

    public:
        template<class TFSMetaDBModelDescr>
        void CreateTable() {
            CreateTable(
                TFSMetaDBModelDescr::DBName,
                TFSMetaDBModelDescr::TKey::__ACModelGetFullFormatStatic(),
                TFSMetaDBModelDescr::TValue::__ACModelGetFullFormatStatic()
            );
        }

    private:
        TFSMetaDBSessionImpl* Impl;
    };
}
