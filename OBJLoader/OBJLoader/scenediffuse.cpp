#include "scenediffuse.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
using std::cerr;
using std::endl;
using std::cout;

#include "glutils.h"

using glm::vec3;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

SceneDiffuse::SceneDiffuse()
{
}

void SceneDiffuse::initScene()
{
	cout << "Compiling Shaders" << endl;
	compileAndLinkShader();

	glEnable(GL_DEPTH_TEST);

	cout << "Loading Mesh" << endl;

	//ogre = new VBOMesh("../media/bs_ears.obj", true, false, false);
	ogre = new VBOMesh("C:\\Android&BMO.obj", true, false, false);
	//torus = new VBOTorus(0.7f, 0.3f, 30, 30);

	model = mat4(1.0f);
	model *= glm::rotate(-35.0f, vec3(1.0f, 0.0f, 0.0f));
	model *= glm::rotate(35.0f, vec3(0.0f, 1.0f, 0.0f));
	view = glm::lookAt(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
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
