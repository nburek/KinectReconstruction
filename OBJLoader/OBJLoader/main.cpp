
//#include <GL/glew.h>
#include <iostream>
#include "cookbookogl.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include "scene.h"
#include "glutils.h"
#include "scenediffuse.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#include <string>
using std::string;
using namespace std;

Scene *scene;
GLFWwindow *window;

string parseCLArgs(int argc, char * argv);
void printHelpInfo(const char *);
void error_callback(int error, const char* description);

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
	if (scene)
		scene->animate(!(scene->animating()));
}

void initializeGL() {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	scene->initScene();
}

void mainLoop() {
	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		GLUtils::checkForOpenGLError(__FILE__, __LINE__);
		scene->update(glfwGetTime());
		scene->render();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void resizeGL(int w, int h) {
	scene->resize(w, h);
}

int main(int argc, char *argv[])
{
	scene = new SceneDiffuse();

	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	// Open the window
	string title = "OBJ Loader";
	glfwSetErrorCallback(error_callback);
	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, title.c_str(), NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	// Load the OpenGL functions.
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	GLUtils::dumpGLInfo();

	// Initialization
	cout << "Initializing OpenGL" << endl;
	initializeGL();
	resizeGL(WIN_WIDTH, WIN_HEIGHT);

	// Enter the main loop
	cout << "Executing main loop" << endl;
	mainLoop();

	// Close window and terminate GLFW
	glfwTerminate();
	// Exit program
	exit(EXIT_SUCCESS);
}

void printHelpInfo(const char * exeFile) {
	printf("Usage: %s recipe-name\n\n", exeFile);
	printf("Recipe names: \n");
	printf("  basic              : Basic scene.\n");
	printf("  basic-attrib       : Prints active attributes.\n");
	printf("  basic-debug        : Display debug messages.\n");
	printf("  basic-uniform      : Basic scene with a uniform variable.\n");
	printf("  basic-uniform-block: Scene with a uniform block variable.\n");
}

void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}
