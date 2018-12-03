#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2.h>

#include <string>

class Texture
{
private:
	GLuint textureID;
	GLint textureLocation;
	GLshort textureNum;
	static unsigned char* LoadImage(char* path);


public:
	Texture();
	~Texture();

	void Load(char* path, char* textureUniformName, GLuint shaderProgram, GLint textureNum);

	GLuint GetTextureId() { return textureID; }
	GLshort GetTextureNum() { return textureNum; }

};

#endif