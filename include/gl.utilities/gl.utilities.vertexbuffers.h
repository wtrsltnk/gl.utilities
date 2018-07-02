#ifndef GL_UTILITIES_VERTEXBUFFERS_H
#define GL_UTILITIES_VERTEXBUFFERS_H

#ifdef _WIN32
#include <GL/glad.h>
#endif // _WIN32

#ifdef __ANDROID__
#include <GLES/gl.h>
#include <GLES3/gl3.h>
#endif // __ANDROID__

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "gl.utilities.shaders.h"

// Vertex
template <class...> class Vertex;

template <class PositionType, class ColorType>
class Vertex<PositionType, ColorType>
{
public:
    PositionType pos;
    ColorType col;
};

template <class PositionType, class NormalType, class TexcoordType>
class Vertex<PositionType, NormalType, TexcoordType>
{
public:
    PositionType pos;
    NormalType normal;
    TexcoordType uv;
};

template <class PositionType, class NormalType, class TexcoordType, class ColorType>
class Vertex<PositionType, NormalType, TexcoordType, ColorType>
{
public:
    PositionType pos;
    NormalType normal;
    TexcoordType uv;
    ColorType color;
};

template <class PositionType, class NormalType, class TexcoordType, class ColorType, class BoneType>
class Vertex<PositionType, NormalType, TexcoordType, ColorType, BoneType>
{
public:
    PositionType pos;
    NormalType normal;
    TexcoordType uv;
    ColorType color;
    BoneType bone;
};


// Vertex buffers
class RenderableBuffer
{
protected:
public:
    unsigned int _vertexArrayId;
    unsigned int _vertexBufferId;
    int _vertexCount;
    GLenum _drawMode;
    std::map<int, int> _faces;

    bool setupRenderableBuffer(int vertexCount)
    {
        this->_vertexCount = vertexCount;

        glGenVertexArrays(1, &this->_vertexArrayId);
        glGenBuffers(1, &this->_vertexBufferId);

        return true;
    }

    RenderableBuffer() : _vertexArrayId(0), _vertexBufferId(0), _vertexCount(0), _drawMode(GL_TRIANGLES) { }
    virtual ~RenderableBuffer() { }

    void setDrawMode(GLenum mode) { this->_drawMode = mode; }
    void addFace(int start, int count) { this->_faces.insert(std::make_pair(start, count)); }
    int vertexCount() const { return this->_vertexCount; }

    void render()
    {
        glBindVertexArray(this->_vertexArrayId);
        if (this->_faces.empty())
        {
            glDrawArrays(this->_drawMode, 0, this->_vertexCount);
        }
        else
        {
            for (auto pair : this->_faces) glDrawArrays(this->_drawMode, pair.first, pair.second);
        }
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void cleanup()
    {
        if (this->_vertexBufferId != 0)
        {
            glDeleteBuffers(1, &this->_vertexBufferId);
            this->_vertexBufferId = 0;
        }
        if (this->_vertexArrayId != 0)
        {
            glDeleteVertexArrays(1, &this->_vertexArrayId);
            this->_vertexArrayId = 0;
        }
    }
};

template <class...> class VertexBuffer;

template <class PositionType, class ColorType>
class VertexBuffer<PositionType, ColorType> : public RenderableBuffer
{
    const Shader<PositionType, ColorType>& _shader;
    std::vector<Vertex<PositionType, ColorType>> _verts;
    ColorType _nextColor;

public:
    VertexBuffer(const Shader<PositionType, ColorType>& shader) : _shader(shader) { }
    virtual ~VertexBuffer() { }

    std::vector<Vertex<PositionType, ColorType>>& verts() { return this->_verts; }

    VertexBuffer<PositionType, ColorType>& operator << (const Vertex<PositionType, ColorType>& vertex)
    {
        this->_verts.push_back(vertex);
        this->_vertexCount = this->_verts.size();

        return *this;
    }

    bool setup()
    {
        if (!this->setupRenderableBuffer(this->_verts.size()))
            return false;

        glBindVertexArray(this->_vertexArrayId);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferId);

        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(this->_verts.size() * sizeof(Vertex<PositionType, ColorType>)), 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(this->_verts.size() * sizeof(Vertex<PositionType, ColorType>)), reinterpret_cast<const GLvoid*>(&this->_verts[0]));

        this->_shader.setupAttributes();

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        this->_verts.clear();

        return true;
    }

public:
    VertexBuffer<PositionType, ColorType>& vertex(const PositionType& position)
    {
        typedef Vertex<PositionType, ColorType> vertex;

        this->_verts.push_back(vertex({
                                          position,
                                          this->_nextColor
                                      }));
        this->_vertexCount = this->_verts.size();
        return *this;
    }

    VertexBuffer<PositionType, ColorType>& color(const ColorType& color)
    {
        this->_nextColor = color;
        return *this;
    }
};

template <class PositionType, class NormalType, class TexcoordType>
class VertexBuffer<PositionType, NormalType, TexcoordType> : public RenderableBuffer
{
public:
    const Shader<PositionType, NormalType, TexcoordType>& _shader;
    std::vector<Vertex<PositionType, NormalType, TexcoordType>> _verts;
    NormalType _nextNormal;
    TexcoordType _nextTexcoord;

    VertexBuffer(const Shader<PositionType, NormalType, TexcoordType>& shader) : _shader(shader) { }
    virtual ~VertexBuffer() { }

    std::vector<Vertex<PositionType, NormalType, TexcoordType>>& verts() { return this->_verts; }

    VertexBuffer<PositionType, NormalType, TexcoordType>& operator << (const Vertex<PositionType, NormalType, TexcoordType>& vertex)
    {
        this->_verts.push_back(vertex);
        this->_vertexCount = this->_verts.size();

        return *this;
    }

    bool setup()
    {
        if (!this->setupRenderableBuffer(this->_verts.size()))
            return false;

        glBindVertexArray(this->_vertexArrayId);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferId);

        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(this->_verts.size() * sizeof(Vertex<PositionType, NormalType, TexcoordType>)), 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(this->_verts.size() * sizeof(Vertex<PositionType, NormalType, TexcoordType>)), reinterpret_cast<const GLvoid*>(&this->_verts[0]));

        this->_shader.setupAttributes();

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        this->_verts.clear();

        return true;
    }

public:
    VertexBuffer<PositionType, NormalType, TexcoordType>& vertex(const PositionType& position)
    {
        typedef Vertex<PositionType, NormalType, TexcoordType> vertex;

        this->_verts.push_back(vertex({
                                          position,
                                          this->_nextNormal,
                                          this->_nextTexcoord
                                      }));
        this->_vertexCount = this->_verts.size();
        return *this;
    }

    VertexBuffer<PositionType, NormalType, TexcoordType>& normal(const NormalType& normal)
    {
        this->_nextNormal = normal;
        return *this;
    }

    VertexBuffer<PositionType, NormalType, TexcoordType>& texcoord(const TexcoordType& texcoord)
    {
        this->_nextTexcoord = texcoord;
        return *this;
    }
};

template <class PositionType, class NormalType, class TexcoordType, class ColorType>
class VertexBuffer<PositionType, NormalType, TexcoordType, ColorType> : public RenderableBuffer
{
public:
    const Shader<PositionType, NormalType, TexcoordType, ColorType>& _shader;
    std::vector<Vertex<PositionType, NormalType, TexcoordType, ColorType>> _verts;
    NormalType _nextNormal;
    TexcoordType _nextTexcoord;
    ColorType _nextColor;

    VertexBuffer(const Shader<PositionType, NormalType, TexcoordType, ColorType>& shader) : _shader(shader) { }
    virtual ~VertexBuffer() { }

    std::vector<Vertex<PositionType, NormalType, TexcoordType, ColorType>>& verts() { return this->_verts; }

    VertexBuffer<PositionType, NormalType, TexcoordType, ColorType>& operator << (const Vertex<PositionType, NormalType, TexcoordType, ColorType>& vertex)
    {
        this->_verts.push_back(vertex);
        this->_vertexCount = this->_verts.size();

        return *this;
    }

    bool setup()
    {
        if (!this->setupRenderableBuffer(this->_verts.size()))
            return false;

        glBindVertexArray(this->_vertexArrayId);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferId);

        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(this->_verts.size() * sizeof(Vertex<PositionType, NormalType, TexcoordType, ColorType>)), 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(this->_verts.size() * sizeof(Vertex<PositionType, NormalType, TexcoordType, ColorType>)), reinterpret_cast<const GLvoid*>(&this->_verts[0]));

        this->_shader.setupAttributes();

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        this->_verts.clear();

        return true;
    }

public:
    VertexBuffer<PositionType, NormalType, TexcoordType, ColorType>& vertex(const PositionType& position)
    {
        typedef Vertex<PositionType, NormalType, TexcoordType, ColorType> vertex;

        this->_verts.push_back(vertex({
                                          position,
                                          this->_nextNormal,
                                          this->_nextTexcoord,
                                          this->_nextColor
                                      }));
        this->_vertexCount = this->_verts.size();
        return *this;
    }

    VertexBuffer<PositionType, NormalType, TexcoordType, ColorType>& normal(const NormalType& normal)
    {
        this->_nextNormal = normal;
        return *this;
    }

    VertexBuffer<PositionType, NormalType, TexcoordType, ColorType>& texcoord(const TexcoordType& texcoord)
    {
        this->_nextTexcoord = texcoord;
        return *this;
    }

    VertexBuffer<PositionType, NormalType, TexcoordType, ColorType>& color(const ColorType& color)
    {
        this->_nextColor = color;
        return *this;
    }
};

template <class PositionType, class NormalType, class TexcoordType, class ColorType, class BoneType>
class VertexBuffer<PositionType, NormalType, TexcoordType, ColorType, BoneType> : public RenderableBuffer
{
    const Shader<PositionType, NormalType, TexcoordType, ColorType, BoneType>& _shader;
    std::vector<Vertex<PositionType, NormalType, TexcoordType, ColorType, BoneType>> _verts;
    NormalType _nextNormal;
    TexcoordType _nextTexcoord;
    ColorType _nextColor;
    BoneType _nextBone;

public:
    VertexBuffer(const Shader<PositionType, NormalType, TexcoordType, ColorType, BoneType>& shader) : _shader(shader) { }
    virtual ~VertexBuffer() { }

    std::vector<Vertex<PositionType, NormalType, TexcoordType, ColorType, BoneType>>& verts() { return this->_verts; }

    VertexBuffer<PositionType, NormalType, TexcoordType, ColorType, BoneType>& operator << (const Vertex<PositionType, NormalType, TexcoordType, ColorType, BoneType>& vertex)
    {
        this->_verts.push_back(vertex);
        this->_vertexCount = this->_verts.size();

        return *this;
    }

    bool setup()
    {
        auto vertexSize = sizeof(PositionType) + sizeof(NormalType) + sizeof(TexcoordType) + sizeof(ColorType) + sizeof(BoneType);

        if (!this->setupRenderableBuffer(this->_verts.size()))
            return false;

        glBindVertexArray(this->_vertexArrayId);
        glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferId);

        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(this->_verts.size() * vertexSize), 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(this->_verts.size() * vertexSize), reinterpret_cast<const GLvoid*>(&this->_verts[0]));

        this->_shader.setupAttributes();

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        this->_verts.clear();

        return true;
    }

public:
    VertexBuffer<PositionType, NormalType, TexcoordType, ColorType, BoneType>& vertex(const PositionType& position)
    {
        typedef Vertex<PositionType, NormalType, TexcoordType, ColorType, BoneType> vertex;

        this->_verts.push_back(vertex({
                                          position,
                                          this->_nextNormal,
                                          this->_nextTexcoord,
                                          this->_nextColor,
                                          this->_nextBone
                                      }));
        this->_vertexCount = this->_verts.size();
        return *this;
    }

    VertexBuffer<PositionType, NormalType, TexcoordType, ColorType, BoneType>& normal(const NormalType& normal)
    {
        this->_nextNormal = normal;
        return *this;
    }

    VertexBuffer<PositionType, NormalType, TexcoordType, ColorType, BoneType>& texcoord(const TexcoordType& texcoord)
    {
        this->_nextTexcoord = texcoord;
        return *this;
    }

    VertexBuffer<PositionType, NormalType, TexcoordType, ColorType, BoneType>& color(const ColorType& color)
    {
        this->_nextColor = color;
        return *this;
    }

    VertexBuffer<PositionType, NormalType, TexcoordType, ColorType>& bone(const BoneType& bone)
    {
        this->_nextBone = bone;
        return *this;
    }
};

#endif // GL_UTILITIES_VERTEXBUFFERS_H
