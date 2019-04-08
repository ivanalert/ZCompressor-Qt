set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(zcompressor SHARED
    zcompressor.h
    zcompressor.cpp
    zipheader.h
    zipheader.cpp
    zipwriter.h
    zipheader.cpp
)

add_library(zcompressor_static STATIC
    zcompressor.h
    zcompressor.cpp
    zipheader.h
    zipheader.cpp
    zipwriter.h
    zipwriter.cpp
)

target_link_libraries(zcompressor
    Qt5::Core
    z
)

target_link_libraries(zcompressor_static
    Qt5::Core
    z
)

configure_file(zcompressor.h "${BINARY_DIR}/lib/zcompressor.h"  COPYONLY)
configure_file(zipwriter.h "${BINARY_DIR}/lib/zipwriter.h"  COPYONLY)

target_include_directories(zcompressor_static INTERFACE .)
