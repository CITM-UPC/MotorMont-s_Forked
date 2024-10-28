#include <iostream>
#include <string>
#include <IL/il.h>
#include <IL/ilu.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include <thread>
#include "MyGameEngine/Camera.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/GameObject.h"
#include "MyWindow.h"

using namespace std;
using hrclock = chrono::high_resolution_clock;
using vec3 = glm::dvec3;

static const glm::ivec2 WINDOW_SIZE(1280, 720);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

static Camera camera;
static GameObject go;
SDL_Event event;
bool rightMouseButtonDown = false;
int lastMouseX, lastMouseY;

// Inicialización de OpenGL
void initOpenGL() {
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

void configureCamera() {
    glm::dmat4 projectionMatrix = glm::perspective(glm::radians(45.0), static_cast<double>(WINDOW_SIZE.x) / WINDOW_SIZE.y, 0.1, 100.0);
    glm::dmat4 viewMatrix = camera.view();

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(glm::value_ptr(projectionMatrix));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(glm::value_ptr(viewMatrix));
}
std::string getFileExtension(const std::string& filePath) {
    // Find the last dot in the file path
    size_t dotPosition = filePath.rfind('.');

    // If no dot is found, return an empty string
    if (dotPosition == std::string::npos) {
        return "";
    }

    // Extract and return the file extension
    return filePath.substr(dotPosition + 1);
}

// Función para cargar archivos en GameObject
void handleFileDrop(const std::string& filePath) {
    auto extension = getFileExtension(filePath);
    auto mesh = make_shared<Mesh>();
    auto imageTexture = make_shared<Image>();

    if (extension == "obj" || extension == "fbx" || extension == "dae") {
        mesh->LoadFile(filePath.c_str());
        go.setMesh(mesh);
    }
    else if (extension == "png" || extension == "jpg" || extension == "bmp") {
        imageTexture->loadTexture(filePath);
        go.setTextureImage(imageTexture);
    }
    else {
        cerr << "Unsupported file extension: " << extension << endl;
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

// Función de renderizado
void display_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    configureCamera();

    // Llamar aquí a la función de dibujo de tu objeto
    go.draw();

    // Otros elementos de la escena, como la cuadrícula, etc.
    drawFloorGrid(16, 0.25);
}

// Funciones de manejo de mouse
void mouseButton_func(int button, int state, int x, int y) {
    if (button == SDL_BUTTON_RIGHT) {
        rightMouseButtonDown = (state == SDL_PRESSED);
        lastMouseX = x;
        lastMouseY = y;
    }
}


float yaw = 0.0f; 
float pitch = 0.0f;
const float MAX_PITCH = 89.0f;

void mouseMotion_func(int x, int y) {
    if (rightMouseButtonDown) {
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        const double sensitivity = 0.1;

        yaw += deltaX * sensitivity;
        pitch -= deltaY * sensitivity;

        if (pitch > MAX_PITCH) pitch = MAX_PITCH;
        if (pitch < -MAX_PITCH) pitch = -MAX_PITCH;

        camera.transform().rotate(glm::radians(-deltaX * sensitivity), glm::vec3(0, -1, 0));
        camera.transform().rotate(glm::radians(deltaY * sensitivity), glm::vec3(-1, 0, 0));

        lastMouseX = x;
        lastMouseY = y;
    }
}


static void idle_func() {
    const float move_speed = 0.1f;
    const Uint8* state = SDL_GetKeyboardState(NULL);


    if (rightMouseButtonDown) {

        if (state[SDL_SCANCODE_W]) {
            std::cout << "Moving camera forward." << std::endl;
            camera.transform().translate(glm::vec3(0, 0, move_speed));
        }

        if (state[SDL_SCANCODE_S]) {
            std::cout << "Moving camera backward." << std::endl;
            camera.transform().translate(glm::vec3(0, 0, -move_speed));
        }

        if (state[SDL_SCANCODE_A]) {
            std::cout << "Moving camera left." << std::endl;
            camera.transform().translate(glm::vec3(move_speed, 0, 0));
        }

        if (state[SDL_SCANCODE_D]) {
            std::cout << "Moving camera right." << std::endl;
            camera.transform().translate(glm::vec3(-move_speed, 0, 0));
        }
    }
}

void mouseWheel_func(int direction) {
    camera.transform().translate(vec3(0, 0, direction * 0.1));
}

int main(int argc, char* argv[]) {
    ilInit();
    iluInit();

    MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);
    initOpenGL();

    // Posición inicial de la cámara
    camera.transform().pos() = vec3(0, 1, 4);
    camera.transform().rotate(glm::radians(180.0), vec3(0, 1, 0));

    while (window.processEvents() && window.isOpen()) {
        const auto t0 = hrclock::now();

        display_func();
		idle_func();
        window.swapBuffers();

        const auto t1 = hrclock::now();
        const auto dt = t1 - t0;
        if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_DROPFILE:
                handleFileDrop(event.drop.file);
                SDL_free(event.drop.file);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                mouseButton_func(event.button.button, event.button.state, event.button.x, event.button.y);
                break;
            case SDL_MOUSEMOTION:
                mouseMotion_func(event.motion.x, event.motion.y);
                break;
            case SDL_MOUSEWHEEL:
                mouseWheel_func(event.wheel.y);
                break;
            }
        }
    }

    return EXIT_SUCCESS;
}
