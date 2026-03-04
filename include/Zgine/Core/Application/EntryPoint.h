#pragma once

#include <Zgine/Core/Application/Application.h>
#include <Zgine/Core/Log/Log.h>

#ifdef ZGINE_PLATFORM_WINDOWS

extern Zgine::Application* Zgine::CreateApplication();

int main(int argc, char** argv)
{
    (void)argc; (void)argv; // Suppress unused parameter warnings
    Zgine::Log::Init();

    ZGINE_CORE_INFO("Zgine Engine Initialized");

    auto app = Zgine::CreateApplication();
    app->Run();
    delete app;

    // Shutdown logging system (flushes all pending logs)
    Zgine::Log::Shutdown();

    return 0;
}

#endif
