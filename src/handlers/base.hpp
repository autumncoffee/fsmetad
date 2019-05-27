#pragma once

#include <ac-library/http/handler/handler.hpp>
#include <request.hpp>

namespace NAC {
    class TBaseHandler : public NHTTPHandler::THandler {
    public:
        virtual void Handle(
            const std::shared_ptr<TFSMetaDRequest> request,
            const std::vector<std::string>& args
        ) = 0;

        void Handle(
            const std::shared_ptr<NHTTP::TRequest> request,
            const std::vector<std::string>& args
        ) override {
            Handle(std::shared_ptr<TFSMetaDRequest>(request, (TFSMetaDRequest*)request.get()), args);
        }
    };
}
