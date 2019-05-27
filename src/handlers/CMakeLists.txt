file(GLOB AC_FSMETAD_HANDLERS_SOURCES *.cpp)

add_subdirectory("v1")

add_library(fsmetad_handlers ${AC_FSMETAD_HANDLERS_SOURCES})

target_link_libraries(
    fsmetad_handlers
    fsmetad_handlers_v1
)
