#include "connection.hpp"
#include <wiredtiger.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory>

namespace NAC {
    class TFSMetaDBImpl {
    public:
        TFSMetaDBImpl() = delete;
        TFSMetaDBImpl(const TFSMetaDBImpl&) = delete;
        TFSMetaDBImpl(TFSMetaDBImpl&&) = delete;

        TFSMetaDBImpl(const std::string& path) {
            WT_CONNECTION* db;
            int result = wiredtiger_open(
                path.c_str(),
                nullptr,
                "create,cache_size=5GB,eviction=(threads_max=20,threads_min=5),eviction_dirty_target=30,eviction_target=40,eviction_trigger=60,session_max=10000,log=(enabled=true,file_max=100MB),statistics=(all,clear),statistics_log=(wait=1)",
                &db
            );

            if (result != 0) {
                dprintf(2, "Error opening database: %s", wiredtiger_strerror(result));
                abort();
            }

            Db = std::shared_ptr<WT_CONNECTION>(db, [](WT_CONNECTION* db) {
                int result = db->close(db, nullptr);

                if (result != 0) {
                    dprintf(
                        2,
                        "Failed to close database: %s",
                        wiredtiger_strerror(result)
                    );

                    abort();
                }
            });
        }

        TFSMetaDBSession Open() const {
            WT_SESSION* session;
            int result = Db->open_session(
                Db.get(),
                nullptr,
                nullptr,
                &session
            );

            if (result != 0) {
                dprintf(
                    2,
                    "Failed to open session: %s",
                    wiredtiger_strerror(result)
                );

                abort();
            }

            return TFSMetaDBSession(session);
        }

    private:
        std::shared_ptr<WT_CONNECTION> Db;
    };

    TFSMetaDB::TFSMetaDB(const std::string& path)
        : Impl(new TFSMetaDBImpl(path))
    {
    }

    TFSMetaDB::~TFSMetaDB() {
        delete Impl;
    }

    TFSMetaDBSession TFSMetaDB::Open() const {
        return Impl->Open();
    }
}
