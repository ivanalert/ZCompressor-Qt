add_executable(compressor main.cpp)

target_link_libraries(compressor
    zcompressor_static
)
