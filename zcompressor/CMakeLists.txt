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

if(WIN32)
    # For windows (custom zlib)
    if(DEFINED ZLIB_DIR)
        find_library(ZLIB_LIB zlib ${ZLIB_DIR})
        if(ZLIB_LIB STREQUAL ZLIB_LIB-NOTFOUND)
            message(FATAL_ERROR "zlib not found in ${ZLIB_DIR}")
        else()
            message(STATUS "zlib found in: ${ZLIB_LIB}")
            target_link_libraries(zcompressor
                Qt5::Core
                ${ZLIB_LIB}
            )
            target_link_libraries(zcompressor_static
                Qt5::Core
                ${ZLIB_LIB}
            )
        endif()
    endif()
else()
    target_link_libraries(zcompressor
        Qt5::Core
        z
    )
    target_link_libraries(zcompressor_static
        Qt5::Core
        z
    )
endif()

configure_file(zcompressor.h "${BINARY_DIR}/lib/zcompressor.h"  COPYONLY)
configure_file(zipwriter.h "${BINARY_DIR}/lib/zipwriter.h"  COPYONLY)

target_include_directories(zcompressor_static INTERFACE .)
