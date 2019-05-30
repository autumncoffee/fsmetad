#include "untag.hpp"
#include <models/files.hpp>
#include <models/oplog.hpp>

namespace NAC {
    void TFileV1UntagHandler::Handle(
        const std::shared_ptr<TFSMetaDRequest> request,
        const std::vector<std::string>& args
    ) {
        if (args.size() != 2) {
            request->Send400();
            return;
        }

        TFilesKey key;
        key.SetId(args[0] + "/" + args[1]);

        nlohmann::json op;
        auto conn = request->Db();
        auto tx = conn.Begin();
        const bool ok = conn.Remove<TFileTagsModel>(key);

        if (!ok) {
            request->Send500();
            return;
        }

        op["d"].push_back({
            {"m", TFileTagsModel::DBName},
            {"o", "r"},
            {"d", {
                {"Id", key.GetId()},
            }},
        });

        if (!TOplogModel::Save(conn, op.dump())) {
            request->Send500();
            return;
        }

        if (!tx.Commit()) {
            request->Send500();
            return;
        }

        nlohmann::json out;
        out["ok"] = ok;

        request->SendJson(std::move(out));
    }
}
