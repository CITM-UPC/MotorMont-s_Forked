#include "MyGUI.h"
#include "MyGameEngine/GameObject.h"
#include "SceneManager.h"


#include <imgui.h>



#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <tinyfiledialogs/tinyfiledialogs.h> 
 
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>





MyGUI::MyGUI(SDL_Window* window, void* context) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();
}

MyGUI::~MyGUI() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void MyGUI::ShowMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) 
    {
        if (ImGui::BeginMenu("File")) 
        {
            if (ImGui::BeginMenu("Import"))
            {
                if (ImGui::MenuItem("FBX")) 
                {
                    const char* filterPatterns[1] = { "*.fbx" };
                    const char* filePath = tinyfd_openFileDialog(
                        "Select an FBX file",
                        "",
                        1,
                        filterPatterns,
                        NULL,
                        0
                    );
                    if (filePath) 
                    {
                        SceneManager::LoadGameObject(filePath);
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Quit")) 
            {
                SDL_Quit();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                const char* url = "https://github.com/CITM-UPC/FreakyEngine_Group5";
                SDL_OpenURL(url);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void MyGUI::ShowHierarchy() 
{
    ImGui::SetNextWindowSize(ImVec2(300, 700), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
    if (ImGui::Begin("Hierarchy", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        // Iterar sobre todos los objetos en la escena
        for (auto& go : SceneManager::gameObjectsOnScene) 
        {
            if (SceneManager::gameObjectsOnScene.empty()) continue;

            static char newName[128] = "";
            static bool renaming = false;
            static GameObject* renamingObject = nullptr;

            // Selección y resaltado del objeto
            bool isSelected = (SceneManager::selectedObject == &go);
            if (ImGui::Selectable(go.getName().c_str(), isSelected))
            {
                SceneManager::selectedObject = &go;
            }

            // Iniciar renombrado al hacer doble clic
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) 
            {
                renaming = true;
                renamingObject = &go;
                strcpy_s(newName, go.getName().c_str());
            }

            // Lógica para renombrar el objeto seleccionado
            if (renaming && renamingObject == &go) 
            {
                ImGui::SetKeyboardFocusHere();
                if (ImGui::InputText("##rename", newName, IM_ARRAYSIZE(newName), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    go.setName(newName);
                    renaming = false;
                }
                if (ImGui::IsItemDeactivated() || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                    renaming = false;
                }
            }
        }
        ImGui::End();
    }
}
//ALGO FALLA , HAY QUE REVISARLO
void MyGUI::renderInspector() {
    ImGui::SetNextWindowSize(ImVec2(300, 700), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(980, 20), ImGuiCond_Always);
    ImGui::Begin("Inspector");

    // Obtener el GameObject actualmente seleccionado de forma persistente
     static GameObject* persistentSelectedObject = nullptr;

    // Actualizar persistentSelectedObject solo si hay un cambio en SceneManager::selectedObject
    if (SceneManager::selectedObject != nullptr) {
        persistentSelectedObject = SceneManager::selectedObject;
    }

    if (persistentSelectedObject) {
        // Mostrar información del componente Transform
        if (ImGui::CollapsingHeader("Transform")) {
            glm::vec3 position = persistentSelectedObject->transform().pos();
			glm::vec3 rotation = glm::vec3(persistentSelectedObject->transform().extractEulerAngles(persistentSelectedObject->transform().mat()));
			glm::vec3 scale = persistentSelectedObject->transform().extractScale(persistentSelectedObject->transform().mat());

            ImGui::Text("Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
            ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", rotation.x, rotation.y, rotation.z);
            ImGui::Text("Scale: (%.2f, %.2f, %.2f)", scale.x, scale.y, scale.z);
        }

        // Mostrar información del componente Mesh
        if (persistentSelectedObject->hasMesh() && ImGui::CollapsingHeader("Mesh")) {
            Mesh& mesh = persistentSelectedObject->mesh();
          //  ImGui::Text("Vertices: %d", mesh.vertices());

            static bool showNormalsPerTriangle = false;
            static bool showNormalsPerFace = false;

            ImGui::Checkbox("Show Normals (Per Triangle)", &showNormalsPerTriangle);
            ImGui::Checkbox("Show Normals (Per Face)", &showNormalsPerFace);
            if (showNormalsPerTriangle) {
				persistentSelectedObject->drawNormals();
            }
            if (showNormalsPerFace) {
                persistentSelectedObject->drawFaceNormals();
            }
        }

        // Mostrar información del componente Texture
        if (persistentSelectedObject->hasTexture() && ImGui::CollapsingHeader("Texture")) {
            Texture& texture = persistentSelectedObject->texture();
            // Aquí puedes incluir la lógica de textura comentada en tu código original
        }
    }
    else {
        ImGui::Text("No GameObject selected.");
    }

    ImGui::End();
}
void MyGUI::render() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ShowMainMenuBar();
    ShowHierarchy();
    renderInspector();

	//Show debug window hello world
	ImGui::Begin("Hello, world!");
	ImGui::Text("This is some useful text.");
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}


void MyGUI::handleEvent(const SDL_Event& event) {
	ImGui_ImplSDL2_ProcessEvent(&event);
}


void MyGUI::processEvent(const SDL_Event& event) {
	ImGui_ImplSDL2_ProcessEvent(&event);
}