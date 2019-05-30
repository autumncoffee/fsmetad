#include "oplog.hpp"
#include <models/oplog.hpp>
#include <sstream>
#include <ac-common/str.hpp>
#include <utility>

namespace NAC {
    namespace {
        static inline TWiredTigerIterator Read(TWiredTigerSession conn, const std::string& id_) {
            if (id_.empty()) {
                return conn.SeqScan<TOplogModel>();

            } else {
                std::stringstream ss;
                uint64_t id;
                ss << id_;
                ss >> id;

                TWiredTigerAutoincrementKey key;
                key.SetId(id + 1);

                return conn.Search<TOplogModel>(key);
            }
        }
    }

    void TReplV1OplogHandler::Handle(
        const std::shared_ptr<TFSMetaDRequest> request,
        const std::vector<std::string>& args
    ) {
        const std::string& id = (args.empty() ? std::string() : args[0]);
        unsigned short count = 0;
        TWiredTigerAutoincrementKey key;
        TOplogData data;
        uint64_t maxId;
        TBlob out;
        out << "{\"ok\":true,\"data\":[";

        auto it = Read(request->Db(), id);

        while ((count < 1000) && it.Next(key, data)) {
            maxId = key.GetId();
            const auto& data_ = data.GetData();

            if (count == 0) {
                out.Reserve(data_.size() * 2 * 1000);
            }

            out << data_ << ",";

            ++count;
        }

        if (count > 0) {
            out.Shrink(out.Size() - 1);
            out << "],\"last\":" << std::to_string(maxId);

        } else if (id.empty()) {
            out << "],\"last\":null";

        } else {
            request->SendJson(nlohmann::json {{"ok", false}});
            return;
        }

        out << "}";

        auto response = request->Respond200();
        response.Header("Content-Type", "application/json");
        response.Wrap(out.Size(), out.Data(), /* own = */true);
        out.Reset();

        request->Send(std::move(response));
    }
}
