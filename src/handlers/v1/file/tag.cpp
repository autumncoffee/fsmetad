#include "tag.hpp"
#include <models/files.hpp>
#include <models/id.hpp>
#include <models/oplog.hpp>
#include <stdio.h>

namespace NAC {
    void TFileV1TagHandler::Handle(
        const std::shared_ptr<TFSMetaDRequest> request,
        const std::vector<std::string>& args
    ) {
        if (request->IsPost()) {
            if (args.size() != 1) {
                request->Send400();
                return;
            }

            TFilesKey key;
            key.SetId(args[0]);

            auto conn = request->Db();

            {
                TFilesData data;

                if (!conn.Get<TFilesModel>(key, data)) {
                    request->Send404();
                    return;
                }
            }

            const auto& json = request->Json();
            auto tags = json["tags"].get<std::vector<std::string>>();
            TFileTagsData data;
            nlohmann::json op;
            auto tx = conn.Begin();

            for (const auto& tag : tags) {
                key.SetId(args[0] + "/" + tag);

                data.SetTag(tag);
                data.SetFile(args[0]);

                if (!conn.Set<TFileTagsModel>(key, data)) {
                    request->Send500();
                    return;
                }

                op["d"].push_back({
                    {"m", TFileTagsModel::DBName},
                    {"o", "i"},
                    {"d", {
                        {"Id", key.GetId()},
                        {"Tag", data.GetTag()},
                        {"File", data.GetFile()},
                    }},
                });
            }

            if (!TOplogModel::Save(conn, op.dump())) {
                request->Send500();
                return;
            }

            if (!tx.Commit()) {
                request->Send500();
                return;
            }

            nlohmann::json out;
            out["ok"] = true;

            request->SendJson(std::move(out));

        } else {
            if ((args.size() != 1) && (args.size() != 2)) {
                request->Send400();
                return;
            }

            TFileTagsData key;
            bool first = true;

            if ((args.size() == 2) && !args[1].empty()) {
                key.SetTag(args[0]);
                key.SetFile(args[1]);

            } else {
                key.SetTag(args[0] + '\1');
                key.SetFile(std::string());
                first = false;
            }

            auto it = request->Db().Search<TFileTagsIndex>(key, -1);
            TFileTagsData data;
            size_t counter = 0;
            nlohmann::json out;
            out["results"] = nlohmann::json::array();

            while ((counter < 20) && it.Next(data)) {
                if (data.GetTag() != args[0]) {
                    break;
                }

                if (first) {
                    first = false;
                    continue;
                }

                ++counter;

                nlohmann::json node;
                node["id"] = data.GetFile();

                out["results"].push_back(std::move(node));
            }

            request->SendJson(std::move(out));
        }
    }
}
