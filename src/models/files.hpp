#pragma once

#include <ac-library/wiredtiger/model.hpp>

namespace NAC {
    AC_WT_MODEL_BEGIN(TFilesKey)
    AC_WT_MODEL_FIELD(TWiredTigerStringField, Id)
    AC_WT_MODEL_END()

    AC_WT_MODEL_BEGIN(TFilesData)
    AC_WT_MODEL_FIELD(TWiredTigerStringField, Name)
    AC_WT_MODEL_FIELD(TWiredTigerStringField, Path)
    AC_WT_MODEL_FIELD(TWiredTigerStringField, CType)
    AC_WT_MODEL_FIELD(TWiredTigerUIntField, Size)
    AC_WT_MODEL_FIELD(TWiredTigerUIntField, Offset)

    public:
        std::string GetTempPath() const;

    AC_WT_MODEL_END()

    class TFilesModel : public TWiredTigerModelDescr<TFilesKey, TFilesData> {
    public:
        static std::string DBName;
    };

    AC_WT_MODEL_BEGIN(TFileTagsData)
    AC_WT_MODEL_FIELD(TWiredTigerStringField, Tag)
    AC_WT_MODEL_FIELD(TWiredTigerStringField, File)
    AC_WT_MODEL_END()

    class TFileTagsModel : public TWiredTigerModelDescr<TFilesKey, TFileTagsData> {
    public:
        static std::string DBName;
    };

    using TFileTagsIndexKey = TFileTagsData;

    class TFileTagsIndex : public TWiredTigerIndexDescr<TFileTagsModel, TFileTagsIndexKey> {
    public:
        static std::string DBName;
    };

    AC_WT_MODEL_BEGIN(TFileTagsSecondIndexKey)
    AC_WT_MODEL_FIELD(TWiredTigerStringField, File)
    AC_WT_MODEL_END()

    class TFileTagsSecondIndex : public TWiredTigerIndexDescr<TFileTagsModel, TFileTagsSecondIndexKey> {
    public:
        static std::string DBName;
    };

    AC_WT_MODEL_BEGIN(TFilesSyncInfo)
    AC_WT_MODEL_FIELD(TWiredTigerUIntField, Size)
    AC_WT_MODEL_FIELD(TWiredTigerUIntField, Offset)
    AC_WT_MODEL_END()

    class TFilesSyncInfoModel : public TWiredTigerModelDescr<TFilesKey, TFilesSyncInfo> {
    public:
        static std::string DBName;
    };

    AC_WT_MODEL_BEGIN(TFileNameIndexKey)
    AC_WT_MODEL_FIELD(TWiredTigerStringField, Name)
    AC_WT_MODEL_FIELD(TWiredTigerStringField, Id)
    AC_WT_MODEL_END()

    class TFileNameIndex : public TWiredTigerIndexDescr<TFilesModel, TFileNameIndexKey> {
    public:
        static std::string DBName;
    };
}
