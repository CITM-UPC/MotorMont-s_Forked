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
using namespace std;

struct Triangle {
	Transform transform;
	glm::u8vec3 color;
	double size = 0.5;

	unsigned int texture_id;

	void draw() const {


		glEnable(GL_TEXTURE_2D);

		glPushMatrix(); //Save the modelview values of the matrix
		glMultMatrixd(&transform.mat()[0][0]);
		glColor3ub(color.r, color.g, color.b);
		glBegin(GL_TRIANGLES);

		glTexCoord2d(0.5, 0);
		glVertex2d(0, size);

		glTexCoord2d(0, 1);
		glVertex2d(-size, -size);

		glTexCoord2d(1, 1);
		glVertex2d(size, -size);
		glEnd();
		glPopMatrix(); //Recover the last values of the model view matrix

		glDisable(GL_TEXTURE_2D);

	}
};
//
//struct MyMesh {
//	unsigned int id_index = 0; // index in VRAM 
//	unsigned int num_index = 0;
//	unsigned int* index = nullptr;
//
//	unsigned int id_vertex = 0; // unique vertex in VRAM 
//	unsigned int  num_vertex = 0;
//	float* vertex = nullptr;
//
//public:
//
//	void InitBuffers() {
//
//		vec3 vertex[123];
//		sizeof(vertex);
//
//		glGenBuffers(1, &vertex_data_bufer_id);
//		glBindBuffer(GL_ARRAY_BUFFER, vertex_data_bufer_id);
//		glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(vec3), vertex_data.data(), GL_STATIC_DRAW);
//
//		glGenBuffers(1, &index_data_bufer_id);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_data_bufer_id);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data.size() * sizeof(unsigned int), index_data.data(), GL_STATIC_DRAW);
//
//	}
//};

struct Cube {
	Transform transform;
	glm::u8vec3 color = glm::u8vec3(111, 110, 0);
	double size = 0.5;

	vector<vec3> vertex_data = {
		vec3(-1, -1, -1),
		vec3(1, -1, -1),
		vec3(1, 1, -1),
		vec3(-1, 1, -1),
		vec3(-1, -1, 1),
		vec3(1, -1, 1),
		vec3(1, 1, 1),
		vec3(-1, 1, 1)
	};

	vector<unsigned int> index_data = {
		0, 1, 2, 0, 2, 3,
		1, 5, 6, 1, 6, 2,
		5, 4, 7, 5, 7, 6,
		4, 0, 3, 4, 3, 7,
		3, 2, 6, 3, 6, 7,
		4, 5, 1, 4, 1, 0
	};

	unsigned int vertex_data_bufer_id = 0;
	unsigned int index_data_bufer_id = 0;

	void InitBuffers() {

		vec3 vertex[123];
		sizeof(vertex);

		glGenBuffers(1, &vertex_data_bufer_id);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_data_bufer_id);
		glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(vec3), vertex_data.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &index_data_bufer_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_data_bufer_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data.size() * sizeof(unsigned int), index_data.data(), GL_STATIC_DRAW);

	}

	void draw() const {
		glPushMatrix(); //Save the modelview values of the matrix
		glMultMatrixd(&transform.mat()[0][0]);
		glColor3ub(color.r, color.g, color.b);
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_data_bufer_id);
		glVertexPointer(3, GL_DOUBLE, 0, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_data_bufer_id);
		glDrawElements(GL_TRIANGLES, index_data.size(), GL_UNSIGNED_INT, 0);
		glDisableClientState(GL_VERTEX_ARRAY);

		glPopMatrix(); //Recover the last values of the model view matrix
	}

};

static Camera camera;
static Triangle red_triangle;
static Triangle green_triangle;
static Triangle blue_triangle;
static Cube cube;

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
//
//static void LoadFile(const char* file_path) {
//	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);
//
//	if (scene != nullptr && scene->HasMeshes())
//	{
//		// Use scene->mNumMeshes to iterate on scene->mMeshes array aiReleaseImport(scene);
//		for (int i = 0; i < scene->mNumMeshes; i++)
//		{
//			MyMesh ourMesh;
//			ourMesh.num_vertex = scene->mMeshes[i]->mNumVertices;
//			ourMesh.vertex = new float[ourMesh.num_vertex * 3];
//			memcpy(ourMesh.vertex, scene->mMeshes[i]->mVertices, sizeof(float) * ourMesh.num_vertex * 3);
//
//			if (scene->mMeshes[i]->HasFaces())
//			{
//				ourMesh.num_index = scene->mMeshes[i]->mNumFaces * 3;
//				ourMesh.index = new unsigned int[ourMesh.num_index]; // assume each face is a triangle 
//				for (unsigned int j = 0; j < scene->mMeshes[i]->mNumFaces; ++j)
//				{
//					if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3) 
//					{
//						cout << "Warning, geometry didnt load" << endl;
//					}
//					else
//					{
//						memcpy(&ourMesh.index[i * 3], scene->mMeshes[i]->mFaces[j].mIndices, 3 * sizeof(unsigned int));
//					}
//				}
//			}
//
//		}
//	}
//	else
//		//LOG(“Error loading scene % s”, file_path);
//		cout << "Scene doesnt load properly" << endl;
//
//
//}

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
	red_triangle.draw();
	green_triangle.draw();
	blue_triangle.draw();
	cube.draw();

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

static void idle_func() {
	//animate triangles
	red_triangle.transform.rotate(0.001, vec3(0, 1, 0));
	green_triangle.transform.rotate(0.001, vec3(1, 0, 0));
	blue_triangle.transform.rotate(0.001, vec3(0, 0, 1));
	cube.transform.rotate(0.001, vec3(0, 1, 1));
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
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

	//Init cube
	cube.InitBuffers();

	// Init triangles
	red_triangle.transform.pos() = vec3(0, 1, 0);
	red_triangle.color = glm::u8vec3(255, 0, 0);
	green_triangle.transform.pos() = vec3(1, 1, 0);
	green_triangle.color = glm::u8vec3(0, 255, 0);
	blue_triangle.transform.pos() = vec3(0, 1, 1);
	blue_triangle.color = glm::u8vec3(0, 0, 255);


	ilInit();
	iluInit();
	auto il_img_id = ilGenImage();
	ilBindImage(il_img_id);
	//ilLoadImage("Lenna.png");

	auto img_width = ilGetInteger(IL_IMAGE_WIDTH);
	auto img_height = ilGetInteger(IL_IMAGE_HEIGHT);
	auto img_bpp = ilGetInteger(IL_IMAGE_BPP);
	auto img_format = ilGetInteger(IL_IMAGE_FORMAT);
	auto img_data = ilGetData();

	//Init Texture
	initializeTexture();
	unsigned int texture_id = 0;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, img_bpp, img_width, img_height, 0, img_format, GL_UNSIGNED_BYTE, img_data);
	ilDeleteImage(il_img_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
	/*red_triangle.texture_id = texture_id;
	blue_triangle.texture_id = texture_id;
	green_triangle.texture_id = texture_id;*/

	// Set Glut callbacks
	glutDisplayFunc(display_func);
	glutIdleFunc(idle_func);
	glutReshapeFunc(reshape_func);
	glutMouseWheelFunc(mouseWheel_func); //Mouse wheel movement 

	// Enter glut main loop
	glutMainLoop();

	return EXIT_SUCCESS;
}