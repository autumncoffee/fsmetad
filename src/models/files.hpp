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
    AC_MODEL_FIELD(TFSMetaDBUIntField, Size)
    AC_MODEL_FIELD(TFSMetaDBUIntField, Offset)
    AC_MODEL_END()

    class TFilesModel : public TFSMetaDBModelDescr<TFilesKey, TFilesData> {
    public:
        static std::string DBName;
    };

    AC_MODEL_BEGIN(TFileTagsData)
    AC_MODEL_FIELD(TFSMetaDBStringField, Tag)
    AC_MODEL_FIELD(TFSMetaDBStringField, File)
    AC_MODEL_END()

    class TFileTagsModel : public TFSMetaDBModelDescr<TFilesKey, TFileTagsData> {
    public:
        static std::string DBName;
    };

    using TFileTagsIndexKey = TFileTagsData;

    class TFileTagsIndex : public TFSMetaDBIndexDescr<TFileTagsModel, TFileTagsIndexKey> {
    public:
        static std::string DBName;
    };

    AC_MODEL_BEGIN(TFileTagsSecondIndexKey)
    AC_MODEL_FIELD(TFSMetaDBStringField, File)
    AC_MODEL_END()

    class TFileTagsSecondIndex : public TFSMetaDBIndexDescr<TFileTagsModel, TFileTagsSecondIndexKey> {
    public:
        static std::string DBName;
    };
}
