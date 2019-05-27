#include "get.hpp"
#include <models/files.hpp>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

namespace {
    class TFile {
    public:
        TFile() = delete;
        TFile(const TFile&) = delete;

        TFile(const std::string& path) {
            Fh = open(path.data(), O_RDONLY);

            if (Fh == -1) {
                perror("open");
                return;
            }

            {
                struct stat buf;

                if (fstat(Fh, &buf) == -1) {
                    perror("fstat");
                    return;
                }

                Len_ = buf.st_size;
            }

            if (Len_ == 0) {
                return;
            }

            Addr_ = mmap(nullptr, Len_, PROT_READ, MAP_SHARED, Fh, 0);

            if (Addr_ == MAP_FAILED) {
                perror("mmap");
                return;
            }

            Ok = true;
        }

        bool IsOK() const {
            return Ok;
        }

        ~TFile() {
            if (Ok) {
                munmap(Addr_, Len_);
            }

            if (Fh != -1) {
                close(Fh);
            }
        }

        TFile(TFile&& right) {
            Fh = right.Fh;
            Len_ = right.Len_;
            Addr_ = right.Addr_;
            Ok = right.Ok;

            right.Fh = -1;
            right.Len_ = 0;
            right.Addr_ = nullptr;
            right.Ok = false;
        }

        TFile& operator=(const TFile&) = delete;

        TFile& operator=(TFile&& right) {
            Fh = right.Fh;
            Len_ = right.Len_;
            Addr_ = right.Addr_;
            Ok = right.Ok;

            right.Fh = -1;
            right.Len_ = 0;
            right.Addr_ = nullptr;
            right.Ok = false;

            return *this;
        }

        const char* Data() const {
            return (const char*)Addr_;
        }

        size_t Size() const {
            return Len_;
        }

    private:
        int Fh = -1;
        size_t Len_ = 0;
        void* Addr_ = nullptr;
        bool Ok = false;
    };
}

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
                response.Wrap(file->Size(), (char*)file->Data());
                response.Memorize(file);

                request->Send(std::move(response));
                return;
            }
        }

        request->Send404();
    }
}
