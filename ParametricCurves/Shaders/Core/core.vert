#version 330 core

layout ( location = 0 ) in vec3 vPosition;
layout ( location = 1 ) in vec2 vMapping;
layout ( location = 2 ) in vec3 vNormals;

out vec2 Mapping; 
out vec3 Position; 
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	Position = vPosition;
	Mapping = vMapping;
	Normal = vNormals;
	
	Position = vec3(view * model * vec4(vPosition, 1.0));
	Normal = mat3(transpose(inverse(model))) * vNormals;
	gl_Position = projection * vec4(Position, 1.0);	
}