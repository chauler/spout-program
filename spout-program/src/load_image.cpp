#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <GL/glew.h>
#include <SpoutLibrary.h>
#include <iostream>

bool LoadTextureFromSender(SPOUTLIBRARY& receiver, GLuint* out_texture, int* out_width, int* out_height)
{
    if (receiver.IsConnected()) {
        std::cout << "Connected" << std::endl;
    }
    else {
        std::cout << "Not Connected" << std::endl;
    }
    // Load from file
    int image_width = 1920;
    int image_height = 1017;
    unsigned char* image_data = (unsigned char*)malloc(image_width * image_height * 4 * sizeof(unsigned char));
    if (receiver.IsUpdated()) {
        std::cout<<"updated"<<std::endl;
        receiver.ReceiveImage(image_data);
    }
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}