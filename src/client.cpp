#include "client.hpp"
#include "request.hpp"
#include "utils.hpp"
#include <models/files.hpp>
#include <ac-common/utils/htonll.hpp>
#include <ac-common/file.hpp>
#include <iostream>

namespace NAC {
    void TFSMetaDClient::HandleFrame(
        std::shared_ptr<NWebSocketParser::TFrame> frame,
        std::shared_ptr<NHTTP::TRequest> origin
    ) {
        // std::cerr << "new frame, size: " << std::to_string(frame->Payload.Size()) << std::endl;

        if ((frame->Opcode != 2) && (frame->Opcode != 0)) {
            Drop();
            std::cerr << "invalid websocket opcode: " << std::to_string(frame->Opcode) << std::endl;
            return;
        }

        auto&& request = *(TFSMetaDRequest*)origin.get();

        request.SyncFileBuf().Append(frame->Payload.Size(), frame->Payload.Data());

        if (!frame->IsFin) {
            return;
        }

        request.SyncFileBuf().Finish();
        TMemDisk syncFileBuf(16, "/tmp/fsmetadsync.XXXXXXXXXX");
        std::swap(request.SyncFileBuf(), syncFileBuf);

        const auto& syncFileID = request.GetSyncFileID();

        if (syncFileID.empty()) {
            Drop();
            std::cerr << "no SyncFileID" << std::endl;
            return;
        }

        TFilesKey key;
        key.SetId(syncFileID);

        TFilesSyncInfo data;
        auto&& conn = request.Db();

        if (!conn.Get<TFilesSyncInfoModel>(key, data)) {
            Drop();
            std::cerr << "file should not be synced" << std::endl;
            return;
        }

        if (data.GetOffset() >= data.GetSize()) {
            conn.Remove<TFilesSyncInfoModel>(key);
            Drop();
            std::cerr << "file is already synced" << std::endl;
            return;
        }

        if (syncFileBuf.Size() < sizeof(TFileSyncFrameHeader)) {
            Drop();
            std::cerr << "payload is too small" << std::endl;
            return;
        }

        const char* frameData = syncFileBuf.Data();
        TFileSyncFrameHeader header;

        memcpy(&header.Offset, frameData, sizeof(header.Offset));
        memcpy(&header.Size, frameData + sizeof(header.Offset), sizeof(header.Size));

        header.Offset = ntoh(header.Offset);
        header.Size = ntoh(header.Size);

        if (
            (header.Offset != data.GetOffset())
            || ((header.Size + header.Offset) > data.GetSize())
            || ((header.Size + sizeof(header)) != syncFileBuf.Size())
        ) {
            Drop();
            std::cerr << "invalid header or info" << std::endl;
            return;
        }

        TFilesData fileData;

        if (!conn.Get<TFilesModel>(key, fileData)) {
            conn.Remove<TFilesSyncInfoModel>(key);
            Drop();
            std::cerr << "file does not exist" << std::endl;
            return;
        }

        TFile file(fileData.GetPath(), TFile::ACCESS_RDWR);

        if (!file) {
            Drop();
            std::cerr << "file has vanished from disk" << std::endl;
            return;
        }

        if (file.Size() != data.GetSize()) {
            conn.Remove<TFilesSyncInfoModel>(key);
            Drop();
            std::cerr << "size mismatch" << std::endl;
            return;
        }

        memcpy(file.Data() + header.Offset, frameData + sizeof(header), header.Size);

        file.FSync();

        data.SetOffset(header.Offset + header.Size);

        if (data.GetOffset() < data.GetSize()) {
            conn.Set<TFilesSyncInfoModel>(key, data);

        } else {
            conn.Remove<TFilesSyncInfoModel>(key);
            // std::cerr << "DONE: " << std::to_string(data.GetOffset()) << "/" << std::to_string(data.GetSize()) << std::endl;
        }

        request.Send(FileSyncFrameV1(data.GetSize(), data.GetOffset()));
    }
}
