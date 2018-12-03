#include "Mesh.h"
#include "Material.h"

Mesh::Mesh()
{
	vertexes = new std::vector<glm::vec3*>();
	normals = new std::vector<glm::vec3*>();
	mappings = new std::vector<glm::vec2*>();
	groups = new std::vector<Group*>();
	std::string material_file;
}

Mesh::~Mesh()
{
}

void Mesh::AddGroup()
{
	Group* newGroup = new Group();
	this->groups->push_back(newGroup);
}

void Mesh::AddGroup(std::string name)
{
	Group* newGroup = new Group();
	newGroup->SetName(name);
	this->groups->push_back(newGroup);
}

void Mesh::AddGroup(std::vector<int>* vertexIndices, std::vector<int>* normalIndices, std::vector<int>* mappingIndices)
{
	Group* newGroup = new Group();
	newGroup->AddFace(vertexIndices, normalIndices, mappingIndices);
	this->groups->push_back(newGroup);
}

void Mesh::AddVertex(glm::vec3 vertex)
{
	glm::vec3* newVertex = new glm::vec3(vertex);
	this->vertexes->push_back(newVertex);
}

void Mesh::AddNormal(glm::vec3 normal)
{
	glm::vec3* newNormal = new glm::vec3(normal);
	this->normals->push_back(newNormal);
}

void Mesh::AddMapping(glm::vec2 mapping)
{
	glm::vec2* newMapping = new glm::vec2(mapping);
	this->mappings->push_back(newMapping);
}

void Mesh::Bind()
{

	std::vector<GLfloat> *finalVector = new std::vector<GLfloat>();
	std::vector<int> *tempVertInd, *tempNormInd, *tempTexInd;
	std::vector<Face*>* facesTemp;

	// Iterate through groups
	for (std::vector<Group*>::iterator groupsIt = groups->begin(); groupsIt != groups->end(); ++groupsIt) {

		finalVector = new std::vector<GLfloat>();

		facesTemp = (*groupsIt)->GetFaces();

		// Iterate through faces 
		for (std::vector<Face*>::iterator facesIt = facesTemp->begin(); facesIt != facesTemp->end(); ++facesIt) {

			tempVertInd = new std::vector<int>;
			tempNormInd = new std::vector<int>;
			tempTexInd = new std::vector<int>;

			tempVertInd = (*facesIt)->GetVerts();
			tempNormInd = (*facesIt)->GetNorms();
			tempTexInd = (*facesIt)->GetTexts();

			for (int i = 0; i < tempVertInd->size(); i++) {
				// Index has -1 because vectors start at index 0 and obj at 1.

				glm::vec3 *tempVertex = (vertexes->at((*tempVertInd)[i] - 1));
				glm::vec3 *tempNormal = (normals->at((*tempNormInd)[i] - 1));
				glm::vec2 *tempMappin = (mappings->at((*tempTexInd)[i] - 1));

				finalVector->push_back(tempVertex->x);
				finalVector->push_back(tempVertex->y);
				finalVector->push_back(tempVertex->z);

				if (tempTexInd->size() > 0) {
					finalVector->push_back(tempMappin->s);
					finalVector->push_back(tempMappin->t);
				}

				if (tempNormInd->size() > 0) {
					finalVector->push_back(tempNormal->x);
					finalVector->push_back(tempNormal->y);
					finalVector->push_back(tempNormal->z);
				}

			}
		}

		if (finalVector->size() > 0) {
			(*groupsIt)->Bind(finalVector);
		}
	}	
}
