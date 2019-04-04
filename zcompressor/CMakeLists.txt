set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(zcompressor
    zcompressor.h
    zcompressor.cpp
)

target_link_libraries(zcompressor
    Qt5::Core
    z
)

target_include_directories(zcompressor INTERFACE .)
