//
// Created by adminstrator on 13/04/2022.
//

#ifndef PROJECT_BASE_BLENDING2D_H
#define PROJECT_BASE_BLENDING2D_H

#include <glad/glad.h>
#include <stb_image.h>
#include <rg/Error.h>

class Blending2D {
    unsigned int b_Id;
public:
    Blending2D(std::string pathToImg) {
        unsigned int tex;
        glGenTextures(1, &tex);

        int width, height, nChannel;
        //stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(pathToImg.c_str(), &width, &height, &nChannel, 0);

        GLenum format;
        if(data) {
            if(nChannel == 1)
                format = GL_RED;
            else if (nChannel == 3)
                format = GL_RGB;
            else if (nChannel == 4)
                format = GL_RGBA;


            glBindTexture(GL_TEXTURE_2D, tex);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);




        }
        else {
            ASSERT( false, "Failed to load texture!\n");
        }

        stbi_image_free(data);
        b_Id = tex;
    }
    void active(GLenum e) {
        glActiveTexture(e);
        glBindTexture(GL_TEXTURE_2D, b_Id);
    }

};










#endif //PROJECT_BASE_BLENDING2D_H
