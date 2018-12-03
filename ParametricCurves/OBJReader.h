#ifndef OBJREADER_H
#define OBJREADER_H

#include <GL\glew.h>
#include <glm\vec3.hpp>
#include <glm\vec2.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>

#include "Mesh.h"
#include "MTLReader.h"
#include <map>

namespace OBJReader
{

	enum ObjTypes {
		NONE,
		V,
		VN,
		VT,
		F,
		G,
		MTLLIB,
		USEMTL,
		COMMENTARY,
	};

	typedef std::map<std::string, ObjTypes> TypesMap;
	static TypesMap typesMap = {
		{ "v", ObjTypes::V },
		{ "vn", ObjTypes::VN },
		{ "vt", ObjTypes::VT },
		{ "f", ObjTypes::F },
		{ "g", ObjTypes::G },
		{ "mtllib", ObjTypes::MTLLIB},
		{ "usemtl", ObjTypes::USEMTL },
		{ "#", ObjTypes::COMMENTARY },
	};


	static Mesh* Read(const GLchar* path) {

		Mesh* newMesh = new Mesh();

		std::ifstream file;
		file.exceptions(std::ifstream::badbit);

		Group* currentGroup = nullptr;

		try {
			file.open(path);

			std::cout << path;

			if (!file.is_open()) {
				std::cout << "ERRO::OBJREADER::ERRO NA ABERTURA DO ARQUIVO";
			}

			std::string line, temp;
			std::stringstream sstream;
			int lineCounter = 1;

			while (!file.eof()) {

				sstream = std::stringstream();
				line = temp = "";

				std::getline(file, line);

				sstream << line;
				sstream >> temp;

				ObjTypes type = typesMap[temp];

				switch (type)
				{
				case OBJReader::V:
				{
					float x, y, z;
					sstream >> x >> y >> z;
					newMesh->AddVertex(glm::vec3(x, y, z));
					break;
				}
				case OBJReader::VN:
				{
					float x, y, z;
					sstream >> x >> y >> z;
					newMesh->AddNormal(glm::vec3(x, y, z));
					break;
				}
				case OBJReader::VT:
				{
					float x, y;
					sstream >> x >> y;
					// test to invert v = 1 - v to treat objs created on blender
					y = 1.0 - y;
					newMesh->AddMapping(glm::vec2(x, y));
					break;
				}
				case OBJReader::F:
				{
					std::vector<int> *posV = new std::vector<int>;
					std::vector<int> *normV = new std::vector<int>;
					std::vector<int> *texV = new std::vector<int>;
					std::string token, valueAux;
					std::stringstream tokenStream;

					while (sstream.rdbuf()->in_avail()) {

						token = valueAux = "";
						sstream >> token;

						tokenStream = std::stringstream();
						tokenStream << token;

						if (!token.find('/')) { // Only vertices
							while (tokenStream.rdbuf()->in_avail()) {
								std::getline(tokenStream, valueAux, '/');
								posV->push_back(std::atoi(&valueAux[0]));
							}
						}
						else {

							while (tokenStream.rdbuf()->in_avail()) {
								// Get individual values

								std::getline(tokenStream, valueAux, '/');

								posV->push_back(std::atoi(&valueAux[0]));

								std::getline(tokenStream, valueAux, '/');
								if (valueAux == "") { // v//n
									std::getline(tokenStream, valueAux, '/');
									normV->push_back(std::atoi(&valueAux[0]));
								}
								else {
									texV->push_back(std::atoi(&valueAux[0])); // v/t

									std::getline(tokenStream, valueAux, '/');
									if (valueAux != "") {
										normV->push_back(std::atoi(&valueAux[0])); // v/t/n
									}
								}
							}
						}

					}

					if (currentGroup != nullptr) {
						currentGroup->AddFace(posV, normV, texV);
					}
					else {
						std::cout << "ERRO::OBJREADER::GRUPO NÃO DEFINIDO PARA FACE NA LINHA " << lineCounter << std::endl;

						bool addedGroup = false;
						std::vector<Group*> *tempGroups = newMesh->GetGroups();
						for (std::vector<Group*>::iterator it = tempGroups->begin(); it != tempGroups->end(); ++it) {
							if ("NO_GROUP" == (*it)->GetName()) { // Checking if a group with that name already exists
								currentGroup = *it;
								addedGroup = true;
								break;
							}
						}
						if (!addedGroup) {
							newMesh->AddGroup("NO_GROUP");
							currentGroup = newMesh->GetGroups()->back();
						}

					}

					break;
				}
				case OBJReader::G:
				{
					std::string name;
					std::getline(sstream, name);

					bool addGroup = true;
					std::vector<Group*> *tempGroups = newMesh->GetGroups();
					for (std::vector<Group*>::iterator it = tempGroups->begin(); it != tempGroups->end(); ++it) {
						if (name == (*it)->GetName()) { // Checking if a group with that name already exists
							addGroup = false;
							currentGroup = *it;
							break;
						}
					}

					if (addGroup) {
						newMesh->AddGroup(name);
						currentGroup = newMesh->GetGroups()->back();
					}

					break;
				}

				case OBJReader::MTLLIB:
				{
					std::string mtlFile;
					std::getline(sstream, mtlFile);
					newMesh->setMaterialFile(mtlFile.substr(1, std::string::npos));
					break;
				}
				case OBJReader::USEMTL:
				{
					std::string mtlGroup;
					std::getline(sstream, mtlGroup);
					currentGroup->SetMaterialName(mtlGroup.substr(1, std::string::npos));
					break;

				}
				case OBJReader::COMMENTARY:
				case OBJReader::NONE:
					break;
				default:
					std::cout << "Erro na leitura da linha " << lineCounter << std::endl;
					break;
				}

				lineCounter++;
			}

			file.close();
			return newMesh;
		}
		catch (const std::ifstream::failure& e) {

			if (!file.eof()) {
				std::cout << "ERRO::OBJREADER::O ARQUIVO NÃO PODE SER LIDO" << std::endl;
			}

		}

		file.close();
		return nullptr;
	}
}

#endif