#include "session.hpp"
#include <wiredtiger.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory>

namespace NAC {
    class TFSMetaDBSessionImpl {
    public:
        TFSMetaDBSessionImpl() = delete;
        TFSMetaDBSessionImpl(const TFSMetaDBSessionImpl&) = delete;
        TFSMetaDBSessionImpl(TFSMetaDBSessionImpl&&) = delete;

        TFSMetaDBSessionImpl(WT_SESSION* session) {
            Session = std::shared_ptr<WT_SESSION>(session, [](WT_SESSION* session) {
                int result = session->close(session, nullptr);

                if (result != 0) {
                    dprintf(
                        2,
                        "Failed to close session: %s\n",
                        wiredtiger_strerror(result)
                    );

                    abort();
                }
            });
        }

        void CreateTable(
            const std::string& dbName,
            const std::string& keyFormat,
            const std::string& valueFormat,
            const std::string& fieldNames
        ) {
            int result = Session->create(
                Session.get(),
                ("table:" + dbName).c_str(),
                ("key_format=" + keyFormat + ",value_format=" + valueFormat + ",columns=(" + fieldNames + ")").c_str()
            );

            if (result != 0) {
                dprintf(
                    2,
                    "Failed to create table: %s\n",
                    wiredtiger_strerror(result)
                );

                abort();
            }
        }

        std::shared_ptr<WT_CURSOR> Cursor(
            const std::string& dbName,
            const char* const options = nullptr
        ) {
            WT_CURSOR* cursor;
            int result = Session->open_cursor(
                Session.get(),
                dbName.data(),
                nullptr,
                options,
                &cursor
            );

            if (result != 0) {
                dprintf(
                    2,
                    "Error opening cursor: %s\n",
                    wiredtiger_strerror(result)
                );

                abort();
            }

            return std::shared_ptr<WT_CURSOR>(cursor, [](WT_CURSOR* cursor) {
                cursor->close(cursor);
            });
        }

        bool Insert(
            const std::string& dbName,
            const TFSMetaDBModelBase& key_,
            const TFSMetaDBModelBase& value_
        ) {
            auto cursor = Cursor(("table:" + dbName), "raw");
            auto keyBlob = key_.Dump((void*)Session.get());
            auto valueBlob = value_.Dump((void*)Session.get());

            WT_ITEM key {
                .data = keyBlob.Data(),
                .size = keyBlob.Size(),
            };

            WT_ITEM value {
                .data = valueBlob.Data(),
                .size = valueBlob.Size(),
            };

            cursor->set_key(cursor.get(), &key);
            cursor->set_value(cursor.get(), &value);

            return (cursor->insert(cursor.get()) == 0);
        }

        bool Get(
            const std::string& dbName,
            const TFSMetaDBModelBase& key_,
            TFSMetaDBModelBase& out
        ) {
            auto cursor = Cursor(("table:" + dbName), "raw");
            auto keyBlob = key_.Dump((void*)Session.get());
            WT_ITEM key {
                .data = keyBlob.Data(),
                .size = keyBlob.Size(),
            };

            cursor->set_key(cursor.get(), &key);

            int result = cursor->search(cursor.get());

            if(result == 0) {
                WT_ITEM value;
                result = cursor->get_value(cursor.get(), &value);

                if (result == 0) {
                    out.Load((void*)Session.get(), value.size, value.data);

                    return true;
                }

            } else if (result != WT_NOTFOUND) {
                dprintf(
                    2,
                    "Failed to get key: %s\n",
                    wiredtiger_strerror(result)
                );
            }

            return false;
        }

    private:
        std::shared_ptr<WT_SESSION> Session;
    };

    TFSMetaDBSession::TFSMetaDBSession(void* ptr)
        : Impl(new TFSMetaDBSessionImpl((WT_SESSION*)ptr))
    {
    }

    TFSMetaDBSession::~TFSMetaDBSession() {
        delete Impl;
    }

    void TFSMetaDBSession::CreateTable(
        const std::string& dbName,
        const std::string& keyFormat,
        const std::string& valueFormat,
        const std::string& fieldNames
    ) {
        Impl->CreateTable(dbName, keyFormat, valueFormat, fieldNames);
    }

    bool TFSMetaDBSession::Insert(
        const std::string& dbName,
        const TFSMetaDBModelBase& key,
        const TFSMetaDBModelBase& data
    ) {
        return Impl->Insert(dbName, key, data);
    }

    bool TFSMetaDBSession::Get(
        const std::string& dbName,
        const TFSMetaDBModelBase& key,
        TFSMetaDBModelBase& out
    ) {
        return Impl->Get(dbName, key, out);
    }
}
