#ifndef SCENEDIFFUSE_H
#define SCENEDIFFUSE_H

#include "scene.h"
#include "glslprogram.h"
#include "vbomesh.h"
//#include "vbotorus.h"

#include "cookbookogl.h"

#include <glm/glm.hpp>
using glm::mat4;

class SceneDiffuse : public Scene
{
private:
	GLSLProgram prog;

	int width, height;
	bool loadCamMatFlag;
	//VBOTorus *torus;
	VBOMesh *ogre;

	mat4 model;
	mat4 view;
	mat4 projection;
	vector<mat4>* camMatVec;

	void setMatrices();
	void compileAndLinkShader();
	//void loadCameraMatrices(const char * fileName, int matIndex, glm::mat4 &camMatrix);
	void loadCameraMatrices(const char * fileName, vector<mat4>* camMatrix);
	void trimString(string & str);

public:
	SceneDiffuse();

	void initScene();
	void update(float t);
	void setCameraMat(int cameraMatIndex);
	void setLoadCamMatFlag(bool loadCameraMatrices);
	void render();
	void resize(int, int);
};

#endif // SCENEDIFFUSE_H
