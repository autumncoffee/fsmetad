#include "get.hpp"
#include <models/files.hpp>
#include <ac-common/file.hpp>

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
            auto file = std::make_shared<TFile>(data.GetPath());

            if (file->IsOK()) {
                auto response = request->Respond200();

                response.Header("Content-Type", data.GetCType());
                response.Wrap(file->Size(), file->Data());
                response.Memorize(file);

                request->Send(std::move(response));
                return;
            }
        }

        request->Send404();
    }
}
