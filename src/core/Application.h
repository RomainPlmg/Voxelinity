#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <glm/glm.hpp>
#include <memory>

class Window;
class World;
class Renderer;
class UIManager;
class Event;

struct GLFWwindow;

struct AppStatus {
    float FPS;
    int drawcalls;
    int nbTrianglesToRender;
    bool vsync;
    glm::ivec2 resolution;

    AppStatus() : FPS(0), drawcalls(0), nbTrianglesToRender(0), vsync(true), resolution(glm::ivec2(0, 0)) {}
};

class Application {
   public:
    Application();
    ~Application();

    void Init();
    void Run();
    void Close();

    void OnEvent(const Event& event);

    /* Getters */
    Window* GetWindow() { return m_Window.get(); }
    World* GetWorld() { return m_World.get(); }
    static const AppStatus& GetStatus() { return m_AppStatus; }

    static Application* Create();

   private:
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<World> m_World;
    std::unique_ptr<UIManager> m_UIManager;

    Renderer* m_Renderer;

    static AppStatus m_AppStatus;
};

#endif  // __APPLICATION_H__
