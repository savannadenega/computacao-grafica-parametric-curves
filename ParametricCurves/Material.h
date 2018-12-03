#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include <GL\glew.h>

#include <vector>
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>

#include <string>
#include <vector>

class Material
{

public:

	Material(std::string name) {
		mat_name = name;
		SetKa(0, 0, 0);
		SetKd(0, 0, 0);
		SetKs(0, 0, 0);
		SetNs(0);
		setTextureId(-1);
		hasTexture = false;
	};
	Material() {}

	void SetName(std::string name) {
		this->mat_name = name;
	};

	std::string GetName() { return this->mat_name; }

	void SetKa(GLfloat r, GLfloat g, GLfloat b) {
		this->Ka = glm::vec3(r, g, b);
	};

	void SetKd(GLfloat r, GLfloat g, GLfloat b) {
		this->Kd = glm::vec3(r, g, b);
	};

	void SetKs(GLfloat r, GLfloat g, GLfloat b) {
		this->Ks = glm::vec3(r, g, b);
	};

	void SetNs(GLfloat ns) {
		this->Ns = ns;
	};

	void SetMap_Kd(std::string name) {
		this->map_Kd = name;
		this->hasTexture = true;
	};

	glm::vec3 GetKa() { return Ka; }
	glm::vec3 GetKd() { return Kd; }
	glm::vec3 GetKs() { return Ks; }
	GLfloat GetNs() { return Ns; }
	std::string GetMapKd() { return map_Kd; }
	bool GetHasTexture() {
		if (this->hasTexture)
			return true;
		else
			return false;
	}
	void setTextureId(int i) { textureId = i; }
	int getTextureId() { return textureId; }

private:

	std::string mat_name;
	glm::vec3 Ka, Kd, Ks;
	GLfloat Ns;
	std::string map_Kd;
	int textureId;
	bool hasTexture;
};

#endif