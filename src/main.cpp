#include <handlers/v1/file/new.hpp>
#include <handlers/v1/file/get.hpp>
#include <handlers/v1/file/tag.hpp>
#include <handlers/v1/file/untag.hpp>
#include <handlers/v1/file/info.hpp>
#include <handlers/v1/file/remove.hpp>
#include <handlers/v1/repl/oplog.hpp>
#include <ac-library/http/server/server.hpp>
#include <ac-library/http/server/client.hpp>
#include <ac-library/http/router/router.hpp>
#include <stdlib.h>
#include "request.hpp"
#include <ac-library/wiredtiger/connection.hpp>
#include <models/files.hpp>
#include <models/oplog.hpp>
#include <ac-common/utils/string.hpp>

using namespace NAC;

int main() {
    unsigned short nodeNum(0);

    {
        const char* num(getenv("NODE_NUM"));

        if (num) {
            NStringUtils::FromString(num, nodeNum);
        }
    }

    TWiredTiger db(getenv("DB_PATH"));

    {
        auto&& session = db.Open();
        session.CreateTable<TFilesModel>();
        session.CreateTable<TFileTagsModel>();
        session.CreateIndex<TFileTagsIndex>();
        session.CreateIndex<TFileTagsSecondIndex>();

        session.CreateTable<TOplogModel>();
    }

    auto v1FileTagHandler = std::make_shared<TFileV1TagHandler>();
    auto v1routerFile = std::make_shared<NHTTPRouter::TRouter>();
    v1routerFile->Add("^new/*$", std::make_shared<TFileV1NewHandler>(nodeNum));
    v1routerFile->Add("^get/([a-z0-9]+)(?:/[^/]+)?/*$", std::make_shared<TFileV1GetHandler>());
    v1routerFile->Add("^tag/([a-z0-9]+)/*$", v1FileTagHandler);
    v1routerFile->Add("^tag/([^/]+)(?:/([a-z0-9]+))?/*$", v1FileTagHandler);
    v1routerFile->Add("^untag/([a-z0-9]+)/([^/]+)/*$", std::make_shared<TFileV1UntagHandler>());
    v1routerFile->Add("^info/([a-z0-9]+)/*$", std::make_shared<TFileV1InfoHandler>());
    v1routerFile->Add("^remove/([a-z0-9]+)/*$", std::make_shared<TFileV1RemoveHandler>());

    auto v1router = std::make_shared<NHTTPRouter::TRouter>();
    v1router->Add("^file/", v1routerFile);
    v1router->Add("^repl/oplog(?:/([0-9]+))?/*$", std::make_shared<TReplV1OplogHandler>());

    NHTTPRouter::TRouter router;
    router.Add("^/v1/", v1router);

    NHTTPServer::TServer::TArgs args;

    args.BindIP4 = getenv("BIND_V4");
    args.BindIP6 = getenv("BIND_V6");
    args.BindPort6 = args.BindPort4 = NStringUtils::FromString<unsigned short>(getenv("BIND_PORT"));
    args.ThreadCount = 10;
    args.ClientArgsFactory = [&router, &db]() {
        return new NHTTPServer::TClient::TArgs(router, [&db](
            std::shared_ptr<NHTTPLikeParser::TParsedData> data,
            const NHTTPServer::TResponder& responder
        ) {
            return (NHTTP::TRequest*)(new TFSMetaDRequest(db, data, responder));
        });
    };

    NHTTPServer::TServer(args, router).Run();

    return 0;
}
