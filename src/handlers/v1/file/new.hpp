#pragma once

#include "base.hpp"

namespace NAC {
    class TFileV1NewHandler : public TFileV1BaseHandler {
    public:
        TFileV1NewHandler() = delete;

        TFileV1NewHandler(unsigned short nodeNum)
            : TFileV1BaseHandler()
            , NodeNum(nodeNum)
        {
        }

        void Handle(
            const std::shared_ptr<TFSMetaDRequest> request,
            const std::vector<std::string>& args
        ) override;

    private:
        unsigned short NodeNum;
    };
}
