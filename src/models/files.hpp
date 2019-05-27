#pragma once

#include <db/model.hpp>

namespace NAC {
    AC_MODEL_BEGIN(TFilesKey)
    AC_MODEL_FIELD(TFSMetaDBStringField, Id)
    AC_MODEL_END()

    AC_MODEL_BEGIN(TFilesData)
    AC_MODEL_FIELD(TFSMetaDBStringField, Name)
    AC_MODEL_FIELD(TFSMetaDBStringField, Path)
    AC_MODEL_FIELD(TFSMetaDBStringField, CType)
    AC_MODEL_END()

    class TFilesModel : public TFSMetaDBModelDescr<TFilesKey, TFilesData> {
    public:
        static std::string DBName;
    };
}
