#include "files.hpp"
#include <fs_wrapper.hpp>

namespace NAC {
    AC_WT_MODEL_IMPL_START(TFilesKey)
    AC_WT_MODEL_FIELD_IMPL(TFilesKey, Id)
    AC_WT_MODEL_IMPL_END(TFilesKey)

    AC_WT_MODEL_IMPL_START(TFilesData)
    AC_WT_MODEL_FIELD_IMPL(TFilesData, Name)
    AC_WT_MODEL_FIELD_IMPL(TFilesData, Path)
    AC_WT_MODEL_FIELD_IMPL(TFilesData, CType)
    AC_WT_MODEL_FIELD_IMPL(TFilesData, Size)
    AC_WT_MODEL_FIELD_IMPL(TFilesData, Offset)

    std::string TFilesData::GetTempPath() const {
        const stdfs::path path(GetPath());
        return (path.parent_path() / ("." + path.filename().string())).string();
    }

    AC_WT_MODEL_IMPL_END(TFilesData)

    std::string TFilesModel::DBName = "files";

    AC_WT_MODEL_IMPL_START(TFileTagsData)
    AC_WT_MODEL_FIELD_IMPL(TFileTagsData, Tag)
    AC_WT_MODEL_FIELD_IMPL(TFileTagsData, File)
    AC_WT_MODEL_IMPL_END(TFileTagsData)

    std::string TFileTagsModel::DBName = "file_tags";

    std::string TFileTagsIndex::DBName = "ft";

    AC_WT_MODEL_IMPL_START(TFileTagsSecondIndexKey)
    AC_WT_MODEL_FIELD_IMPL(TFileTagsSecondIndexKey, File)
    AC_WT_MODEL_IMPL_END(TFileTagsSecondIndexKey)

    std::string TFileTagsSecondIndex::DBName = "ft2";

    AC_WT_MODEL_IMPL_START(TFilesSyncInfo)
    AC_WT_MODEL_FIELD_IMPL(TFilesSyncInfo, Size)
    AC_WT_MODEL_FIELD_IMPL(TFilesSyncInfo, Offset)
    AC_WT_MODEL_IMPL_END(TFilesSyncInfo)

    std::string TFilesSyncInfoModel::DBName = "filesyncinfo";

    AC_WT_MODEL_IMPL_START(TFileNameIndexKey)
    AC_WT_MODEL_FIELD_IMPL(TFileNameIndexKey, Name)
    AC_WT_MODEL_FIELD_IMPL(TFileNameIndexKey, Id)
    AC_WT_MODEL_IMPL_END(TFileNameIndexKey)

    std::string TFileNameIndex::DBName = "filenames";
}
