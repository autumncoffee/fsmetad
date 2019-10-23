#include "files.hpp"

namespace NAC {
    AC_MODEL_IMPL_START(TFilesKey)
    AC_MODEL_FIELD_IMPL(TFilesKey, Id)
    AC_MODEL_IMPL_END(TFilesKey)

    AC_MODEL_IMPL_START(TFilesData)
    AC_MODEL_FIELD_IMPL(TFilesData, Name)
    AC_MODEL_FIELD_IMPL(TFilesData, Path)
    AC_MODEL_FIELD_IMPL(TFilesData, CType)
    AC_MODEL_FIELD_IMPL(TFilesData, Size)
    AC_MODEL_FIELD_IMPL(TFilesData, Offset)
    AC_MODEL_IMPL_END(TFilesData)

    std::string TFilesModel::DBName = "files";

    AC_MODEL_IMPL_START(TFileTagsData)
    AC_MODEL_FIELD_IMPL(TFileTagsData, Tag)
    AC_MODEL_FIELD_IMPL(TFileTagsData, File)
    AC_MODEL_IMPL_END(TFileTagsData)

    std::string TFileTagsModel::DBName = "file_tags";

    std::string TFileTagsIndex::DBName = "ft";

    AC_MODEL_IMPL_START(TFileTagsSecondIndexKey)
    AC_MODEL_FIELD_IMPL(TFileTagsSecondIndexKey, File)
    AC_MODEL_IMPL_END(TFileTagsSecondIndexKey)

    std::string TFileTagsSecondIndex::DBName = "ft2";

    AC_MODEL_IMPL_START(TFilesSyncInfo)
    AC_MODEL_FIELD_IMPL(TFilesSyncInfo, Size)
    AC_MODEL_FIELD_IMPL(TFilesSyncInfo, Offset)
    AC_MODEL_IMPL_END(TFilesSyncInfo)

    std::string TFilesSyncInfoModel::DBName = "filesyncinfo";
}
