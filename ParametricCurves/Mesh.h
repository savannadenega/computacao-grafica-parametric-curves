#ifndef MESH_H
#define MESH_H

#include <GL\glew.h>
#include <vector>
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <string>
#include "Group.h"
#include "Material.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

class Mesh
{
public:
	Mesh();
	~Mesh();

	void AddGroup();
	void AddGroup(std::string name);
	void AddGroup(std::vector<int>* vertexIndices, std::vector<int>* normalIndices, std::vector<int>* mappingIndices);
	void AddVertex(glm::vec3 vertex);
	void AddNormal(glm::vec3 normal);
	void AddMapping(glm::vec2 mapping);

	void setMaterialFile(std::string mat) {	material_file = mat;}
	void setMaterials(std::vector<Material*>* material) { materials = material; }
	std::vector<Material*>* GetMaterials() { return materials; }

	std::vector<glm::vec3*>* GetVertexes() { return vertexes; }
	std::vector<glm::vec3*>* GetNormals() { return normals; }
	std::vector<glm::vec2*>* GetMappings() { return mappings; }
	std::string GetMeterialFile() { return material_file; }
	std::vector<Group*>* GetGroups() { return groups; }

	void Bind();

private:

	std::vector<glm::vec3*>* vertexes;
	std::vector<glm::vec3*>* normals;
	std::vector<glm::vec2*>* mappings;
	std::string material_file;

	std::vector<Group*>* groups;
	std::vector<Material*>* materials;

};

#endif