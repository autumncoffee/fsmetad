#include "sync.hpp"
#include <models/files.hpp>
#include <ac-common/string_sequence.hpp>
#include <ac-library/http/utils/multipart.hpp>
#include <deque>
#include <ac-common/str.hpp>
#include <tuple>
#include <utils.hpp>
#include <stdio.h>
#include <iostream>

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
            TFilesData fileData;

            if (conn.Get<TFilesModel>(key, fileData)) {
                TFile file(fileData.GetPath(), TFile::ACCESS_INFO);

                if (file) {
                    request->WebSocketStart();
                    request->SetSyncFileID(args[0]);
                    request->Send(FileSyncFrameV1(file.Size(), file.Size()));

                    return;
                }
            }

            request->Send404();
            return;
        }

        if (data.GetOffset() >= data.GetSize()) {
            TFilesData fileData;

            if (!conn.Get<TFilesModel>(key, fileData)) {
                conn.Remove<TFilesSyncInfoModel>(key);
                std::cerr << "file does not exist" << std::endl;
                request->Send404();
                return;
            }

            if (rename(fileData.GetTempPath().c_str(), fileData.GetPath().c_str()) == 0) {
                conn.Remove<TFilesSyncInfoModel>(key);

            } else {
                perror("rename");
                request->Send500();
                return;
            }
        }

        request->WebSocketStart();
        request->SetSyncFileID(args[0]);
        request->Send(FileSyncFrameV1(data.GetSize(), data.GetOffset()));
    }
}
