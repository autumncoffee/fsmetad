#include "info.hpp"
#include <models/files.hpp>
#include <ac-common/file.hpp>

namespace NAC {
    void TFileV1InfoHandler::Handle(
        const std::shared_ptr<TFSMetaDRequest> request,
        const std::vector<std::string>& args
    ) {
        if (args.size() != 1) {
            request->Send400();
            return;
        }

        TFilesKey key;
        key.SetId(args[0]);

        TFilesData data;
        auto conn = request->Db();

        if (conn.Get<TFilesModel>(key, data)) {
            TFile file(data.GetPath(), TFile::ACCESS_INFO);

            if (file.IsOK()) {
                nlohmann::json out;
                out["id"] = args[0];
                out["name"] = data.GetName();
                out["ctype"] = data.GetCType();
                out["tags"] = nlohmann::json::array();

                {
                    size_t size = data.GetSize();

                    if (size == 0) {
                        size = file.Size();
                    }

                    out["size"] = size;
                }

                {
                    TFileTagsSecondIndexKey tagIndexKey;
                    tagIndexKey.SetFile(args[0]);

                    auto it = conn.Search<TFileTagsSecondIndex>(tagIndexKey);
                    TFileTagsData tag;

                    while (it.Next(tag)) {
                        if (tag.GetFile() != args[0]) {
                            break;
                        }

                        out["tags"].push_back(tag.GetTag());
                    }
                }

                request->SendJson(std::move(out));
                return;
            }
        }

        request->Send404();
    }
}
