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
                        "Failed to close session: %s",
                        wiredtiger_strerror(result)
                    );

                    abort();
                }
            });
        }

        void CreateTable(
            const std::string& dbName,
            const std::string& keyFormat,
            const std::string& valueFormat
        ) {
            int result = Session->create(
                Session.get(),
                ("table:" + dbName).c_str(),
                ("key_format=" + keyFormat + ",value_format=" + valueFormat).c_str()
            );

            if (result != 0) {
                dprintf(
                    2,
                    "Failed to create table: %s",
                    wiredtiger_strerror(result)
                );

                abort();
            }
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
        const std::string& valueFormat
    ) {
        Impl->CreateTable(dbName, keyFormat, valueFormat);
    }
}
