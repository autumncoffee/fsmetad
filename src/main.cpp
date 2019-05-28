#include <handlers/v1/file/new.hpp>
#include <handlers/v1/file/get.hpp>
#include <handlers/v1/file/tag.hpp>
#include <handlers/v1/file/untag.hpp>
#include <handlers/v1/file/info.hpp>
#include <handlers/v1/file/remove.hpp>
#include <ac-library/http/server/server.hpp>
#include <ac-library/http/server/client.hpp>
#include <ac-library/http/router/router.hpp>
#include <stdlib.h>
#include "request.hpp"
#include <ac-library/wiredtiger/connection.hpp>
#include <models/files.hpp>

using namespace NAC;

int main() {
    TWiredTiger db(getenv("DB_PATH"));

    {
        auto&& session = db.Open();
        session.CreateTable<TFilesModel>();
        session.CreateTable<TFileTagsModel>();
        session.CreateIndex<TFileTagsIndex>();
        session.CreateIndex<TFileTagsSecondIndex>();
    }

    auto v1FileTagHandler = std::make_shared<TFileV1TagHandler>();
    auto v1router = std::make_shared<NHTTPRouter::TRouter>();
    v1router->Add("^file/new/*$", std::make_shared<TFileV1NewHandler>());
    v1router->Add("^file/get/([a-z0-9]+)(?:/[^/]+)?/*$", std::make_shared<TFileV1GetHandler>());
    v1router->Add("^file/tag/([a-z0-9]+)/*$", v1FileTagHandler);
    v1router->Add("^file/tag/([^/]+)(?:/([a-z0-9]+))?/*$", v1FileTagHandler);
    v1router->Add("^file/untag/([a-z0-9]+)/([^/]+)/*$", std::make_shared<TFileV1UntagHandler>());
    v1router->Add("^file/info/([a-z0-9]+)/*$", std::make_shared<TFileV1InfoHandler>());
    v1router->Add("^file/remove/([a-z0-9]+)/*$", std::make_shared<TFileV1RemoveHandler>());

    NHTTPRouter::TRouter router;
    router.Add("^/v1/", v1router);

    NHTTPServer::TServer::TArgs args;

    args.BindIP4 = getenv("BIND_V4");
    args.BindIP6 = getenv("BIND_V6");
    args.BindPort6 = args.BindPort4 = atoi(getenv("BIND_PORT"));
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
