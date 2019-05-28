#include "new.hpp"
#include <models/id.hpp>
#include <models/files.hpp>

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

        uint16_t attempt = 0;
        std::string id;
        auto conn = request->Db();
        TFilesKey key;
        bool ok = false;

        while (attempt < 20) {
            id = GenerateID(attempt);
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

        if (json.count("tags") > 0) {
            auto tags = json["tags"].get<std::vector<std::string>>();
            TFileTagsData data;

            for (const auto& tag : tags) {
                key.SetId(id + "/" + tag);

                data.SetTag(tag);
                data.SetFile(id);

                conn.Set<TFileTagsModel>(key, data);
            }
        }

        nlohmann::json out;
        out["id"] = std::move(id);

        request->SendJson(std::move(out));
    }
}
