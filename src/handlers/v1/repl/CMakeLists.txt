file(GLOB AC_FSMETAD_HANDLERS_V1_REPL_SOURCES *.cpp)

add_library(fsmetad_handlers_v1_repl ${AC_FSMETAD_HANDLERS_V1_REPL_SOURCES})

target_link_libraries(
    fsmetad_handlers_v1_repl
    fsmetad_models
)
