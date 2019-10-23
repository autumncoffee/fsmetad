#include "get.hpp"
#include <models/files.hpp>
#include <ac-common/string_sequence.hpp>
#include <ac-library/http/utils/multipart.hpp>
#include <deque>
#include <ac-common/str.hpp>
#include <tuple>

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
                    if (syncInfo.GetOffset() >= syncInfo.GetSize()) {
                        conn.Remove<TFilesSyncInfoModel>(key);

                    } else {
                        request->Send404();
                        return;
                    }
                }
            }

            request->SendFile(
                data.GetPath(),
                data.GetCType(),
                data.GetSize(),
                data.GetOffset()
            );

        } else {
            request->Send404();
        }
    }
}
