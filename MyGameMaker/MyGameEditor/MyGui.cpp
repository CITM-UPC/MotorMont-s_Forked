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

void MyGUI::render() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::BeginMenu("Import")) {
                if (ImGui::MenuItem("FBX")) {
                    const char* filterPatterns[1] = { "*.fbx" };
                    const char* filePath = tinyfd_openFileDialog(
                        "Select an FBX file",
                        "",
                        1,
                        filterPatterns,
                        NULL,
                        0
                    );
                    if (filePath) {
                        SceneManager::LoadGameObject(filePath);
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Quit")) {
                SDL_Quit();
            }
            ImGui::EndMenu();

        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                
                const char* url = "https://github.com/CITM-UPC/FreakyEngine_Group5";
                SDL_OpenURL(url);
            }

            ImGui::EndMenu(); 
        }
        ImGui::EndMainMenuBar(); 
    }

    ImGui::SetNextWindowSize(ImVec2(300, 700), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
    if (ImGui::Begin("Hierarchy", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        for (auto& go : SceneManager::gameObjectsOnScene) {
			if (SceneManager::gameObjectsOnScene.size() <1) continue;
            static char newName[128] = "";
            static bool renaming = false;
            static GameObject* renamingObject = nullptr;

            // Flags para el nodo, incluyendo selección y expansión
            ImGuiTreeNodeFlags nodeFlags = (SceneManager::selectedObject == &go ? ImGuiTreeNodeFlags_Selected : 0) |
                ImGuiTreeNodeFlags_OpenOnArrow |
                ImGuiTreeNodeFlags_SpanAvailWidth;
            bool nodeOpen = ImGui::TreeNodeEx(go.getName().c_str(), nodeFlags);

            // Lógica para renombrar
            if (renaming && renamingObject == &go) {
                ImGui::SetKeyboardFocusHere();
                if (ImGui::InputText("##rename", newName, IM_ARRAYSIZE(newName), ImGuiInputTextFlags_EnterReturnsTrue)) {
                    go.setName(newName);
                    renaming = false;
                }
                if (ImGui::IsItemDeactivated() || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                    renaming = false;
                }
            }
            else {
                if (SceneManager::selectedObject != nullptr) {
                    if (ImGui::Selectable(go.getName().c_str(), SceneManager::selectedObject == &go)) {
                        SceneManager::selectedObject = &go;
                    }
                }
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                    renaming = true;
                    renamingObject = &go;
                    strcpy_s(newName, go.getName().c_str());
                }
            }

            // Si el nodo está abierto, podemos agregar sub-elementos aquí
            if (nodeOpen) {
                // Lógica para sub-elementos (si los tienes, de otro modo se puede omitir)
                // Ejemplo:
                // for (auto& child : go.getChildren()) {
                //     ImGui::Text(child.getName().c_str());
                // }

                ImGui::TreePop();
            }
        }
    }
    ImGui::End();

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