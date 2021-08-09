//
// Created by matf-rg on 30.10.20..
//

#ifndef PROJECT_BASE_TEXTURE2D_H
#define PROJECT_BASE_TEXTURE2D_H
#include <glad/glad.h>
#include <stb_image.h>
#include <rg/Error.h>

class Texture2D {
    unsigned int m_Id;
public:
    Texture2D(std::string pathToImg, GLenum filtering, GLenum sampling, GLenum rgbA) {
        unsigned int tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampling);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampling);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);

        int width, height, nChannel;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(pathToImg.c_str(), &width, &height, &nChannel, 0);

        if(data) {
            glTexImage2D(GL_TEXTURE_2D, 0, rgbA, width, height, 0, rgbA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            ASSERT( false, "Failed to load texture!\n");
        }

        stbi_image_free(data);
        m_Id = tex;
    }
    void active(GLenum e) {
        glActiveTexture(e);
        glBindTexture(GL_TEXTURE_2D, m_Id);
    }

};

#endif //PROJECT_BASE_TEXTURE2D_H
