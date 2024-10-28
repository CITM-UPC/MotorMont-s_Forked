#include <iostream>
#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <IL/il.h>
#include <IL/ilu.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Assimp/cimport.h" 
#include "Assimp/scene.h" 
#include "Assimp/postprocess.h"


#include "MyGameEngine/Camera.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/GameObject.h"

#include <IL/ilut.h>
using namespace std;




static Camera camera;
static GameObject loadedMesh;

//A bool to get all the keys if pressed
bool keyStates[256] = { false };
bool rightMouseButtonDown = false;
int lastMouseX, lastMouseY;

static std::array<array<glm::u8vec3, 256>, 256> texture;

static void initializeTexture() {
	for (int i = 0; i < texture.front().size(); i++)
	{
		for (int j = 0; j < texture.size(); j++)
		{
			const glm::u8vec3 whiteColor(255, 255, 255);
			const glm::u8vec3 blackColor(0, 0, 0);
			texture[i][j] = (i / 8 + j / 8) % 2 == 0 ? whiteColor : blackColor;
		}
	}

}


static void drawFloorGrid(int size, double step) {
	glColor3ub(0, 0, 0);
	glBegin(GL_LINES);
	for (double i = -size; i <= size; i += step) {
		glVertex3d(i, 0, -size);
		glVertex3d(i, 0, size);
		glVertex3d(-size, 0, i);
		glVertex3d(size, 0, i);
	}
	glEnd();
}

static void display_func() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(&camera.view()[0][0]);

	drawFloorGrid(16, 0.25);
	//red_triangle.draw();
	//green_triangle.draw();
	//blue_triangle.draw();
	//cube.draw();
	loadedMesh.draw();

	glutSwapBuffers();
}

static void init_opengl() {
	glewInit();

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);

	glClearColor(0.5, 0.5, 0.5, 1.0);
}

static void reshape_func(int width, int height) {
	glViewport(0, 0, width, height);
	camera.aspect = static_cast<double>(width) / height;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(&camera.projection()[0][0]);
}

static void mouseWheel_func(int wheel, int direction, int x, int y) {
	camera.transform().translate(vec3(0, 0, direction * 0.1));
}
static void mouse_func(int button, int state, int x, int y) {
	if (button == GLUT_RIGHT_BUTTON) {
		rightMouseButtonDown = (state == GLUT_DOWN);
		lastMouseX = x;
		lastMouseY = y;
	}
}
static void motion_func(int x, int y) {
	if (rightMouseButtonDown) {
		int deltaX = x - lastMouseX;
		int deltaY = y - lastMouseY;

		
		const double sensitivity = 0.1;
		double angleX = deltaX * sensitivity;
		double angleY = deltaY * sensitivity;

		
		camera.transform().rotate(glm::radians(-angleX), vec3(0, 1, 0));
		camera.transform().rotate(glm::radians(-angleY), vec3(1, 0, 0));

		lastMouseX = x;
		lastMouseY = y;
	}
}
void keyboardDown_func(unsigned char key, int x, int y) {
	keyStates[key] = true;
}

void keyboardUp_func(unsigned char key, int x, int y) {
	keyStates[key] = false;
}
//Similar to update function in SDL
static void idle_func() {
	const double move_speed = 0.1;
	int modifiers = glutGetModifiers();

	if (rightMouseButtonDown) {
		if (keyStates['w'] || keyStates['W']) {
			camera.transform().translate(vec3(0, 0, move_speed));
		}
		if (keyStates['s'] || keyStates['S']) {
			camera.transform().translate(vec3(0, 0, -move_speed));
		}
		if (keyStates['a'] || keyStates['A']) {
			camera.transform().translate(vec3(move_speed, 0, 0));
		}
		if (keyStates['d'] || keyStates['D']) {
			camera.transform().translate(vec3(-move_speed, 0, 0));
		}
	}

	// Animate triangles
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	ilInit();
	iluInit();
	ilutInit();
	// Iniit window and context
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Glut Simple Example");

	// Init OpenGL
	init_opengl();

	// Init camera
	camera.transform().pos() = vec3(0, 1, 4);
	camera.transform().rotate(glm::radians(180.0), vec3(0, 1, 0));


	auto mesh = make_shared<Mesh>();
	auto imageTexture = make_shared<Image>();

	mesh->LoadFile("BakerHouse.fbx");
	loadedMesh.setMesh(mesh);
	imageTexture->loadTexture("Baker_house.png");
	loadedMesh.setTextureImage(imageTexture);

	//loadedMesh.loadFile("BakerHouse.fbx");
	////loadedMesh.CreateDefaultTexture();
	//loadedMesh.loadTexture("Baker_house.png");



	// Set Glut callbacks
	glutDisplayFunc(display_func);
	glutIdleFunc(idle_func);
	glutReshapeFunc(reshape_func);

	//Camera movement
	glutMouseWheelFunc(mouseWheel_func);
	glutKeyboardFunc(keyboardDown_func);
	glutKeyboardUpFunc(keyboardUp_func);
	glutMouseFunc(mouse_func);
	glutMotionFunc(motion_func);


	// Enter glut main loop
	glutMainLoop();

	return EXIT_SUCCESS;
}