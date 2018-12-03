#pragma once

#include <stb_image.h>

#include "Group.h"
#include <stdlib.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Group::Group()
{
	faces = new std::vector<Face*>();

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	type = NONE;
	this->hasMaterial = false;
}

Group::~Group() {}

void Group::AddFace(std::vector<int>* vertexIndices, std::vector<int>* normalIndices, std::vector<int>* mappingIndices)
{
	if (vertexIndices->size() > 3) {
		//faces must be divided on 1, 2, 3 e 1, 3, 4
		std::vector<int>* verFace1 = new std::vector<int>();
		std::vector<int>* norFace1 = new std::vector<int>();
		std::vector<int>* mapFace1 = new std::vector<int>();
		std::vector<int>* verFace2 = new std::vector<int>();
		std::vector<int>* norFace2 = new std::vector<int>();
		std::vector<int>* mapFace2 = new std::vector<int>();

		//first element belongs to the first and second face
		verFace1->push_back(vertexIndices->front());
		verFace2->push_back(vertexIndices->front());
		vertexIndices->erase(vertexIndices->begin());
		if (normalIndices->size() > 0 && mappingIndices->size() > 0) {
			norFace1->push_back(normalIndices->front());
			norFace2->push_back(normalIndices->front());
			normalIndices->erase(normalIndices->begin());
			mapFace1->push_back(mappingIndices->front());
			mapFace2->push_back(mappingIndices->front());
			mappingIndices->erase(mappingIndices->begin());
		}
		//second element belongs to first face only
		verFace1->push_back(vertexIndices->front());
		vertexIndices->erase(vertexIndices->begin());
		if (normalIndices->size() > 0 && mappingIndices->size() > 0) {
			norFace1->push_back(normalIndices->front());
			normalIndices->erase(normalIndices->begin());
			mapFace1->push_back(mappingIndices->front());
			mappingIndices->erase(mappingIndices->begin());
		}
		//third element belongs to both faces
		verFace1->push_back(vertexIndices->front());
		verFace2->push_back(vertexIndices->front());
		vertexIndices->erase(vertexIndices->begin());
		if (normalIndices->size() > 0 && mappingIndices->size() > 0) {
			norFace1->push_back(normalIndices->front());
			norFace2->push_back(normalIndices->front());
			normalIndices->erase(normalIndices->begin());
			mapFace1->push_back(mappingIndices->front());
			mapFace2->push_back(mappingIndices->front());
			mappingIndices->erase(mappingIndices->begin());
		}
		//fourth element only belongs to the second face
		verFace2->push_back(vertexIndices->front());
		vertexIndices->erase(vertexIndices->begin());
		if (normalIndices->size() > 0 && mappingIndices->size() > 0) {
			norFace2->push_back(normalIndices->front());
			normalIndices->erase(normalIndices->begin());
			mapFace2->push_back(mappingIndices->front());
			mappingIndices->erase(mappingIndices->begin());
		}
		//add the two faces to the group	
		Face* newFace1 = new Face(verFace1, norFace1, mapFace1);
		this->faces->push_back(newFace1);
		Face* newFace2 = new Face(verFace2, norFace2, mapFace2);
		this->faces->push_back(newFace2);
	}
	else if (vertexIndices->size() == 3) {
		Face* newFace = new Face(vertexIndices, normalIndices, mappingIndices);
		this->faces->push_back(newFace);
	}
}

void Group::Bind(std::vector<GLfloat> *vertices) {
	//texture is only set if the group has a material
	if (this->HasMaterial()) {
		if (this->GetMaterial()->GetHasTexture()) { //texture is only set if the material has a texture
													//create the texture
			unsigned int texture;
			glGenTextures(1, &texture);
			GLint textureId = this->GetMaterial()->getTextureId();

			glActiveTexture(GL_TEXTURE0 + textureId);
			glBindTexture(GL_TEXTURE_2D, textureId);
			

			int textureLocation = shaderObj->Uniform("texture1");
			glUniform1i(textureLocation, textureId);

			std::string textureName = this->GetMaterial()->GetMapKd();
			char *texture_name = &textureName[0];	

			shaderObj->LoadTexture(texture_name, "texture1", textureName);
			
			//ligthing
			//Ka
			glm::vec3 Ka = this->GetMaterial()->GetKa();
			int KaLocation = shaderObj->Uniform("Ka");
			if (Ka.r == 0 && Ka.g == 0 && Ka.b == 0)
				glUniform3f(KaLocation, 1.0, 1.0, 1.0);
			else
				glUniform3f(KaLocation, Ka.r, Ka.g, Ka.b);

			//Kd
			glm::vec3 Kd = this->GetMaterial()->GetKd();
			int KdLocation = shaderObj->Uniform("Kd");
			if (Kd.r == 0 && Kd.g == 0 && Kd.b == 0)
				glUniform3f(KdLocation, 1.0, 0.5, 0.0);
			else
				glUniform3f(KdLocation, Kd.r, Kd.g, Kd.b);

			//Ks
			glm::vec3 Ks = this->GetMaterial()->GetKs();
			int KsLocation = shaderObj->Uniform("Ks");
			if (Ks.r == 0 && Ks.g == 0 && Ks.b == 0)
				glUniform3f(KsLocation, 1.0, 1.0, 1.0);
			else
				glUniform3f(KsLocation, Ks.r, Ks.g, Ks.b);

			//Ns
			float Ns = this->GetMaterial()->GetNs();
			int NsLocation = shaderObj->Uniform("Ns");
			if (Ns == 0)
				glUniform1f(NsLocation, 100);	
			else
				glUniform1f(NsLocation, Ns);
		}
		
	}

	if (type == NONE) {
		DefineType();
	}

	if (type != GroupType::EMPTY) {

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices->size(), &vertices->at(0), GL_STATIC_DRAW);

		int stride = GetStride();
		int points = 0;

		//Vertexes
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid *)0);
		glEnableVertexAttribArray(0);
		points += 3;

		if (type == GroupType::V_T || type == GroupType::V_T_N) {
			//Texture
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid *)(points * sizeof(GLfloat)));
			/*
			Properties of VertexAttribPointer:
			1 - layout on shader
			2 - how many numbers to read
			3 - GL_FLOAT
			4 - GL_FALSE
			5 - interval of numbers from each number to be read, stride determines that
			6 - where to start, points is based on the type of the group (V, VT, VTN...)
			*/
			glEnableVertexAttribArray(1);
			points += 2;
		}

		if (type == GroupType::V_N || type == GroupType::V_T_N) {
			//Normals
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid *)(points * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);
		}
		glBindVertexArray(0);
	}

}
void Group::DefineType()
{
	auto tV = faces->at(0)->GetVerts()->size();
	auto tN = faces->at(0)->GetNorms()->size();
	auto tT = faces->at(0)->GetTexts()->size();

	if (tV == 0) {
		type = EMPTY;
	}
	else {
		if (tT > 0) { // Has T
			if (tN > 0) { // Has N
				type = GroupType::V_T_N;
			}
			else {
				type = GroupType::V_T;
			}
		}
		else { // No T
			if (tN > 0) { // Has N
				type = GroupType::V_N;
			}
			else { // No N
				type = GroupType::V;
			}
		}
	}
}

int Group::GetStride()
{
	switch (type)
	{
	case V:
		return 3;
	case V_T:
		return 4;
	case V_N:
		return 5;
	case V_T_N:
		return 8;
	default:
		return 0;
	}
}
