#include <iostream>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "OBJReader.h"
#include "Time.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void lerArqCurva(const GLchar* path);
void ajustarTamanhoCurva(std::vector<glm::vec3*>* points, float factor);
float calcularAnguloOBJ(int indexA, int indexB);

int textureNum = 0;
//Tamanho da curva
float tamanhoCurva = 20.0f;

std::vector<glm::vec3*>* pontosCurva = new std::vector<glm::vec3*>();
std::vector<glm::vec3*>* scaledCurvePoints = new std::vector<glm::vec3*>();

//screen
const GLint WIDTH = 1000, HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 10.0f, 30.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

glm::vec3 lightPos(0.0f, 0.0f, 3.0f);

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "OBJ Reader", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (nullptr == window) {
		std::cout << "Falha ao criar janela GLFW" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit()) {
		std::cout << "Falha ao criar janela GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glViewport(0, 0, screenWidth, screenHeight);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDepthFunc(GL_LESS);

	Shader *coreShader = new Shader("Shaders/Core/core.vert", "Shaders/Core/core.frag");
	coreShader->Use();

	lerArqCurva("originalCurve.txt");
	ajustarTamanhoCurva(pontosCurva, tamanhoCurva);

	std::vector<Mesh*>* meshVec = new std::vector<Mesh*>();
	std::string objs = "bmw.obj curve.obj end";
	istringstream ss(objs);
	string temp;
	ss >> temp;
	while (temp != "end") {
		meshVec->push_back(OBJReader::Read(temp.c_str()));
		ss >> temp;
	}

	for (std::vector<Mesh*>::iterator obj = meshVec->begin(); obj != meshVec->end(); ++obj) {
		// Read MTL files to the meshes
		(*obj)->setMaterials(MTLReader::read((*obj)->GetMeterialFile(), textureNum));
	}

	for (std::vector<Mesh*>::iterator obj = meshVec->begin(); obj != meshVec->end(); ++obj) {
		// Print material list for all objs
		std::vector<Material*> *tempMats = (*obj)->GetMaterials();
		for (std::vector<Material*>::iterator mat = tempMats->begin(); mat != tempMats->end(); ++mat) {
			std::cout << (*mat)->GetName() << std::endl;
		}
	}

	// Assign materials to the groups within the meshes
	for (std::vector<Mesh*>::iterator obj = meshVec->begin(); obj != meshVec->end(); ++obj) {

		std::vector<Group*> *tempGroups = (*obj)->GetGroups();
		std::vector<Material*> *tempMaterials = (*obj)->GetMaterials();
		std::string name;
		// Iterate through the groups and add the materials to them
		for (std::vector<Group*>::iterator it = tempGroups->begin(); it != tempGroups->end(); ++it) {
			// Set shader on the group
			(*it)->SetShader(coreShader);
			for (std::vector<Material*>::iterator itMaterial = tempMaterials->begin(); itMaterial != tempMaterials->end(); ++itMaterial) {
				if ((*it)->GetMaterialName() == (*itMaterial)->GetName()) {
					Material *newMat = new Material((*itMaterial)->GetName());
					(*it)->SetMaterial((*itMaterial));
				}
			}
		}
	}
	// Bind all the meshes
	for (std::vector<Mesh*>::iterator obj = meshVec->begin(); obj != meshVec->end(); ++obj) {
		(*obj)->Bind();
	}

	glm::mat4 projection(1);
	projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

	float angle = 0.0f;
	int movementIndex = 0;

	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		processInput(window);

		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model(1);
		glm::mat4 view(1);

		projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		view = camera.GetViewMatrix();

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}

		GLint modelLoc = glGetUniformLocation(coreShader->program, "model");
		GLint viewLoc = glGetUniformLocation(coreShader->program, "view");
		GLint projLoc = glGetUniformLocation(coreShader->program, "projection");

		coreShader->Use();

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		std::vector<Group*>* currentGroups = nullptr;

		//iterate through the different meshes
		for (std::vector<Mesh*>::iterator obj = meshVec->begin(); obj != meshVec->end(); ++obj) {
			currentGroups = (*obj)->GetGroups();

			// Iterare through groups
			for (std::vector<Group*>::iterator group = currentGroups->begin(); group != currentGroups->end(); ++group) {
				if ((*group)->GetType() != GroupType::EMPTY && (*group)->GetType() != GroupType::NONE) {
					glBindVertexArray((*group)->VAO());

					int textureLocation = coreShader->Uniform("texture1");
					glEnable(GL_TEXTURE_2D);
					if ((*group)->HasMaterial()) {
						Material *mat = (*group)->GetMaterial();
						if (mat->GetHasTexture()) {
							int textureId = mat->getTextureId();
							glUniform1i(textureLocation, textureId);
							glBindTexture(GL_TEXTURE_2D, textureId);
						}
						if ((*group)->GetName() == " road") {
							glm::mat4 transform = glm::scale(model, glm::vec3(tamanhoCurva));
							glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(transform));
						}
						else {
							glm::mat4 transform = glm::translate(model, glm::vec3(pontosCurva->at(movementIndex)->x, pontosCurva->at(movementIndex)->y, pontosCurva->at(movementIndex)->z));
							angle = -calcularAnguloOBJ(movementIndex, movementIndex + 5);
							angle += 6.2; // Add 90º to fix initial direction from the motocycle							
							transform = glm::rotate(transform, angle, glm::vec3(0, 1, 0));
							glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(transform));
						}
					}
					glDrawArrays(GL_TRIANGLES, 0, (*group)->GetFacesSize() * 3);
					glDisable(GL_TEXTURE_2D);
				}
			}
		}
		movementIndex += 1;
		if (pontosCurva->size() - 5 == movementIndex)
			movementIndex = 0;
		glfwSwapBuffers(window);
	}
	coreShader->Delete();
	glfwTerminate();
	return EXIT_SUCCESS;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void ajustarTamanhoCurva(std::vector<glm::vec3*>* points, float factor) {
	for (int i = 0; i < points->size(); i++) {
		scaledCurvePoints->push_back(new glm::vec3(points->at(i)->x*factor, points->at(i)->y, points->at(i)->z*factor));
	}
	pontosCurva = scaledCurvePoints;
}

void lerArqCurva(const GLchar* path) {

	std::ifstream file;
	file.exceptions(std::ifstream::badbit);

	try {
		file.open(path);

		if (!file.is_open()) {
			std::cout << "ERRO::Pontos da Curva::ERRO NO ARQUIVO";
		}

		std::string line, temp;
		std::stringstream sstream;
		int lineCounter = 1;

		while (!file.eof()) {

			sstream = std::stringstream();
			line = temp = "";

			//get first line of the file
			std::getline(file, line);

			//get content of the line
			sstream << line;
			sstream >> temp;

			if (temp == "v") {
				float x, y, z;
				sstream >> x >> y >> z;
				pontosCurva->push_back(new glm::vec3(x, y, z));
			}
			lineCounter++;
		}
		file.close();
	}
	catch (const std::ifstream::failure& e) {
		if (!file.eof()) {
			std::cout << "ERROR::Pontos da Curva::ERRO NA LEITURA DO ARQUIVO" << std::endl;
		}
	}
}

float calcularAnguloOBJ(int indexA, int indexB) {

	glm::vec3* a = pontosCurva->at(indexA);
	glm::vec3* b;

	if (indexA == pontosCurva->size() - 5) {
		b = pontosCurva->at(0);
	}
	else {
		b = pontosCurva->at(indexB);
	}

	GLfloat dx = b->x - a->x;
	GLfloat dz = b->z - a->z;

	GLfloat angle = glm::atan(dz, dx);

	return angle;
}