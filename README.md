# gl.utilities
OpenGL utilities that help you create prototypes faster. It contains a template class for shaders, vertexbuffers and textures for now.

Just copy the header files from this directory into your project and start using them. The only dependency is within "gl.utilities.vertexbuffers.h" which needs "gl.utilities.shaders.h". Further more its only std.

In the examples I use glm for the types, but you should be able to use your own types.

## Vertex types

There are templated classes for thee vertex attributes configurations. The first is for vertex position and color. See "examples/01-VertexAndColorExample" on how to use these. The second configuration has position, normal and texcoords. See "examples/02-VertexNormalAndTexcoordExample" on how to use these. The third configuration has position, normal, texcoords and color. See "examples/03-VertexNormalTexcoordAndColorExample" on how to use these. The configurations with texcoords also have a uniform for the texture itself.