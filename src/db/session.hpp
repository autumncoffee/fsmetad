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
        void CreateIndex(const std::string&, const std::string&, const std::string&);
        bool Insert(const std::string&, const TFSMetaDBModelBase&, const TFSMetaDBModelBase&);
        bool Set(const std::string&, const TFSMetaDBModelBase&, const TFSMetaDBModelBase&);
        bool Append(const std::string&, TFSMetaDBModelBase&, const TFSMetaDBModelBase&);
        bool Get(const std::string&, const TFSMetaDBModelBase&, TFSMetaDBModelBase&);
        TFSMetaDBIterator Search(const std::string&, const std::string&, const std::string&, const TFSMetaDBModelBase&, int);

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
                TFSMetaDBIndexDescr::TKey::__ACModelGetFieldNameListStatic()
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
        bool Set(
            const typename TFSMetaDBModelDescr::TKey& key,
            const typename TFSMetaDBModelDescr::TValue& data
        ) {
            return Set(
                TFSMetaDBModelDescr::DBName,
                key,
                data
            );
        }

        template<class TFSMetaDBModelDescr>
        bool Append(
            typename TFSMetaDBModelDescr::TKey& key,
            const typename TFSMetaDBModelDescr::TValue& data
        ) {
            return Append(
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

        template<class TFSMetaDBIndexDescr, class TValue = typename TFSMetaDBIndexDescr::TModel::TValue>
        TFSMetaDBIterator Search(
            const typename TFSMetaDBIndexDescr::TKey& key,
            int direction = 0
        ) {
            return Search(
                TFSMetaDBIndexDescr::TModel::DBName,
                TFSMetaDBIndexDescr::DBName,
                TValue::__ACModelGetFieldNameListStatic(),
                key,
                direction
            );
        }

    private:
        TFSMetaDBSessionImpl* Impl;
    };
}
