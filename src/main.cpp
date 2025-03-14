#include "core/Application.h"
#include "utils/Logger.h"

int main(void) {
    auto app = Application::Create();

    Logger::Init();  // Initialize the logger

    app->Init();
    app->Run();
    app->Close();

    delete app;

    Logger::Shutdown();  // Destroy thhe logger

    return 0;
}
