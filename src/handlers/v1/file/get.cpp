#include "get.hpp"
#include <models/files.hpp>
#include <ac-common/string_sequence.hpp>
#include <ac-library/http/utils/multipart.hpp>
#include <deque>
#include <ac-common/str.hpp>
#include <tuple>
#include <ac-library/http/urlescape.hpp>

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
        auto&& conn = request->Db();

        if (conn.Get<TFilesModel>(key, data)) {
            {
                TFilesSyncInfo syncInfo;

                if (conn.Get<TFilesSyncInfoModel>(key, syncInfo)) {
                    request->Send404();
                    return;
                }
            }

            auto response = request->RespondFile(
                data.GetPath(),
                data.GetCType(),
                data.GetSize(),
                data.GetOffset()
            );

            if (request->QueryParam("dl") == std::string("1")) {
                response.Header("Content-Disposition", std::string("attachment; filename=") + (std::string)NHTTP::URLEscape(data.GetName()));
            }

            {
                const auto& contentEncoding = request->HeaderValue("x-fsmd-ce");

                if (!contentEncoding.empty()) {
                    response.Header("Content-Encoding", contentEncoding);
                }
            }

            request->Send(response);

        } else {
            request->Send404();
        }
    }
}
