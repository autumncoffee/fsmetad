file(GLOB AC_FSMETAD_HANDLERS_V1_FILE_SOURCES *.cpp)

add_library(fsmetad_handlers_v1_file ${AC_FSMETAD_HANDLERS_V1_FILE_SOURCES})

target_link_libraries(
    fsmetad_handlers_v1_file
    fsmetad_models
    ${FSMETAD_EXTRA_LIBS}
)
