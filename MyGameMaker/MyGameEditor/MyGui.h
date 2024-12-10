#include "MyWindow.h"
#include <memory>
#include <string>

class GameObject;

class MyGUI : public IEventProcessor
{
public:
    MyGUI(SDL_Window* window, void* context);
    MyGUI(MyGUI&&) noexcept = delete;
    MyGUI(const MyGUI&) = delete;
    MyGUI& operator=(const MyGUI&) = delete;
    ~MyGUI();
    void render();
    void renderInspector();
    void processEvent(const SDL_Event& event) override;
    void handleEvent(const SDL_Event& event);

    void ShowMainMenuBar();
    void ConsoleWindow();

    void ShowSpawnFigures(bool* p_open);
    void ShowMetricsWindow(bool* p_open);
    void ShowRenderSystemInfo(bool* p_open);
    void ShowLibraryVerions(bool* p_open);

    void ShowHierarchy();

    void ShowAssetsWindow();

private:
    void handleDeleteKey(); // Nueva función para manejar la tecla "Suprimir"
    GameObject* draggedObject = nullptr; // Objeto arrastrado

    bool show_assets_window = false;
};
