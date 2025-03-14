#include "Time.h"

#include "pch.h"

Time* m_TimeInst = nullptr;

Time::Time() : m_CurrentTime(0.0), m_LastTime(0.0), m_DeltaTime(0.0), m_FPS(0.0) {}

void Time::Init() { m_TimeInst = new Time(); }

void Time::Shutdown() {
    delete m_TimeInst;
    m_TimeInst = nullptr;
}

Time& Time::Get() {
    assert(m_TimeInst != nullptr);
    return *m_TimeInst;
}

void Time::Update() {
    m_CurrentTime = glfwGetTime();

    if (m_LastTime == 0) {
        m_LastTime = m_CurrentTime;
    }

    m_DeltaTime = m_CurrentTime - m_LastTime;
    m_FPS = 1.0 / m_DeltaTime;

    m_LastTime = m_CurrentTime;
}
