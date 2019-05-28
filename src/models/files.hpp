#pragma once

#include <ac-library/wiredtiger/model.hpp>

namespace NAC {
    AC_MODEL_BEGIN(TFilesKey)
    AC_MODEL_FIELD(TWiredTigerStringField, Id)
    AC_MODEL_END()

    AC_MODEL_BEGIN(TFilesData)
    AC_MODEL_FIELD(TWiredTigerStringField, Name)
    AC_MODEL_FIELD(TWiredTigerStringField, Path)
    AC_MODEL_FIELD(TWiredTigerStringField, CType)
    AC_MODEL_FIELD(TWiredTigerUIntField, Size)
    AC_MODEL_FIELD(TWiredTigerUIntField, Offset)
    AC_MODEL_END()

    class TFilesModel : public TWiredTigerModelDescr<TFilesKey, TFilesData> {
    public:
        static std::string DBName;
    };

    AC_MODEL_BEGIN(TFileTagsData)
    AC_MODEL_FIELD(TWiredTigerStringField, Tag)
    AC_MODEL_FIELD(TWiredTigerStringField, File)
    AC_MODEL_END()

    class TFileTagsModel : public TWiredTigerModelDescr<TFilesKey, TFileTagsData> {
    public:
        static std::string DBName;
    };

    using TFileTagsIndexKey = TFileTagsData;

    class TFileTagsIndex : public TWiredTigerIndexDescr<TFileTagsModel, TFileTagsIndexKey> {
    public:
        static std::string DBName;
    };

    AC_MODEL_BEGIN(TFileTagsSecondIndexKey)
    AC_MODEL_FIELD(TWiredTigerStringField, File)
    AC_MODEL_END()

    class TFileTagsSecondIndex : public TWiredTigerIndexDescr<TFileTagsModel, TFileTagsSecondIndexKey> {
    public:
        static std::string DBName;
    };
}
