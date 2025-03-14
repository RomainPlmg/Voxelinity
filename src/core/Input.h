#ifndef __INPUT_H__
#define __INPUT_H__

struct GLFWwindow;
class Window;

#include <glm/glm.hpp>
#include <memory>

class Input {
   public:
    static void Init(const std::unique_ptr<Window>& window);
    static bool IsKeyPressed(int keycode);
    static bool IsMouseButtonPressed(int button);
    static glm::dvec2 GetMousePosition();

   private:
    static GLFWwindow* m_Handler;
};

#endif  // __INPUT_H__
