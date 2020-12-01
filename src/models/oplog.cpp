#include "oplog.hpp"

namespace NAC {
    AC_WT_MODEL_IMPL_START(TOplogData)
    AC_WT_MODEL_FIELD_IMPL(TOplogData, Timestamp)
    AC_WT_MODEL_FIELD_IMPL(TOplogData, Data)
    AC_WT_MODEL_IMPL_END(TOplogData)

    std::string TOplogModel::DBName = "oplog";

    bool TOplogModel::Save(TWiredTigerSession& conn, std::string&& data_) {
        TOplogData data;
        data.SetData(data_);
        data.SetTimestamp(time(nullptr));

        return conn.Append<TOplogModel>(data);
    }
}
