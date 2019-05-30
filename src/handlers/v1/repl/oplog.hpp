#pragma once

#include "base.hpp"

namespace NAC {
    class TReplV1OplogHandler : public TReplV1BaseHandler {
    public:
        void Handle(
            const std::shared_ptr<TFSMetaDRequest> request,
            const std::vector<std::string>& args
        ) override;
    };
}
