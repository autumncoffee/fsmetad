#pragma once

#include <ac-library/wiredtiger/model.hpp>
#include <ac-library/wiredtiger/autoincrement.hpp>
#include <ac-library/wiredtiger/session.hpp>

namespace NAC {
    AC_WT_MODEL_BEGIN(TOplogData)
    AC_WT_MODEL_FIELD(TWiredTigerUIntField, Timestamp)
    AC_WT_MODEL_FIELD(TWiredTigerStringField, Data)
    AC_WT_MODEL_END()

    class TOplogModel : public TWiredTigerModelDescr<TWiredTigerAutoincrementKey, TOplogData> {
    public:
        static std::string DBName;

        static bool Save(TWiredTigerSession& conn, std::string&& data);
    };
}
