file(GLOB AC_FSMETAD_HANDLERS_V1_SOURCES *.cpp)

add_subdirectory("file")

add_library(fsmetad_handlers_v1 ${AC_FSMETAD_HANDLERS_V1_SOURCES})

target_link_libraries(
    fsmetad_handlers_v1
    fsmetad_handlers_v1_file
)
