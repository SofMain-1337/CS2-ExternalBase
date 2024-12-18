#include "menu/menu.h"
#include "memory/memory.h"
#include "globals/settings.h"
#include "threads/threads.h"
#include "offsets/offsets.h"

#include <thread>

int __stdcall wWinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    PWSTR arguments,
    int commandShow) {

    if (!offsets::UpdateOffset())
        return EXIT_FAILURE;

    const auto memory = Memory("cs2.exe");

    globals::client = memory.GetModuleAddress("client.dll");

    std::thread(threads::RunMiscThread, std::ref(memory)).detach();
    std::thread(threads::RunVisualThread, std::ref(memory)).detach();
    std::thread(threads::RunAimThread, std::ref(memory)).detach();

    gui::CreateHWindow("SofMain CS2 Base [Public/Free Build]");
    gui::CreateDevice();
    gui::CreateImGui();

    bool windowVisible = true;

    while (globals::isRunning) {
        if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
            windowVisible = !windowVisible;
            ShowWindow(gui::window, windowVisible ? SW_SHOW : SW_HIDE);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        if (windowVisible) {
            gui::BeginRender();
            gui::Render();
            gui::EndRender();
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    gui::DestroyImGui();
    gui::DestroyDevice();
    gui::DestroyHWindow();

    return EXIT_SUCCESS;
}
