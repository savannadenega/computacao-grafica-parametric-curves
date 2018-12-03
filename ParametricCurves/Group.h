#ifndef GROUP_H
#define GROUP_H
#endif

#include <GL\glew.h>

#include <string>
#include <vector>

#include "Shader.h"
#include "Face.h"
#include "Material.h"

enum GroupType
{
	NONE,
	EMPTY,
	V,
	V_T,
	V_N,
	V_T_N
};

class Group
{

public:

	Group();
	~Group();

	void SetName(std::string name) {
		this->name = name;
	};

	std::string GetName() {
		return name;
	};

	void SetMaterial(Material* material) {
		this->material = material;
		this->hasMaterial = true;
	};

	Material* GetMaterial() {
		return material;
	};

	void Group::SetMaterialName(std::string name) {
		this->nameMat = name;
	}

	std::string Group::GetMaterialName() { 
		return this->nameMat; 
	}

	bool HasMaterial() {
		if (this->hasMaterial)
			return true;
		else
			return false;
	};

	void AddFace(std::vector<int>* vertexIndices, std::vector<int>* normalIndices, std::vector<int>* mappingIndices);
	
	std::vector<Face*>* GetFaces() {
		return faces;
	}

	GLint GetFacesSize() {
		return faces->size();
	}

	void Bind(std::vector<GLfloat> *vertices);

	GLuint VAO() {
		return vbo;
	}
	GLuint VBO() {
		return vbo;
	}

	GroupType GetType() {
		return type;
	}

	void SetType(GroupType type) {
		this->type = type;
	}

	void SetShader(Shader* s) { 
		shaderObj = s; 
	}

	void DefineType();


private:

	std::string name;
	Material *material = NULL;
	std::string nameMat;
	bool hasMaterial;
	GroupType type;

	std::vector<Face*>* faces;
	Shader* shaderObj;
	GLuint vao, vbo;
	
	int GetStride();
};
