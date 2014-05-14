#include "scenediffuse.h"

#include <string>
using std::string;
#include <cstdio>
#include <cstdlib>
#include <iostream>
using std::cerr;
using std::endl;
using std::cout;

#include <sstream>
using std::istringstream;
#include <fstream>
using std::ifstream;

#include "glutils.h"

using glm::vec3;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/type_ptr.hpp>

SceneDiffuse::SceneDiffuse()
{
}

void SceneDiffuse::initScene()
{
	cout << "Compiling Shaders" << endl;
	compileAndLinkShader();

	glEnable(GL_DEPTH_TEST);

	cout << "Loading Camera Matrix" << endl;
	glm::mat4 camMatrix;
	loadCameraMatrices("cameraMatrix.txt", 100, camMatrix);
	view = camMatrix;
	
	cout << "Loading Mesh" << endl;

	//ogre = new VBOMesh("../media/bs_ears.obj", true, false, false);
	ogre = new VBOMesh("C:\\Android&BMO.obj", true, false, false);
	//torus = new VBOTorus(0.7f, 0.3f, 30, 30);

	model = mat4(1.0f);
	//view = glm::lookAt(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	
	projection = mat4(1.0f);

	prog.setUniform("Kd", 0.9f, 0.5f, 0.3f);
	prog.setUniform("Ld", 1.0f, 1.0f, 1.0f);
	prog.setUniform("LightPosition", view * vec4(5.0f, 5.0f, 2.0f, 1.0f));

}

void SceneDiffuse::update(float t)
{

}

void SceneDiffuse::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setMatrices();
	ogre->render();
}

void SceneDiffuse::setMatrices()
{
	mat4 mv = view * model;
	prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("NormalMatrix",
		mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	prog.setUniform("MVP", projection * mv);
}

void SceneDiffuse::resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	projection = glm::perspective(70.0f, (float)w / h, 0.3f, 100.0f);
}

void SceneDiffuse::compileAndLinkShader()
{
	try {
		prog.compileShader("diffuse.vert");
		prog.compileShader("diffuse.frag");
		prog.link();
		prog.validate();
		prog.use();
	}
	catch (GLSLProgramException & e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneDiffuse::loadCameraMatrices(const char * fileName, int matIndex, glm::mat4 &camMatrix){
	ifstream objStream(fileName, std::ios::in);

	if (!objStream) {
		cerr << "Unable to open matrice file: " << fileName << endl;
		exit(1);
	}

	if (matIndex == 0){
		cout << "0 index is identity, no need to load a matrix";
		return;
	}
	string line;
	//getline(objStream, line);
	for (int i = 0; i < matIndex +1; i++){
		if (!objStream.eof()) {
			getline(objStream, line);
		}
		else{
			cout << "Index out of bounds" << endl;
			return;
		}
	}

	objStream.close();

	float* camArray = new float[16];
	if (line.length() > 0){
		istringstream lineStream(line);
		lineStream >> camArray[0] >> camArray[1] >> camArray[2] >> camArray[3] >> camArray[4] >> camArray[5] >> camArray[6] >> camArray[7] >> camArray[8]
			>> camArray[9] >> camArray[10] >> camArray[11] >> camArray[12] >> camArray[13] >> camArray[14] >> camArray[15];
	}

	camMatrix = glm::make_mat4(camArray);
}

void SceneDiffuse::trimString(string & str) {
	const char * whiteSpace = " \t\n\r";
	size_t location;
	location = str.find_first_not_of(whiteSpace);
	str.erase(0, location);
	location = str.find_last_not_of(whiteSpace);
	str.erase(location + 1);
}