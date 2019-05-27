file(GLOB AC_FSMETAD_DB_SOURCES *.cpp)

add_library(fsmetad_db ${AC_FSMETAD_DB_SOURCES})

target_link_libraries(
    fsmetad_db
    -lwiredtiger
)
