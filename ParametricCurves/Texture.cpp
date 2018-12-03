#include <stb_image.h>
#include <stdlib.h>

#include "Texture.h"

Texture::Texture()
{
}


Texture::~Texture()
{
}

unsigned char* Texture::LoadImage(char* path) {
	int width, height, nrChannels;
	unsigned char *image = SOIL_load_image(path, &width, &height, &nrChannels, SOIL_LOAD_RGBA);
	
	if (image) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glDisable(GL_TEXTURE_2D);
	return nullptr;
}


void Texture::Load(char* path, char* textureUniformName, GLuint shaderProgram, GLint textureNum)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	LoadImage(path);
}
