set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(zcompressor SHARED
    zcompressor.h
    zcompressor.cpp
)

add_library(zcompressor_static STATIC
    zcompressor.h
    zcompressor.cpp
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

target_include_directories(zcompressor_static INTERFACE .)
