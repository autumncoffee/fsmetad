#include "request.hpp"

namespace NAC {
    const nlohmann::json& TFSMetaDRequest::Json() {
        if (!Json_) {
            Json_.reset(new nlohmann::json);

            if (ContentLength() > 0) {
                *Json_ = nlohmann::json::parse(Content(), Content() + ContentLength());
            }
        }

        return *Json_;
    }
}
