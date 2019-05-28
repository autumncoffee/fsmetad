#include "remove.hpp"
#include <models/files.hpp>
#include <ac-common/file.hpp>
#include <deque>
#include <unistd.h>
#include <stdio.h>

namespace NAC {
    void TFileV1RemoveHandler::Handle(
        const std::shared_ptr<TFSMetaDRequest> request,
        const std::vector<std::string>& args
    ) {
        if (args.size() != 1) {
            request->Send400();
            return;
        }

        TFilesKey key;
        auto conn = request->Db();

        {
            TFileTagsSecondIndexKey tagIndexKey;
            tagIndexKey.SetFile(args[0]);

            auto it = conn.Search<TFileTagsSecondIndex>(tagIndexKey);
            TFileTagsData tag;
            std::deque<std::string> toRemove;

            while (it.Next(tag)) {
                if (tag.GetFile() != args[0]) {
                    break;
                }

                toRemove.push_back(args[0] + "/" + tag.GetTag());
            }

            for (const auto& id : toRemove) {
                key.SetId(id);
                conn.Remove<TFileTagsModel>(key);
            }
        }

        key.SetId(args[0]);
        TFilesData data;

        if (conn.Get<TFilesModel>(key, data)) {
            const auto& path = data.GetPath();
            TFile file(path, TFile::ACCESS_INFO);

            if (file.IsOK()) {
                if (unlink(path.c_str()) != 0) {
                    perror("unlink");

                    request->Send500();
                    return;
                }
            }

            conn.Remove<TFilesModel>(key);
        }

        nlohmann::json out;
        out["ok"] = true;

        request->SendJson(std::move(out));
    }
}
