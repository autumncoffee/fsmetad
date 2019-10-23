#include "sync.hpp"
#include <models/files.hpp>
#include <ac-common/string_sequence.hpp>
#include <ac-library/http/utils/multipart.hpp>
#include <deque>
#include <ac-common/str.hpp>
#include <tuple>
#include <utils.hpp>

namespace NAC {
    void TFileV1SyncHandler::Handle(
        const std::shared_ptr<TFSMetaDRequest> request,
        const std::vector<std::string>& args
    ) {
        if ((args.size() != 1) || !request->IsWebSocketHandshake()) {
            request->Send400();
            return;
        }

        TFilesKey key;
        key.SetId(args[0]);

        TFilesSyncInfo data;
        auto&& conn = request->Db();

        if (!conn.Get<TFilesSyncInfoModel>(key, data)) {
            request->Send404();
            return;
        }

        if (data.GetOffset() >= data.GetSize()) {
            conn.Remove<TFilesSyncInfoModel>(key);
            request->Send404();
            return;
        }

        request->WebSocketStart();
        request->SetSyncFileID(args[0]);
        request->Send(FileSyncFrameV1(data.GetSize(), data.GetOffset()));
    }
}
