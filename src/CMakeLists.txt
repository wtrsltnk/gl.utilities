
add_library(gl.utilities INTERFACE)

target_include_directories(gl.utilities
    INTERFACE
        $<INSTALL_INTERFACE:include>
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
        ${PROJECT_SOURCE_DIR}/include/gl.utilities/gl.utilities.loaders.h
        ${PROJECT_SOURCE_DIR}/include/gl.utilities/gl.utilities.shaders.h
        ${PROJECT_SOURCE_DIR}/include/gl.utilities/gl.utilities.textures.h
        ${PROJECT_SOURCE_DIR}/include/gl.utilities/gl.utilities.vertexbuffers.h
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
