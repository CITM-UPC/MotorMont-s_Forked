#include "MyGUI.h"
#include "MyGameEngine/GameObject.h"
#include "SceneManager.h"
#include "BasicShapesManager.h"
#include "SystemInfo.h"
#include "Console.h"
#include <memory>
#include <filesystem>


#include <imgui.h>


#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <tinyfiledialogs/tinyfiledialogs.h> 
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>

#include <windows.h>
#include <psapi.h>

bool show_metrics_window = false;

bool show_hardware_window = false;

bool show_software_window = false;

bool show_spawn_figures_window = false;

MyGUI::MyGUI(SDL_Window* window, void* context) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Button] = ImVec4(0.0f, 0.8f, 0.0f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.9f, 0.1f, 1.0f);   
    colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.7f, 0.0f, 1.0f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.8f);       
    colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.7f, 0.0f, 1.0f);       
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.9f, 0.1f, 1.0f);  
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.6f, 0.0f, 1.0f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.0f, 0.8f, 0.0f, 0.9f);
	colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    colors[ImGuiCol_Header] = ImVec4(0.0f, 0.8f, 0.0f, 1.0f);           
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.9f, 0.0f, 1.0f);    
    colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);     

    colors[ImGuiCol_Border] = ImVec4(0.0f, 0.7f, 0.0f, 1.0f);          
    colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.5f, 0.0f, 0.5f);    

    colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); 
    colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.7f, 0.0f, 1.0f);     

    colors[ImGuiCol_SliderGrab] = ImVec4(0.0f, 0.9f, 0.0f, 1.0f);       
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); 

    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();
}

MyGUI::~MyGUI() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void MyGUI::ShowMainMenuBar() {
    if (show_metrics_window) {
        ShowMetricsWindow(&show_metrics_window);
    }
    if (show_hardware_window) {
        ShowRenderSystemInfo(&show_hardware_window);
    }
    if (show_software_window) {
        ShowLibraryVerions(&show_software_window);
    }
    if (show_spawn_figures_window) {
        ShowSpawnFigures(&show_spawn_figures_window);
    }

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Quit")) {
                SDL_Quit();
                exit(0);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Mesh")) {
            ImGui::Checkbox("Mesh Creator", &show_spawn_figures_window);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scene")) {
            if (ImGui::MenuItem("Save Scene")) {
                const char* filterPatterns[] = { "*.scene" };
                const char* filePath = tinyfd_saveFileDialog("Save Scene", "scene_output.scene", 1, filterPatterns, nullptr);
                if (filePath) {
                    SceneManager::saveScene(filePath);
                }
            }
            if (ImGui::MenuItem("Load Scene")) {
                const char* filterPatterns[] = { "*.scene" };
                const char* filePath = tinyfd_openFileDialog("Load Scene", "", 1, filterPatterns, nullptr, 0);
                if (filePath) {
                    SceneManager::loadScene(filePath);
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                const char* url = "https://github.com/CITM-UPC/MotorMont-s_Forked";
                SDL_OpenURL(url);
            }

            if (ImGui::BeginMenu("View")) {
                if (ImGui::RadioButton("Console", !show_assets_window)) {
                    show_assets_window = false; // Show Console
                }
                if (ImGui::RadioButton("Assets", show_assets_window)) {
                    show_assets_window = true; // Show Assets
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void MyGUI::ConsoleWindow() {
    ImGui::SetNextWindowSize(ImVec2(680, 200), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(300, ImGui::GetIO().DisplaySize.y - 200), ImGuiCond_Always);

    ImGui::Begin("Console");

    if (ImGui::Button("Clear")) {
        Console::Instance().Clear();
    }

    const auto& messages = Console::Instance().GetMessages();
    ImGui::Text("Message Count: %zu", messages.size());
    for (const auto& message : messages) {
        ImGui::Text("%s", message.c_str());
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::End();
}

void MyGUI::ShowAssetsWindow() {
    ImGui::SetNextWindowSize(ImVec2(680, 200), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(300, ImGui::GetIO().DisplaySize.y - 200), ImGuiCond_Always);

    ImGui::Begin("Assets");

    static std::string baseDirectory = "Assets";

    // Check if the directory exists
    if (!std::filesystem::exists(baseDirectory)) {
        ImGui::Text("Error: Assets directory not found at %s", baseDirectory.c_str());
        ImGui::End();
        return;
    }

    // Safely iterate through the directory
    try {
        for (const auto& entry : std::filesystem::directory_iterator(baseDirectory)) {
            if (entry.is_directory()) {
                ImGui::Text("[Folder] %s", entry.path().filename().string().c_str());
            }
            else if (entry.is_regular_file()) {
                std::string extension = entry.path().extension().string();
                ImGui::Text("[File] %s (%s)", entry.path().filename().string().c_str(), extension.c_str());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        ImGui::Text("Error: Unable to access assets directory.");
        ImGui::Text("%s", e.what());
    }

    ImGui::End();
}




void MyGUI::ShowSpawnFigures(bool* p_open) {

    ImGui::Begin("Spawn Figures");

    if (ImGui::Button("Spawn Triangle")) {
        BasicShapesManager::createFigure(1, SceneManager::gameObjectsOnScene, 1.0, vec3(0.0f,0.0f,0.0f) );
        SceneManager::selectedObject = &SceneManager::gameObjectsOnScene.back();
    }

    if (ImGui::Button("Spawn Square")) {
        BasicShapesManager::createFigure(2, SceneManager::gameObjectsOnScene, 1.0, vec3(0.0f, 0.0f, 0.0f));
        SceneManager::selectedObject = &SceneManager::gameObjectsOnScene.back();
    }

    if (ImGui::Button("Spawn Cube")) {
        BasicShapesManager::createFigure(3, SceneManager::gameObjectsOnScene, 1.0, vec3(0.0f, 0.0f, 0.0f));
        SceneManager::selectedObject = &SceneManager::gameObjectsOnScene.back();
    }
    ImGui::End();
}

float GetMemoryUsage() {
    PROCESS_MEMORY_COUNTERS memCounter;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter))) {
        return static_cast<float>(memCounter.WorkingSetSize) / (1024.0f * 1024.0f);
    }
    return 0.0f;
}

void MyGUI::ShowMetricsWindow(bool* p_open) 
{
    static std::vector<float> fpsHistory;
    static std::vector<float> memoryHistory;
    static const int maxSamples = 100;  

    float fps = ImGui::GetIO().Framerate;
    fpsHistory.push_back(fps);
    if (fpsHistory.size() > maxSamples) {
        fpsHistory.erase(fpsHistory.begin());
    }

    float memoryUsage = GetMemoryUsage();
    memoryHistory.push_back(memoryUsage);
    if (memoryHistory.size() > maxSamples) {
        memoryHistory.erase(memoryHistory.begin());
    }

    ImGui::Begin("Performance Graphs");

    
    ImGui::Text("FPS Graph");
    ImGui::PlotLines("FPS", fpsHistory.data(), fpsHistory.size(), 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));
    ImGui::Text("Current FPS: %.1f", fps);

    ImGui::Separator();  

    ImGui::Text("Memory Usage Graph");
    ImGui::PlotLines("Memory (MB)", memoryHistory.data(), memoryHistory.size(), 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));
    ImGui::Text("Current Memory Usage: %.1f MB", memoryUsage);

    ImGui::End();

}

void MyGUI::ShowRenderSystemInfo(bool* p_open)
{
    ImGui::Begin("Hardware Information");
    std::string systemInfo = SystemInfo::GetFullSystemInfo();
    ImGui::TextWrapped("%s", systemInfo.c_str());
    ImGui::End();
}

void MyGUI::ShowLibraryVerions(bool* p_open)
{
    ImGui::Begin("Software Information");
    std::string libraryInfo = SystemInfo::GetFullLibraryVerions();
    ImGui::TextWrapped("%s", libraryInfo.c_str());
    ImGui::End();
}

void MyGUI::ShowHierarchy()
{
    ImGui::SetNextWindowSize(ImVec2(300, 700), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
    if (ImGui::Begin("Hierarchy", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {

        for (auto& go : SceneManager::gameObjectsOnScene)
        {
            if (SceneManager::gameObjectsOnScene.empty()) continue;

            static char newName[128] = "";
            static bool renaming = false;
            static std::shared_ptr<GameObject> renamingObject = nullptr;

            bool isSelected = (SceneManager::selectedObject == &go);
            if (ImGui::Selectable(go.getName().c_str(), isSelected))
            {
                SceneManager::selectedObject = &go;
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                renaming = true;
                renamingObject = std::make_shared<GameObject>(go);
                strcpy_s(newName, go.getName().c_str());
            }

            if (renaming && renamingObject == std::make_shared<GameObject>(go))
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

            // Funcionalidad de arrastrar y soltar
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                GameObject* goPtr = &go; // Aseg�rate de que est�s pasando un puntero v�lido
                ImGui::SetDragDropPayload("GAMEOBJECT", &goPtr, sizeof(GameObject*)); // El payload debe contener el puntero
                ImGui::Text("Dragging %s", go.getName().c_str());
                ImGui::EndDragDropSource();
            }


            // Hacer que este objeto sea un "drop target"
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
                    IM_ASSERT(payload->DataSize == sizeof(GameObject*)); // Verifica el tama�o del payload
                    GameObject* draggedObject = *(GameObject**)payload->Data;

                    if (draggedObject == nullptr) {
                        std::cout << "Error: Dragged object is null!" << std::endl;
                    }
                    else {
                        std::cout << "Dragged object: " << draggedObject->getName() << std::endl;
                        // Aqu� se maneja el emparentamiento
                        if (draggedObject != &go) {
                            draggedObject->setParent(&go);
                            go.addChild(draggedObject);
                            (std::remove(SceneManager::gameObjectsOnScene.begin(), SceneManager::gameObjectsOnScene.end(), *draggedObject));
                        }
                    }
                }

                ImGui::EndDragDropTarget();
            }

            // A�adir un desplazamiento visual para mostrar jerarqu�as
            if (go.hasChildren()) {
                ImGui::Indent();
                for (auto& child : go.getChildren()) {
                    ImGui::Text("  %s", child->getName().c_str());
                }
                ImGui::Unindent();
            }
        }

        ImGui::End();
    }
}




void MyGUI::renderInspector() {
    ImGui::SetNextWindowSize(ImVec2(300, 700), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(980, 20), ImGuiCond_Always);
    ImGui::Begin("Inspector");

    static GameObject* persistentSelectedObject = nullptr;

    if (SceneManager::selectedObject != nullptr) {
        persistentSelectedObject = SceneManager::selectedObject;
    }

    if (persistentSelectedObject) {
        ImGui::Text("ID: %d", persistentSelectedObject->getUUID());

        if (ImGui::CollapsingHeader("Transform")) {
            Transform& transform = persistentSelectedObject->GetComponent<TransformComponent>()->transform();

            glm::vec3 position = transform.pos();
            glm::vec3 rotation = glm::vec3(transform.extractEulerAngles(transform.mat()));
            glm::vec3 scale = transform.extractScale(transform.mat());

            // Editable Position
            if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f, -1000.0f, 1000.0f, "%.3f", ImGuiSliderFlags_None)) {
                transform.pos() = position;
            }

            // Editable Rotation
            static glm::vec3 inputRotation = glm::vec3(0.0f);
            if (ImGui::DragFloat3("Rotation", glm::value_ptr(inputRotation), 0.1f, -360.0f, 360.0f, "%.3f", ImGuiSliderFlags_None)) {
                transform.setRotation(glm::radians(inputRotation.x), glm::radians(inputRotation.y), glm::radians(inputRotation.z));
            }

            // Editable Scale
            if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f, 0.001f, 10.0f, "%.3f", ImGuiSliderFlags_None)) {
                transform.setScale(scale);
            }


        }

        if (persistentSelectedObject->hasMesh() && ImGui::CollapsingHeader("Mesh")) {
            Mesh& mesh = persistentSelectedObject->mesh();

            static bool showNormalsPerTriangle = false;
            static bool showNormalsPerFace = false;

            ImGui::Checkbox("Show Normals (Per Triangle)", &showNormalsPerTriangle);
            ImGui::Checkbox("Show Normals (Per Face)", &showNormalsPerFace);
            if (showNormalsPerTriangle) {
                persistentSelectedObject->mesh().drawNormals(persistentSelectedObject->GetComponent<TransformComponent>()->transform().mat());
            }
            if (showNormalsPerFace) {
                persistentSelectedObject->mesh().drawNormalsPerFace(persistentSelectedObject->GetComponent<TransformComponent>()->transform().mat());
            }
        }

        if (persistentSelectedObject->hasTexture() && ImGui::CollapsingHeader("Texture")) {
            Texture& texture = persistentSelectedObject->texture();
            static bool showCheckerTexture = false;
            ImGui::Text("Width: %d", texture.image().width());
            ImGui::Text("Height: %d", texture.image().height());

            if (ImGui::Button("Toggle Checker Texture")) {
                showCheckerTexture = !showCheckerTexture;
                persistentSelectedObject->hasCheckerTexture = showCheckerTexture;
            }
        }
    }
    else {
        ImGui::Text("No GameObject selected.");
    }


    ImGui::End();
}

void MyGUI::processEvent(const SDL_Event& event) {
    ImGui_ImplSDL2_ProcessEvent(&event);

    handleEvent(event);
}

void MyGUI::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DELETE) {
        handleDeleteKey();

        ImGui_ImplSDL2_ProcessEvent(&event);
    }
    
}

void MyGUI::handleDeleteKey() {
    SceneManager::deleteSelectedObject();
}


void MyGUI::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ShowMainMenuBar();
    ShowHierarchy();
    renderInspector();

    if (show_assets_window) {
        ShowAssetsWindow();
    }
    else {
        ConsoleWindow();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


