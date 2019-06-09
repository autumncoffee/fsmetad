#include "new.hpp"
#include <models/id.hpp>
#include <models/files.hpp>
#include <models/oplog.hpp>

#ifdef RELEASE_FILESYSTEM
#include <filesystem>
#else
#include <experimental/filesystem>

namespace std {
    namespace filesystem = std::experimental::filesystem;
}
#endif

namespace NAC {
    void TFileV1NewHandler::Handle(
        const std::shared_ptr<TFSMetaDRequest> request,
        const std::vector<std::string>& args
    ) {
        const auto& json = request->Json();
        auto path = json["path"].get<std::string>();

        if (!std::filesystem::is_regular_file(path)) {
            request->Send400();
            return;
        }

        TFilesData data;
        data
            .SetName(json["name"].get<std::string>())
            .SetPath(path)
            .SetCType(json["ctype"].get<std::string>())
            .SetSize(0)
            .SetOffset(0)
        ;

        unsigned char attempt = 0;
        std::string id;
        auto conn = request->Db();
        TFilesKey key;
        bool ok = false;
        auto tx = conn.Begin();

        while (attempt < 20) {
            id = GenerateID(NodeNum);
            key.SetId(id);

            if (conn.Insert<TFilesModel>(key, data)) {
                ok = true;
                break;
            }

            ++attempt;
        }

        if (!ok) {
            request->Send500();
            return;
        }

        nlohmann::json op;
        op["d"].push_back({
            {"m", TFilesModel::DBName},
            {"o", "i"},
            {"d", {
                {"Id", id},
                {"Name", data.GetName()},
                {"Path", data.GetPath()},
                {"CType", data.GetCType()},
                {"Size", data.GetSize()},
                {"Offset", data.GetOffset()},
            }},
        });

        if (json.count("tags") > 0) {
            auto tags = json["tags"].get<std::vector<std::string>>();
            TFileTagsData data;

            for (const auto& tag : tags) {
                key.SetId(id + "/" + tag);

                data.SetTag(tag);
                data.SetFile(id);

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
        out["id"] = std::move(id);

        request->SendJson(std::move(out));
    }
}
