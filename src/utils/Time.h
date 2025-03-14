#ifndef __TIME_H__
#define __TIME_H__

#include "gfx/GraphicContext.h"

class Time {
   private:
    Time();
    double m_CurrentTime;  // Actual time
    double m_LastTime;     // Time since last call
    double m_DeltaTime;    // Elapsed time between two frames in seconds
    double m_FPS;

   public:
    // Call this method at the start of each frame
    static void Init();
    static void Shutdown();

    static Time& Get();

    void Update();

    /* Getters */
    double GetCurrentTime() { return glfwGetTime(); }  // Get current time in second
    double GetDeltaTime() { return m_DeltaTime; }      // Return delta time in second
    double GetFPS() { return m_FPS; }                  // Return the number of frame per second
};

#endif  // __TIME_H__
