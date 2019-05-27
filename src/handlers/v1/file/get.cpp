#include "get.hpp"
#include <models/files.hpp>

namespace NAC {
    void TFileV1GetHandler::Handle(
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

        if (request->Db().Get<TFilesModel>(key, data)) {
            nlohmann::json out;
            out["id"] = args[0];
            out["name"] = data.GetName();
            out["ctype"] = data.GetCType();

            request->SendJson(std::move(out));

        } else {
            request->Send404();
        }
    }
}
