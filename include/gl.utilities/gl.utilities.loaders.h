#ifndef GL_UTILITIES_LOADERS_H
#define GL_UTILITIES_LOADERS_H

#include "gl-utilities-textures.h"
#include <cstdlib>

class TextureLoader
{
public:
// When STB image is included, we can load images trough this library
#ifdef STBI_INCLUDE_STB_IMAGE_H
    bool execute(Texture* texture, const std::string& filename)
    {
        int x = 0, y = 0, comp = 3;
        auto imageData = stbi_load(filename.c_str(), &x, &y, &comp, 4);
        if (imageData != nullptr)
        {
            texture->_width = x;
            texture->_height = y;
            std::cout << "loaded " << filename << std::endl;
            auto format = comp == 4 ? GL_RGBA : GL_RGB;
            glBindTexture(GL_TEXTURE_2D, texture->_textureId);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, format, x, y, 0, format, GL_UNSIGNED_BYTE, imageData);
            free(imageData);

            return true;
        }

        std::cout << "Unable to load " << filename << std::endl;
        return false;
    }

    bool execute(Texture* texture, const std::vector<unsigned char>& buffer)
    {
        int x = 0, y = 0, comp = 3;
        auto imageData = stbi_load_from_memory(buffer.data(), buffer.size(), &x, &y, &comp, 4);
        if (imageData != nullptr)
        {
            texture->_width = x;
            texture->_height = y;
            auto format = comp == 4 ? GL_RGBA : GL_RGB;
            glBindTexture(GL_TEXTURE_2D, texture->_textureId);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, format, x, y, 0, format, GL_UNSIGNED_BYTE, imageData);
            free(imageData);

            return true;
        }

        std::cout << "Unable to load texture from memory" << std::endl;
        return false;
    }

#endif // STBI_INCLUDE_STB_IMAGE_H
};

#endif // GL_UTILITIES_LOADERS_H
