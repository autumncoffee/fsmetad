#pragma once

#include <ac-library/http/request.hpp>
#include <json.hh>
#include <utility>
#include <memory>
#include <ac-library/wiredtiger/connection.hpp>

namespace NAC {
    class TFSMetaDRequest : public NHTTP::TRequest {
    public:
        template<typename... TArgs>
        TFSMetaDRequest(TWiredTiger& db, TArgs&&... args)
            : NHTTP::TRequest(std::forward<TArgs>(args)...)
            , Db_(db)
        {
        }

        const nlohmann::json& Json();

        NHTTP::TResponse RespondJson(const nlohmann::json& data) const {
            auto response = Respond200();

            response.Header("Content-Type", "application/json");
            response.Write(data.dump());

            return response;
        }

        template<typename... TArgs>
        void SendJson(TArgs&&... args) {
            Send(RespondJson(std::forward<TArgs>(args)...));
        }

        TWiredTigerSession Db() const {
            return Db_.Open();
        }

    private:
        std::unique_ptr<nlohmann::json> Json_;
        TWiredTiger& Db_;
    };
}
