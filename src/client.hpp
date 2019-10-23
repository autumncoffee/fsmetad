#pragma once

#include <ac-library/http/server/client.hpp>

namespace NAC {
    class TFSMetaDClient : public NHTTPServer::TClient {
    public:
        using NHTTPServer::TClient::TClient;

    protected:
        void HandleFrame(
            std::shared_ptr<NWebSocketParser::TFrame> frame,
            std::shared_ptr<NHTTP::TRequest> origin
        ) override;
    };
}
