#include "new.hpp"
#include <models/id.hpp>
#include <models/files.hpp>
#include <models/oplog.hpp>
#include <ac-common/file.hpp>
#include <stdlib.h>
#include <fs_wrapper.hpp>

namespace NAC {
    void TFileV1NewHandler::Handle(
        const std::shared_ptr<TFSMetaDRequest> request,
        const std::vector<std::string>& args
    ) {
        const auto& json = request->Json();
        std::string path;
        uint64_t size(0);
        uint64_t toSync(0);

        if (json.count("path") > 0) {
            path = json["path"].get<std::string>();

            if (!std::filesystem::is_regular_file(path)) {
                request->Send400();
                return;
            }

        } else if (json.count("size") > 0) {
            size = json["size"].get<uint64_t>();

            if (size == 0) {
                request->Send400();
                return;
            }

            const char* root_ = getenv("FS_ROOT");

            if (!root_) {
                request->Send500();
                return;
            }

            std::filesystem::path root(root_);

            for (size_t i = 0; i < 10; ++i) {
                auto filename = GenerateID(NodeNum);
                path = (root / filename).string();
                TFile file(path, TFile::ACCESS_CREATEX);

                if (!file) {
                    continue;
                }

                TFile tmpFile((root / ("." + filename)).string(), TFile::ACCESS_CREATEX);
                tmpFile.Resize(size);

                if (!tmpFile) {
                    request->Send500();
                    return;
                }

                toSync = size;
                size = 0;
                break;
            }

            if (size != 0) {
                request->Send500();
                return;
            }

        } else {
            request->Send400();
            return;
        }

        TFilesData data;
        data
            .SetName(json["name"].get<std::string>())
            .SetPath(path)
            .SetCType(json["ctype"].get<std::string>())
            .SetSize(size)
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

        if (toSync > 0) {
            TFilesSyncInfo fileSyncInfo;
            fileSyncInfo
                .SetSize(toSync)
                .SetOffset(0)
            ;

            if (!conn.Insert<TFilesSyncInfoModel>(key, fileSyncInfo)) {
                request->Send500();
                return;
            }
        }

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
