
add_library(gl.utilities
    gl.utilities.cpp
    )

target_compile_features(gl.utilities
    PRIVATE cxx_auto_type
    PRIVATE cxx_nullptr
    PRIVATE cxx_range_for
    PRIVATE cxx_thread_local
    )

target_include_directories(gl.utilities
    PUBLIC
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ../include
    )

install(
    TARGETS
        gl.utilities
    EXPORT
        gl.utilities
    DESTINATION
        "lib"
    )

install(
    FILES
        ${system.net_SOURCE_DIR}/include/gl.utilities/gl.utilities.loaders.h
        ${system.net_SOURCE_DIR}/include/gl.utilities/gl.utilities.shaders.h
        ${system.net_SOURCE_DIR}/include/gl.utilities/gl.utilities.textures.h
        ${system.net_SOURCE_DIR}/include/gl.utilities/gl.utilities.vertexbuffers.h
    DESTINATION
        "include/gl.utilities"
    )

install(
    EXPORT
        gl.utilities
    FILE
        gl.utilities-config.cmake
    DESTINATION
        "cmake"
    )
