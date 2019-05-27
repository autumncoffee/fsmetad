#pragma once

#include <string>
#include "model.hpp"
#include <iostream>

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
            const std::string&,
            const std::string&
        );

        bool Insert(
            const std::string&,
            const TFSMetaDBModelBase&,
            const TFSMetaDBModelBase&
        );

        bool Get(
            const std::string&,
            const TFSMetaDBModelBase&,
            TFSMetaDBModelBase&
        );

    public:
        template<class TFSMetaDBModelDescr>
        void CreateTable() {
            std::string fields;

            for (const auto& it : TFSMetaDBModelDescr::TKey::__GetACModelFieldsStatic()) {
                fields += it->Name + ",";
            }

            for (const auto& it : TFSMetaDBModelDescr::TValue::__GetACModelFieldsStatic()) {
                fields += it->Name + ",";
            }

            if (fields.empty()) {
                std::cerr << TFSMetaDBModelDescr::DBName << " has no fields" << std::endl;
                abort();
            }

            fields.resize(fields.size() - 1);

            CreateTable(
                TFSMetaDBModelDescr::DBName,
                TFSMetaDBModelDescr::TKey::__ACModelGetFullFormatStatic(),
                TFSMetaDBModelDescr::TValue::__ACModelGetFullFormatStatic(),
                fields
            );
        }

        template<class TFSMetaDBModelDescr>
        bool Insert(
            const typename TFSMetaDBModelDescr::TKey& key,
            const typename TFSMetaDBModelDescr::TValue& data
        ) {
            return Insert(
                TFSMetaDBModelDescr::DBName,
                key,
                data
            );
        }

        template<class TFSMetaDBModelDescr>
        bool Get(
            const typename TFSMetaDBModelDescr::TKey& key,
            typename TFSMetaDBModelDescr::TValue& out
        ) {
            return Get(
                TFSMetaDBModelDescr::DBName,
                key,
                out
            );
        }

    private:
        TFSMetaDBSessionImpl* Impl;
    };
}
