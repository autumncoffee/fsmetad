#include "get.hpp"
#include <models/files.hpp>
#include <ac-common/file.hpp>
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

        if (request->Db().Get<TFilesModel>(key, data)) {
            const bool isHead = (request->Method() == "head");
            auto file = std::make_shared<TFile>(data.GetPath(), (isHead ? TFile::ACCESS_INFO : TFile::ACCESS_RDONLY));

            if (file->IsOK()) {
                if (isHead) {
                    auto response = request->Respond200();
                    response.DoNotAddContentLength();
                    response.Header("Content-Length", std::to_string(file->Size()));
                    response.Header("Accept-Ranges", "bytes");
                    response.Header("Content-Type", data.GetCType());
                    request->Send(std::move(response));

                } else {
                    auto range = request->Range();

                    if (range.Ranges.empty()) {
                        auto response = request->Respond200();
                        response.Header("Content-Type", data.GetCType());
                        response.Wrap(file->Size(), file->Data());
                        response.Memorize(file);
                        request->Send(std::move(response));

                    } else {
                        const bool onlyOnePart(range.Ranges.size() == 1);
                        auto response = request->Respond206();
                        response.Memorize(file);

                        for (const auto& spec : range.Ranges) {
                            char* start = nullptr;
                            size_t size = 0;
                            TMaybe<size_t> end(spec.End);
                            unsigned char sizeMod = 0;

                            if (end) {
                                if (spec.Start) {
                                    ++(*end);
                                    ++sizeMod;
                                }

                                if (*end > file->Size()) {
                                    request->Send416();
                                    return;
                                }
                            }

                            if (spec.Start) {
                                if (end) {
                                    size = *end - *spec.Start;

                                } else {
                                    size = file->Size() - *spec.Start;
                                }

                                start = file->Data() + *spec.Start;

                            } else {
                                size = *end;
                                start = file->Data() + file->Size() - size;
                            }

                            const size_t fromByte(start - file->Data());
                            auto part = (onlyOnePart ? std::move(response) : NHTTP::TResponse());

                            part.Header("Content-Type", data.GetCType());
                            part.Header("Content-Range", range.Unit + " " + std::to_string(fromByte) + "-" + std::to_string(fromByte + size - sizeMod) + "/" + std::to_string(file->Size()));
                            part.Wrap(size, start);

                            if (onlyOnePart) {
                                request->Send(std::move(part));
                                return;
                            }

                            response.AddPart(std::move(part));
                        }

                        response.Header("Content-Type", "multipart/byteranges");
                        request->Send(std::move(response));
                    }
                }

                return;
            }
        }

        request->Send404();
    }
}
