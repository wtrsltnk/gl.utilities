#ifndef GL_UTILITIES_SHADERS_H
#define GL_UTILITIES_SHADERS_H

#ifdef _WIN32
#include <glad/glad.h>
#endif // _WIN32

#ifdef __ANDROID__
#include <GLES/gl.h>
#include <GLES3/gl3.h>
#endif // __ANDROID__

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <streambuf>

// Shaders
class CompiledShader
{
protected:
public:
    GLuint _shaderId;
    CompiledShader() : _shaderId(0) { }
    virtual ~CompiledShader() { }

    GLuint id() const { return this->_shaderId; }

    virtual bool compileFromFile(const std::string& vertShaderFile, const std::string& fragShaderFile)
    {
        std::ifstream vertShaderFileStream(vertShaderFile.c_str());
        std::string vertShaderStr((std::istreambuf_iterator<char>(vertShaderFileStream)),
                         std::istreambuf_iterator<char>());

        std::ifstream fragShaderFileStream(fragShaderFile.c_str());
        std::string fragShaderStr((std::istreambuf_iterator<char>(fragShaderFileStream)),
                         std::istreambuf_iterator<char>());

        return compile(vertShaderStr, fragShaderStr);
    }

    virtual bool compile(const std::string& vertShaderStr, const std::string& fragShaderStr)
    {
        GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char *vertShaderSrc = vertShaderStr.c_str();
        const char *fragShaderSrc = fragShaderStr.c_str();

        GLint result = GL_FALSE;
        GLint logLength;

        // Compile vertex shader
        glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
        glCompileShader(vertShader);

        // Check vertex shader
        glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<GLchar> vertShaderError(static_cast<size_t>((logLength > 1) ? logLength : 1));
            glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
            std::cout << &vertShaderError[0] << std::endl;

            return false;
        }

        // Compile fragment shader
        glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
        glCompileShader(fragShader);

        // Check fragment shader
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<GLchar> fragShaderError(static_cast<size_t>((logLength > 1) ? logLength : 1));
            glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
            std::cout << &fragShaderError[0] << std::endl;

            return false;
        }

        this->_shaderId = glCreateProgram();
        glAttachShader(this->_shaderId, vertShader);
        glAttachShader(this->_shaderId, fragShader);
        glLinkProgram(this->_shaderId);

        glGetProgramiv(this->_shaderId, GL_LINK_STATUS, &result);
        if (result == GL_FALSE)
        {
            glGetProgramiv(this->_shaderId, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<GLchar> programError(static_cast<size_t>((logLength > 1) ? logLength : 1));
            glGetProgramInfoLog(this->_shaderId, logLength, NULL, &programError[0]);
            std::cout << &programError[0] << std::endl;

            return false;
        }

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);

        return true;
    }

    void use() const
    {
        glUseProgram(this->_shaderId);
    }
};

// Shaders with Projection, View and Model uniforms
class PVMShader : public CompiledShader
{
public:
    GLuint _projectionUniformId;
    GLuint _viewUniformId;
    GLuint _modelUniformId;

    PVMShader()
        : CompiledShader(), _projectionUniformId(0), _viewUniformId(0), _modelUniformId(0),
          _projectionUniformName("u_projection"), _viewUniformName("u_view"), _modelUniformName("u_model")
    { }
    virtual ~PVMShader() { }

    std::string _projectionUniformName;
    std::string _viewUniformName;
    std::string _modelUniformName;

    virtual bool compile(const std::string& vertShaderStr, const std::string& fragShaderStr)
    {
        if (!CompiledShader::compile(vertShaderStr, fragShaderStr))
            return false;

        this->_projectionUniformId = glGetUniformLocation(this->_shaderId, this->_projectionUniformName.c_str());
        this->_viewUniformId = glGetUniformLocation(this->_shaderId, this->_viewUniformName.c_str());
        this->_modelUniformId = glGetUniformLocation(this->_shaderId, this->_modelUniformName.c_str());

        return true;
    }

    void setupMatrices(const float projection[], const float view[], const float model[])
    {
        this->use();

        glUniformMatrix4fv(this->_projectionUniformId, 1, false, projection);
        glUniformMatrix4fv(this->_viewUniformId, 1, false, view);
        glUniformMatrix4fv(this->_modelUniformId, 1, false, model);
    }

    void setupMatrices(const float projectionView[], const float model[])
    {
        this->use();

        glUniformMatrix4fv(this->_projectionUniformId, 1, false, projectionView);
        glUniformMatrix4fv(this->_modelUniformId, 1, false, model);
    }
};

class TextureShader : public PVMShader
{
public:
    GLuint _textureUniformId;
    TextureShader()
        : _textureUniformId(0), _textureUniformName("texture")
    { }

    virtual ~TextureShader() { }

    std::string _textureUniformName;

    virtual bool compile(const std::string& vertShaderStr, const std::string& fragShaderStr)
    {
        if (!PVMShader::compile(vertShaderStr, fragShaderStr))
            return false;

        this->_textureUniformId = glGetUniformLocation(this->_shaderId, this->_textureUniformName.c_str());
        glUniform1i(this->_textureUniformId, 0);

        return true;
    }
};

template <class...> class Shader;

template <class PositionType, class ColorType>
class Shader<PositionType, ColorType> : public PVMShader
{
public:
    Shader()
        : _vertexAttributeName("vertex"), _colorAttributeName("color")
    { }

    virtual ~Shader() { }

    std::string _vertexAttributeName;
    std::string _colorAttributeName;

    void setupAttributes() const
    {
        auto vertexSize = sizeof(PositionType) + sizeof(ColorType);

        auto vertexAttrib = glGetAttribLocation(this->_shaderId, this->_vertexAttributeName.c_str());
        glVertexAttribPointer(GLuint(vertexAttrib), sizeof(PositionType) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, 0);
        glEnableVertexAttribArray(GLuint(vertexAttrib));

        auto colorAttrib = glGetAttribLocation(this->_shaderId, this->_colorAttributeName.c_str());
        glVertexAttribPointer(GLuint(colorAttrib), sizeof(ColorType) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid*>(sizeof(PositionType)));
        glEnableVertexAttribArray(GLuint(colorAttrib));
    }
};

template <class PositionType, class NormalType, class TexcoordType>
class Shader<PositionType, NormalType, TexcoordType> : public TextureShader
{
public:
    GLuint _textureUniformId;
    Shader()
        : _vertexAttributeName("vertex"), _normalAttributeName("normal"), _texcoordAttributeName("texcoord")
    { }

    virtual ~Shader() { }

    std::string _vertexAttributeName;
    std::string _normalAttributeName;
    std::string _texcoordAttributeName;

    void setupAttributes() const
    {
        auto vertexSize = sizeof(PositionType) + sizeof(NormalType) + sizeof(TexcoordType);

        GLuint vertexAttrib = glGetAttribLocation(this->_shaderId, this->_vertexAttributeName.c_str());
        glVertexAttribPointer(vertexAttrib, sizeof(PositionType) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, 0);
        glEnableVertexAttribArray(vertexAttrib);

        GLuint normalAttrib = glGetAttribLocation(this->_shaderId, this->_normalAttributeName.c_str());
        glVertexAttribPointer(normalAttrib, sizeof(NormalType) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid*>(sizeof(PositionType)));
        glEnableVertexAttribArray(normalAttrib);

        GLuint texcoordAttrib = glGetAttribLocation(this->_shaderId, this->_texcoordAttributeName.c_str());
        glVertexAttribPointer(texcoordAttrib, sizeof(TexcoordType) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid*>(sizeof(PositionType) + sizeof(NormalType)));
        glEnableVertexAttribArray(texcoordAttrib);
    }
};

template <class PositionType, class NormalType, class TexcoordType, class ColorType>
class Shader<PositionType, NormalType, TexcoordType, ColorType> : public TextureShader
{
public:
    Shader()
        : _vertexAttributeName("vertex"), _normalAttributeName("normal"),
          _texcoordAttributeName("texcoord"), _colorAttributeName("color")
    { }

    virtual ~Shader() { }

    std::string _vertexAttributeName;
    std::string _normalAttributeName;
    std::string _texcoordAttributeName;
    std::string _colorAttributeName;

    void setupAttributes() const
    {
        auto vertexSize = sizeof(PositionType) + sizeof(NormalType) + sizeof(TexcoordType) + sizeof(ColorType);

        GLuint vertexAttrib = glGetAttribLocation(this->_shaderId, this->_vertexAttributeName.c_str());
        glVertexAttribPointer(vertexAttrib, sizeof(PositionType) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, 0);
        glEnableVertexAttribArray(vertexAttrib);

        GLuint normalAttrib = glGetAttribLocation(this->_shaderId, this->_normalAttributeName.c_str());
        glVertexAttribPointer(normalAttrib, sizeof(NormalType) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid*>(sizeof(PositionType)));
        glEnableVertexAttribArray(normalAttrib);

        GLuint texcoordAttrib = glGetAttribLocation(this->_shaderId, this->_texcoordAttributeName.c_str());
        glVertexAttribPointer(texcoordAttrib, sizeof(TexcoordType) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid*>(sizeof(PositionType) + sizeof(NormalType)));
        glEnableVertexAttribArray(texcoordAttrib);

        GLuint colorAttrib = glGetAttribLocation(this->_shaderId, this->_colorAttributeName.c_str());
        glVertexAttribPointer(colorAttrib, sizeof(ColorType) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid*>(sizeof(PositionType) + sizeof(NormalType) + sizeof(TexcoordType)));
        glEnableVertexAttribArray(colorAttrib);
    }
};

class SkinnedShader : public TextureShader
{
    GLuint _bonesUniformId;
    GLuint _bonesBufferId;
public:
    SkinnedShader()
        : _bonesUniformId(0), _bonesBufferId(0),
          _bonesBlockUniformName("u_bones")
    { }
    virtual ~SkinnedShader() { }

    std::string _bonesBlockUniformName;

    virtual bool compile(const std::string& vertShaderStr, const std::string& fragShaderStr, int maxBoneCount)
    {
        if (!PVMShader::compile(vertShaderStr, fragShaderStr))
            return false;

        this->_bonesUniformId = 0;
        GLint uniform_block_index = glGetUniformBlockIndex(this->_shaderId, this->_bonesBlockUniformName.c_str());
        glUniformBlockBinding(this->_shaderId, uniform_block_index, this->_bonesUniformId);

        glGenBuffers(1, &this->_bonesBufferId);

        glBindBuffer(GL_UNIFORM_BUFFER, this->_bonesBufferId);
        glBufferData(GL_UNIFORM_BUFFER, maxBoneCount * sizeof(float) * 16, 0, GL_STREAM_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        return true;
    }

    void setupBones(const float boneMatrices[][16], int boneCount)
    {
        this->use();

        glBindBuffer(GL_UNIFORM_BUFFER, this->_bonesBufferId);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, boneCount * sizeof(float) * 16, boneMatrices);
        glBindBufferRange(GL_UNIFORM_BUFFER, this->_bonesUniformId, this->_bonesBufferId, 0, boneCount * sizeof(float) * 16);
    }

};

template <class PositionType, class NormalType, class TexcoordType, class ColorType, class BoneType>
class Shader<PositionType, NormalType, TexcoordType, ColorType, BoneType> : public SkinnedShader
{
public:
    Shader()
        : _vertexAttributeName("vertex"), _normalAttributeName("normal"),
          _texcoordAttributeName("texcoord"), _colorAttributeName("color"),
          _boneAttributeName("bone")
    { }

    virtual ~Shader() { }

    std::string _vertexAttributeName;
    std::string _normalAttributeName;
    std::string _texcoordAttributeName;
    std::string _colorAttributeName;
    std::string _boneAttributeName;

    void setupAttributes() const
    {
        auto vertexSize = sizeof(PositionType) + sizeof(NormalType) + sizeof(TexcoordType) + sizeof(ColorType) + sizeof(BoneType);

        GLuint vertexAttrib = glGetAttribLocation(this->_shaderId, this->_vertexAttributeName.c_str());
        glVertexAttribPointer(vertexAttrib, sizeof(PositionType) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, 0);
        glEnableVertexAttribArray(vertexAttrib);

        GLuint normalAttrib = glGetAttribLocation(this->_shaderId, this->_normalAttributeName.c_str());
        glVertexAttribPointer(normalAttrib, sizeof(NormalType) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid*>(sizeof(PositionType)));
        glEnableVertexAttribArray(normalAttrib);

        GLuint texcoordAttrib = glGetAttribLocation(this->_shaderId, this->_texcoordAttributeName.c_str());
        glVertexAttribPointer(texcoordAttrib, sizeof(TexcoordType) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid*>(sizeof(PositionType) + sizeof(NormalType)));
        glEnableVertexAttribArray(texcoordAttrib);

        GLuint colorAttrib = glGetAttribLocation(this->_shaderId, this->_colorAttributeName.c_str());
        glVertexAttribPointer(colorAttrib, sizeof(ColorType) / sizeof(float), GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid*>(sizeof(PositionType) + sizeof(NormalType) + sizeof(TexcoordType)));
        glEnableVertexAttribArray(colorAttrib);

        GLuint boneAttrib = glGetAttribLocation(this->_shaderId, this->_boneAttributeName.c_str());
        glVertexAttribPointer(boneAttrib, sizeof(BoneType) / sizeof(int), GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid*>(sizeof(PositionType) + sizeof(NormalType) + sizeof(TexcoordType) + sizeof(ColorType)));
        glEnableVertexAttribArray(boneAttrib);
    }
};

#endif // GL_UTILITIES_SHADERS_H
