#pragma once

#ifndef MTLREADER_H
#define MTLREADER_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include "Material.h"

using namespace std;

class MTLReader
{

public:

	static std::vector<Material*>* read(string filePath, int &textureNum) {
		std::vector<Material*>* materials = new std::vector<Material*>();

		cout << "Leitura mtl " << filePath << endl;

		Material* current = NULL;

		ifstream myFile(filePath);
		while (!myFile.eof()) {
			string line;
			getline(myFile, line);
			while (line == "" || line.at(0) == '#') {
				getline(myFile, line);
				if (myFile.eof()) {
					return materials;
					cout << "Termina leitura mtl " << filePath << endl;
				}
			}
			istringstream ss(line);
			string temp;
			ss >> temp;
			if (temp == "newmtl") {
				string name;
				ss >> name;
				current = new Material(name);

				if (current != NULL) {
					materials->push_back(current);
				}
			}
			else if (temp == "Ka" || temp == "Kd" || temp == "Ks") {
				float r;
				float g;
				float b;
				ss >> r >> g >> b;
				if (temp == "Ka") {
					current->SetKa(r, g, b);
				}
				else if (temp == "Kd") {
					current->SetKd(r, g, b);
				}
				else if (temp == "Ks") {
					current->SetKs(r, g, b);
				}
			}
			else if (temp == "Ns") {
				float ns;
				ss >> ns;
				current->SetNs(ns);
			}
			else if (temp == "map_Kd") {
				string mapKd;
				ss >> mapKd;
				current->SetMap_Kd(mapKd);
				current->setTextureId(textureNum);
				textureNum++;
			}
		}

		cout << "Termina leitura mtl " << filePath << endl;
		return materials;
	}
};

#endif