#pragma once

#include <string>
#include "model.hpp"
#include <iostream>
#include "iterator.hpp"

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
        void CreateTable(const std::string&, const std::string&, const std::string&, const std::string&);
        void CreateIndex(const std::string&, const std::string&, const std::string&, const std::string&);
        bool Insert(const std::string&, const TFSMetaDBModelBase&, const TFSMetaDBModelBase&);
        bool Get(const std::string&, const TFSMetaDBModelBase&, TFSMetaDBModelBase&);
        TFSMetaDBIterator Search(const std::string&, const std::string&, const std::string&, const TFSMetaDBModelBase&);

    public:
        template<class TFSMetaDBModelDescr>
        void CreateTable() {
            CreateTable(
                TFSMetaDBModelDescr::DBName,
                TFSMetaDBModelDescr::TKey::__ACModelGetFullFormatStatic(),
                TFSMetaDBModelDescr::TValue::__ACModelGetFullFormatStatic(),
                TFSMetaDBModelDescr::FieldNames()
            );
        }

        template<class TFSMetaDBIndexDescr>
        void CreateIndex() {
            CreateIndex(
                TFSMetaDBIndexDescr::TModel::DBName,
                TFSMetaDBIndexDescr::DBName,
                TFSMetaDBIndexDescr::TKey::__ACModelGetFieldNameListStatic(),
                TFSMetaDBIndexDescr::TValue::__ACModelGetFieldNameListStatic()
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

        template<class TFSMetaDBIndexDescr>
        TFSMetaDBIterator Search(
            const typename TFSMetaDBIndexDescr::TKey& key
        ) {
            return Search(
                TFSMetaDBIndexDescr::TModel::DBName,
                TFSMetaDBIndexDescr::DBName,
                TFSMetaDBIndexDescr::TValue::__ACModelGetFieldNameListStatic(),
                key
            );
        }

    private:
        TFSMetaDBSessionImpl* Impl;
    };
}
