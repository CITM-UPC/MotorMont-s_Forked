#include "MyWindow.h"

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
    void ShowConsole();

    void ShowSpawnFigures(bool* p_open);
    void ShowMetricsWindow(bool* p_open);
    void ShowRenderSystemInfo(bool* p_open);
    void ShowLibraryVerions(bool* p_open);

    void ShowHierarchy();

private:
    void handleDeleteKey(); // Nueva funci�n para manejar la tecla "Suprimir"
};
