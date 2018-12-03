#ifndef FACE_H
#define FACE_H

#include <GL\glew.h>

class Face
{
public:

	Face() {
	};
	~Face() {};

	Face(std::vector<int>* vertsArr, std::vector<int>* normsArr, std::vector<int>* textsArr) {
		this->verts = vertsArr;
		this->norms = normsArr;
		this->texts = textsArr;
	}

	void SetVerts(std::vector<int>* arr) {
		verts = arr;
	};
	void SetNorms(std::vector<int>* arr) {
		norms = arr;
	};
	void SetTexts(std::vector<int>* arr) {
		texts = arr;
	};

	std::vector<int>* GetVerts(){
		return verts;
	};
	std::vector<int>* GetNorms(){
		return norms;
	};
	std::vector<int>* GetTexts(){
		return texts;
	};

	void Bind() { ; }

private:
	std::vector<int> *verts, *norms, *texts;

};

#endif
