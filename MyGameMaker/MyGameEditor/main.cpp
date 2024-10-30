#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp> 
#include <glm/gtx/orthonormalize.hpp>
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
#include <vector>

#include "MyGameEngine/Camera.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/GameObject.h"
#include "MyWindow.h"
#include "BasicShapesManager.h"
#include "MyGui.h"

using namespace std;
using hrclock = chrono::high_resolution_clock;
using vec3 = glm::dvec3;

static const glm::ivec2 WINDOW_SIZE(1280, 720);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

static Camera camera;

static vector<GameObject> gameObjects;
GameObject* selectedObject = nullptr;
SDL_Event event;
bool rightMouseButtonDown = false;
int lastMouseX, lastMouseY;

// Inicializaci�n de OpenGL
void initOpenGL() {
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}


//Mouse relative positions
glm::vec2 getMousePosition() {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return glm::vec2(static_cast<float>(x), static_cast<float>(y));
}
glm::vec3 screenToWorld(const glm::vec2& mousePos, float depth, const glm::mat4& projection, const glm::mat4& view) {
    // Paso 1: Convertir coordenadas de pantalla a NDC (Normalized Device Coordinates)
    float x = (2.0f * mousePos.x) / WINDOW_SIZE.x - 1.0f;
    float y = 1.0f - (2.0f * mousePos.y) / WINDOW_SIZE.y;  // Invertir Y
    glm::vec4 clipCoords(x, y, -1.0f, 1.0f); // El espacio de clip est� en -1 en Z

    // Paso 2: Convertir desde coordenadas de clip a espacio de c�mara
    glm::vec4 eyeCoords = glm::inverse(projection) * clipCoords;
    eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);

    // Paso 3: Convertir a coordenadas de mundo
    glm::vec3 worldRay = glm::vec3(glm::inverse(view) * eyeCoords);
    worldRay = glm::normalize(worldRay);

    // Multiplicar el rayo por la profundidad para obtener el punto exacto en el espacio 3D
    glm::vec3 cameraPosition = glm::vec3(glm::inverse(view)[3]); // Posici�n de la c�mara en el espacio mundial
    return cameraPosition + worldRay * depth;
}



//RayCastFunctions

glm::vec3 getRayFromMouse(int mouseX, int mouseY, const glm::mat4& projection, const glm::mat4& view, const glm::ivec2& viewportSize) {
    float x = (2.0f * mouseX) / viewportSize.x - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / viewportSize.y;
    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));
    return rayWorld;
}


bool intersectRayWithBoundingBox(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const BoundingBox& bbox) {
    float tmin = (bbox.min.x - rayOrigin.x) / rayDirection.x;
    float tmax = (bbox.max.x - rayOrigin.x) / rayDirection.x;

    // Asegurarse de que tmin sea el punto de entrada y tmax el punto de salida
    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (bbox.min.y - rayOrigin.y) / rayDirection.y;
    float tymax = (bbox.max.y - rayOrigin.y) / rayDirection.y;

    // Asegurarse de que tymin sea el punto de entrada y tymax el punto de salida
    if (tymin > tymax) std::swap(tymin, tymax);

    // Verificar si hay alguna superposici�n en los intervalos de t para las direcciones X e Y
    if ((tmin > tymax) || (tymin > tmax)) {
        return false;
    }

    // Actualizar tmin y tmax para el intervalo combinado
    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    // Repetir el mismo proceso para la dimensi�n Z
    float tzmin = (bbox.min.z - rayOrigin.z) / rayDirection.z;
    float tzmax = (bbox.max.z - rayOrigin.z) / rayDirection.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    // Verificar si hay alguna superposici�n en los intervalos de t
    if ((tmin > tzmax) || (tzmin > tmax)) {
        return false;
    }

    // Actualizar tmin y tmax para el intervalo final combinado
    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    // Devolver verdadero si el rayo intersecta la AABB en una direcci�n positiva (tmin >= 0)
    return tmin >= 0.0f;
}
// Raycast desde el mouse para detectar si est� sobre un GameObject
GameObject* raycastFromMouseToGameObject(int mouseX, int mouseY, const glm::mat4& projection, const glm::mat4& view, const glm::ivec2& viewportSize) {
    glm::vec3 rayOrigin = glm::vec3(glm::inverse(view) * glm::vec4(0, 0, 0, 1));
    glm::vec3 rayDirection = getRayFromMouse(mouseX, mouseY, projection, view, viewportSize);

    GameObject* hitObject = nullptr;

    for (auto& go : gameObjects) {
        if (intersectRayWithBoundingBox(rayOrigin, rayDirection, go.boundingBox())) {
            hitObject = &go;
            break;
        }
    }

    return hitObject;
}



//File drop handler
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



void handleFileDrop(const std::string& filePath, mat4 projection, mat4 view) {
    auto extension = getFileExtension(filePath);
    auto mesh = make_shared<Mesh>();
    auto imageTexture = make_shared<Image>();
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    if (extension == "obj" || extension == "fbx" || extension == "dae") {
        GameObject go;
        mesh->LoadFile(filePath.c_str());
        go.setMesh(mesh);
        glm::vec3 mouseWorldPos = screenToWorld(glm::vec2(mouseX,mouseY), 10.0f, projection, view);
		go.transform().pos() = mouseWorldPos;
        gameObjects.push_back(go);

    }
    else if (extension == "png" || extension == "jpg" || extension == "bmp") {
        imageTexture->loadTexture(filePath);

        // Obtener posici�n actual del mouse
        

        // Detectar si el mouse est� sobre alg�n GameObject
        GameObject* hitObject = raycastFromMouseToGameObject(mouseX, mouseY, projection, view, WINDOW_SIZE);
        if (hitObject) {
            // Si hay un GameObject debajo del mouse, aplicar la textura
            hitObject->setTextureImage(imageTexture);
            cout << "Texture applied to GameObject under mouse." << endl;
        }
        else {
            cerr << "No GameObject under mouse to apply texture." << endl;
        }
    }
    else {
        cerr << "Unsupported file extension: " << extension << endl;
    }
}

//Renderizado del suelo
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
//spawn Initial house
void spawnBakerHouse() {
    GameObject go;
    auto mesh = make_shared<Mesh>();
    mesh->LoadFile("BakerHouse.fbx");
    go.setMesh(mesh);
    gameObjects.push_back(go);
}

void configureCamera() {
    glm::dmat4 projectionMatrix = glm::perspective(glm::radians(45.0), static_cast<double>(WINDOW_SIZE.x) / WINDOW_SIZE.y, 0.1, 100.0);
    glm::dmat4 viewMatrix = camera.view();

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(glm::value_ptr(projectionMatrix));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(glm::value_ptr(viewMatrix));
}
// Funci�n de renderizado
void display_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    configureCamera();

    for (auto& go : gameObjects) {
        go.draw();
    }

    // Otros elementos de la escena, como la cuadr�cula, etc.
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

//Camera rotation
float yaw = 0.0f;
float pitch = 0.0f;
const float MAX_PITCH = 89.0f;
bool altKeyDown = false;


void handleAltKey() {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    altKeyDown = state[SDL_SCANCODE_LALT] || state[SDL_SCANCODE_RALT];
}

void orbitCamera(const vec3& target, int deltaX, int deltaY) {
    const float sensitivity = 0.1f;

    
    yaw += deltaX * sensitivity;
    pitch -= deltaY * sensitivity;
    float distance = glm::length(camera.transform().pos() - target);

    vec3 newPosition;
    newPosition.x = target.x + distance * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newPosition.y = target.y + distance * sin(glm::radians(pitch));
    newPosition.z = target.z + distance * sin(glm::radians(yaw)) * cos(glm::radians(pitch));

   
    camera.transform().pos() = newPosition;
    camera.transform().lookAt(target); 
}
bool altPressedOnce = false; 
vec3 target;
void mouseMotion_func(int x, int y) {
    if (rightMouseButtonDown && altKeyDown == false) {
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
        camera.transform().alignCamera();
    }

    if (rightMouseButtonDown && altKeyDown) {
       
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        if (!altPressedOnce) {
            altPressedOnce = true;

            glm::vec2 mouseScreenPos = getMousePosition();

            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_SIZE.x / WINDOW_SIZE.y, 0.1f, 100.0f);
            glm::mat4 view = camera.view();
			if (selectedObject != nullptr) {
				target = selectedObject->transform().pos();

			}
			else {
				target = glm::vec3(0,0,0);
			}
            orbitCamera(target, deltaX, deltaY);
		}
        else {
            orbitCamera(target, deltaX, deltaY);
        }

      
       

        lastMouseX = x;
        lastMouseY = y;
        camera.transform().alignCamera();
	}
	else {
		altPressedOnce = false; // Reinicia la bandera si Alt no est� presionado
        /*
        
        TO DO: ARREGLAR BOOL QUE NO SE EJECUTE CADA FRAME
        
        
        */
	}
    
}
bool fKeyDown = false;
static void idle_func() {
    float move_speed = 0.1f;
    const Uint8* state = SDL_GetKeyboardState(NULL);


    if (state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT]) {
        move_speed = 0.2f;
    }
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

        if (state[SDL_SCANCODE_Q]) {
            std::cout << "Moving camera up." << std::endl;
            camera.transform().translate(glm::vec3(0, move_speed, 0));
        }

        if (state[SDL_SCANCODE_E]) {
            std::cout << "Moving camera down." << std::endl;
            camera.transform().translate(glm::vec3(0, -move_speed, 0));
        }
    }

    if (state[SDL_SCANCODE_F] && !fKeyDown && selectedObject != NULL) {
        camera.transform().pos() = selectedObject->transform().pos() + vec3(0, 1, 4);
        fKeyDown = true;
        camera.transform().lookAt(selectedObject->transform().pos());
        std::cout << "Camera looking at target." << std::endl;
    }
    else if (!state[SDL_SCANCODE_F]) {
        fKeyDown = false;
    }
    camera.transform().alignCamera();
}
//debug, showing the bounding boxes, not finished
inline static void glVertex3(const vec3& v) { glVertex3dv(&v.x); }
static void drawWiredQuad(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3) {
    glBegin(GL_LINE_LOOP);
    glVertex3(v0);
    glVertex3(v1);
    glVertex3(v2);
    glVertex3(v3);
    glEnd();
}

static void drawBoundingBox(const BoundingBox& bbox) {
    glLineWidth(2.0);
    drawWiredQuad(bbox.v000(), bbox.v001(), bbox.v011(), bbox.v010());
    drawWiredQuad(bbox.v100(), bbox.v101(), bbox.v111(), bbox.v110());
    drawWiredQuad(bbox.v000(), bbox.v001(), bbox.v101(), bbox.v100());
    drawWiredQuad(bbox.v010(), bbox.v011(), bbox.v111(), bbox.v110());
    drawWiredQuad(bbox.v000(), bbox.v010(), bbox.v110(), bbox.v100());
    drawWiredQuad(bbox.v001(), bbox.v011(), bbox.v111(), bbox.v101());

}

void mouseWheel_func(int direction) {
    camera.transform().translate(vec3(0, 0, direction * 0.1));
}

int main(int argc, char* argv[]) {
    ilInit();
    iluInit();

    MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);
    MyGUI gui(window.windowPtr(), window.contextPtr());
    initOpenGL();

    // Posici�n inicial de la c�mara
    camera.transform().pos() = vec3(0, 1, 4);
    camera.transform().rotate(glm::radians(180.0), vec3(0, 1, 0));
    spawnBakerHouse();

    

    while (window.isOpen()) {
        const auto t0 = hrclock::now();
		handleAltKey();
        // Obtener la posici�n actual del mouse
        glm::vec2 mouseScreenPos = getMousePosition();       

        display_func(); // Renderizar la escena
        gui.render();
        window.swapBuffers();

        const auto t1 = hrclock::now();
        const auto dt = t1 - t0;
        if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);

        SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
        
        while (SDL_PollEvent(&event))
        {
            // Obtener matrices de proyecci�n y vista de la c�mara
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_SIZE.x / WINDOW_SIZE.y, 0.1f, 100.0f);
            glm::mat4 view = camera.view();
          
            gui.processEvent(event);
            
            switch (event.type)
            {
			case SDL_QUIT:
				window.close();
				break;
            case SDL_DROPFILE:               
				cout << "File dropped: " << event.drop.file << endl;
                handleFileDrop(event.drop.file, projection, view);
                SDL_free(event.drop.file);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    // Raycast para detectar el objeto debajo del mouse
                    selectedObject = raycastFromMouseToGameObject(mouseScreenPos.x, mouseScreenPos.y, projection, view, WINDOW_SIZE);
                }
            case SDL_MOUSEBUTTONUP:
                mouseButton_func(event.button.button, event.button.state, event.button.x, event.button.y);
                break;
            case SDL_MOUSEMOTION:
                mouseMotion_func(event.motion.x, event.motion.y);
                break;
            case SDL_MOUSEWHEEL:
                mouseWheel_func(event.wheel.y);
                break;
            case SDL_KEYDOWN:
                glm::vec3 mouseWorldPos = screenToWorld(mouseScreenPos, 10.0f, projection, view);

                // Crear figuras en la posici�n 3D calculada
                switch (event.key.keysym.sym) {
                case SDLK_1:  // Crear Tri�ngulo
                    BasicShapesManager::createFigure(1, gameObjects, 1.0, mouseWorldPos);
                    break;
                case SDLK_2:  // Crear Cuadrado
                    BasicShapesManager::createFigure(2, gameObjects, 1.0, mouseWorldPos);
                    break;
                case SDLK_3:  // Crear Cubo
                    BasicShapesManager::createFigure(3, gameObjects, 1.0, mouseWorldPos);
                    break;
                default:
                    break;
                }
                break;
			default:
				cout << event.type << endl;
				break;
            }



        }
        idle_func();    // Actualizar l�gica de juego
    }
    return EXIT_SUCCESS;
}