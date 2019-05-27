file(GLOB AC_FSMETAD_MODELS_SOURCES *.cpp)

add_library(fsmetad_models ${AC_FSMETAD_MODELS_SOURCES})

target_link_libraries(
    fsmetad_models
    fsmetad_db
)
