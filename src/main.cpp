#include <handlers/v1/file/new.hpp>
#include <handlers/v1/file/get.hpp>
#include <ac-library/http/server/server.hpp>
#include <ac-library/http/server/client.hpp>
#include <ac-library/http/router/router.hpp>
#include <stdlib.h>
#include "request.hpp"
#include <db/connection.hpp>
#include <models/files.hpp>

using namespace NAC;

int main() {
    TFSMetaDB db(getenv("DB_PATH"));

    {
        auto&& session = db.Open();
        session.CreateTable<TFilesModel>();
        session.CreateTable<TFileTagsModel>();
        session.CreateIndex<TFileTagsIndex>();
    }

    auto v1router = std::make_shared<NHTTPRouter::TRouter>();
    v1router->Add("^file/new/*$", std::make_shared<TFileV1NewHandler>());
    v1router->Add("^file/get/([a-z0-9]+)(?:/[^/]+)?/*$", std::make_shared<TFileV1GetHandler>());

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
