#include "name.hpp"
#include <models/files.hpp>
#include <models/id.hpp>
#include <models/oplog.hpp>
#include <stdio.h>
#include <ac-library/http/urlescape.hpp>
#include <ac-common/utils/string.hpp>
#include <string.h>

namespace NAC {
    namespace {
        template<bool fuzzy, typename TKey>
        nlohmann::json MakeOutput(
            TWiredTigerIterator&& it,
            const std::string& filename,
            bool first
        ) {
            TKey key;
            TFilesData data;
            size_t counter = 0;
            nlohmann::json out;
            out["results"] = nlohmann::json::array();

            while ((counter < 20) && it.Next(key, data)) {
                if constexpr (fuzzy) {
                    std::string name(data.GetName());
                    NStringUtils::ToLower(name);

                    if (!strstr(name.c_str(), filename.c_str())) {
                        continue;
                    }

                } else {
                    if (data.GetName() != filename) {
                        break;
                    }
                }

                if (first) {
                    first = false;
                    continue;
                }

                ++counter;

                nlohmann::json node;
                node["id"] = key.GetId();

                if (fuzzy) {
                    node["name"] = data.GetName();
                }

                out["results"].push_back(std::move(node));
            }

            return out;
        }
    }

    void TFileV1NameHandler::Handle(
        const std::shared_ptr<TFSMetaDRequest> request,
        const std::vector<std::string>& args
    ) {
        if ((args.size() != 1) && (args.size() != 2)) {
            request->Send400();
            return;
        }

        std::string filename(args[0]);
        NHTTP::URLUnescape(filename);

        const bool fuzzy(request->QueryParam("like") == std::string("1"));

        if (fuzzy) {
            NStringUtils::ToLower(filename);

            if ((args.size() == 2) && !args[1].empty()) {
                TFilesKey key;
                key.SetId(args[1]);

                request->SendJson(MakeOutput<true, TFilesKey>(
                    request->Db().Search<TFilesModel>(key, -1),
                    filename,
                    true
                ));

            } else {
                request->SendJson(MakeOutput<true, TFilesKey>(
                    request->Db().SeqScan<TFilesModel>(-1),
                    filename,
                    false
                ));
            }

            return;
        }

        TFileNameIndexKey key;
        bool first = true;

        if ((args.size() == 2) && !args[1].empty()) {
            key.SetName(filename);
            key.SetId(args[1]);

        } else {
            key.SetName(filename + '\1');
            key.SetId(std::string());
            first = false;
        }

        request->SendJson(MakeOutput<false, TFileNameIndexKey>(
            request->Db().Search<TFileNameIndex>(key, -1),
            filename,
            first
        ));
    }
}
