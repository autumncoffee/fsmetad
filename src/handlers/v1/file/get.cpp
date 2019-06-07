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
                        std::deque<std::tuple<size_t, size_t, TBlob>> parts;

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
                            parts.emplace_back(fromByte, fromByte + size - sizeMod, TBlob(size, start));
                        }

                        auto response = request->Respond206();

                        if (parts.size() == 1) {
                            const auto& [ from, to, blob ] = parts.front();

                            response.Header("Content-Type", data.GetCType());
                            response.Header("Content-Range", range.Unit + " " + std::to_string(from) + "-" + std::to_string(to) + "/" + std::to_string(file->Size()));
                            response.Memorize(file);
                            response.Wrap(blob.Size(), blob.Data());
                            request->Send(std::move(response));

                        } else {
                            TBlobSequence body;
                            body.Memorize(file);
                            const std::string boundary(NHTTPUtils::Boundary());
                            bool isFirst(true);
                            size_t contentLength = 0;

                            for (const auto& [ from, to, blob ] : parts) {
                                TBlob preamble;
                                preamble
                                    << std::string(isFirst ? "" : "\r\n") << "--" << boundary << "\r\n"
                                    << "Content-Type: " << data.GetCType() << "\r\n"
                                    << "Content-Range: " << range.Unit << " " << std::to_string(from) << "-" << std::to_string(to) << "/" << std::to_string(file->Size()) << "\r\n"
                                    << "\r\n"
                                ;

                                contentLength += preamble.Size();
                                contentLength += blob.Size();

                                body.Concat(std::move(preamble));
                                body.Concat(blob);

                                isFirst = false;
                            }

                            {
                                TBlob end;
                                end << "\r\n--" << boundary << "--\r\n";

                                contentLength += end.Size();
                                body.Concat(std::move(end));
                            }

                            response.DoNotAddContentLength();
                            response.Header("Content-Type", "multipart/byteranges; boundary=" + boundary);
                            response.Header("Content-Length", std::to_string(contentLength));

                            auto preamble = response.Preamble();
                            preamble << "\r\n";

                            request->Send(std::move(preamble));
                            request->Send(std::move(body));
                        }
                    }
                }

                return;
            }
        }

        request->Send404();
    }
}
