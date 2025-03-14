#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <glm/glm.hpp>
#include <memory>

struct GLFWwindow;
class GraphicContext;
class Renderer;
class Event;

struct WindowProps {
    int width;
    int height;
    bool vsync;

    WindowProps() : width(1280), height(720), vsync(true) {}
};

class Window {
   public:
    enum Position { Center = 0, TopLeft, TopRight, BottomLeft, BottomRight };

    Window();

    void Init();
    void Clear();
    void Update();
    void SwapBuffers();
    void Shutdown();

    const bool ShouldClose() const;
    void Close();

    void OnEvent(const Event& event);

    /* Getters */
    GLFWwindow* GetHandler() const { return m_Handler; }
    WindowProps* GetProps() const { return m_Props.get(); }
    Renderer* GetRenderer() { return m_Renderer.get(); }
    glm::ivec2 GetCornerPosition(const Position position) const;

    /* Setters */
    void SetClearColor(const float r, const float g, const float b, const float a);
    void CaptureMouse(bool capture);
    void SetMousePosition(glm::dvec2 position);
    void SetVsync(bool enable);

    static std::unique_ptr<Window> Create();

   private:
    std::unique_ptr<WindowProps> m_Props;
    GLFWwindow* m_Handler;
    std::unique_ptr<GraphicContext> m_GraphicContext;
    std::unique_ptr<Renderer> m_Renderer;

    /* Callbacks */
    static void _error_callback(int error, const char* description);
    static void _framebuffer_size_callback(GLFWwindow* window, const int width, const int height);
    static void _key_callback(GLFWwindow* window, const int key, int scancode, const int action, int mods);
    static void _mouse_cursor_pos_callback(GLFWwindow* window, const double xpos, const double ypos);
    static void _mouse_button_callback(GLFWwindow* window, const int button, const int action, int mods);
    static void _window_pos_callback(GLFWwindow* window, const int xpos, const int ypos);
};

#endif  // __WINDOW_H__