#ifndef GL_UTILITIES_TEXTURES_H
#define GL_UTILITIES_TEXTURES_H

#ifdef _WIN32
#include <glad/glad.h>
#endif // _WIN32

#ifdef __ANDROID__
#include <GLES/gl.h>
#include <GLES3/gl3.h>
#endif // __ANDROID__

#include <string>
#include <iostream>

class Texture
{
    friend class TextureLoader;
    GLuint _textureId;
    int _width;
    int _height;
public:
    Texture() : _textureId(0) { }
    Texture(GLuint id) : _textureId(id) { }
    virtual ~Texture() { this->cleanup(); }

    void setup()
    {
        glGenTextures(1, &_textureId);
    }

    void use() const
    {
        glBindTexture(GL_TEXTURE_2D, this->_textureId);
    }

    void cleanup()
    {
        if (this->_textureId != 0)
        {
            glEnable(GL_TEXTURE_2D);
            glDeleteTextures(1, &this->_textureId);
            this->_textureId = 0;
        }
    }
    
    void setSize(int w, int h) { this->_width = w; this->_height = h; }
    int width() const { return this->_width; }
    int height() const { return this->_height; }
};

#endif // GL_UTILITIES_TEXTURES_H
