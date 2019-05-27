#include "files.hpp"

namespace NAC {
    AC_MODEL_IMPL(TFilesKey)
    AC_MODEL_FIELD_IMPL(TFilesKey, Id)

    AC_MODEL_IMPL(TFilesData)
    AC_MODEL_FIELD_IMPL(TFilesData, Name)
    AC_MODEL_FIELD_IMPL(TFilesData, Path)
    AC_MODEL_FIELD_IMPL(TFilesData, CType)

    std::string TFilesModel::DBName = "files";
}
