#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

Shader::~Shader()
{
}

void Shader::LoadTexture(char* path, char* textureUniformName, std::string textureName)
{
	Texture tempTexture;
	tempTexture.Load(path, textureUniformName, program, textureQtd);
	textures[textureName] = tempTexture;
	textureQtd += 1;
}

GLint Shader::Attribute(const GLchar* name)
{
	return glGetAttribLocation(program, name);
}

GLint Shader::Uniform(const GLchar * name)
{
	return glGetUniformLocation(program, name);
}
