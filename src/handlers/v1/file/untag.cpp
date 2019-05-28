#include "untag.hpp"
#include <models/files.hpp>

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

        nlohmann::json out;
        out["ok"] = request->Db().Remove<TFileTagsModel>(key);

        request->SendJson(std::move(out));
    }
}
