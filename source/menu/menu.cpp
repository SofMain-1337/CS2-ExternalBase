#include "menu.h"

#include "../globals/settings.h"

#include "../../external/imgui/imgui_impl_dx9.h"
#include "../../external/imgui/imgui_impl_win32.h"
#include "../../external/imgui/imgui.h"
#include "../../gui.h"
#include "../../input.h"
#include "../../minwindef.h"
#include <dwmapi.h>
HWND my_wnd = NULL;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND window,
    UINT message,
    WPARAM wideParameter,
    LPARAM longParameter
);

LRESULT CALLBACK WindowProcess(
    HWND window,
    UINT message,
    WPARAM wideParameter,
    LPARAM longParameter)
{
    if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
        return true;

    switch (message)
    {

    case WM_SIZE: {
        if (gui::device && wideParameter != SIZE_MINIMIZED)
        {
            gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
            gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
            gui::ResetDevice();
        }
        return 0;
    }

    case WM_SYSCOMMAND: {
        if ((wideParameter & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    }

    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }

    case WM_LBUTTONDOWN: {
        gui::position = MAKEPOINTS(longParameter);
        return 0;
    }

    case WM_MOUSEMOVE: {
        if (wideParameter == MK_LBUTTON)
        {
            const auto points = MAKEPOINTS(longParameter);
            auto rect = RECT{};

            GetWindowRect(gui::window, &rect);

            rect.left += points.x - gui::position.x;
            rect.top += points.y - gui::position.y;

            if (gui::position.x >= 0 &&
                gui::position.x <= gui::WIDTH &&
                gui::position.y >= 0 && gui::position.y <= 19)
                SetWindowPos(
                    gui::window,
                    HWND_TOPMOST,
                    rect.left,
                    rect.top,
                    0, 0,
                    SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
                );
        }
        return 0;
    }
    }

    return DefWindowProc(window, message, wideParameter, longParameter);
}


void gui::CreateHWindow(const char* windowName) noexcept
{
    WNDCLASSEXA windowClass = { sizeof(WNDCLASSEXA), CS_CLASSDC, WindowProcess, 0L, 0L,
                                GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr,
                                "class001", nullptr };
    RegisterClassExA(&windowClass);

    window = CreateWindowExA(
        0,
        "class001",
        windowName,
        WS_POPUP,
        100,
        100,
        WIDTH,
        HEIGHT,
        0,
        0,
        windowClass.hInstance,
        0
    );
    
    ATOM rce = RegisterClassExA(&windowClass);
    RECT rect;
   GetWindowRect(GetDesktopWindow(), &rect);
    SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
   // my_wnd = gui::create_window_in_band(0, rce, L"SofMainCS2Base", WS_POPUP, rect.left, rect.top, rect.right, rect.bottom, 0, 0, windowClass.hInstance, 0, gui::ZBID_UIACCESS);
    ShowWindow(window, SW_SHOWDEFAULT);
    UpdateWindow(window);
   SetLayeredWindowAttributes(window, RGB(0, 0, 0), 255, LWA_ALPHA);
   // MARGINS margin = { -1 };
  //  DwmExtendFrameIntoClientArea(window, &margin);
  //  ShowWindow(window, SW_SHOW);
    //UpdateWindow(window);
}


void gui::DestroyHWindow() noexcept
{
    DestroyWindow(window);
    UnregisterClassA("class001", GetModuleHandle(nullptr));
}
bool gui::CreateDevice() noexcept
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3d)
        return false;

    ZeroMemory(&presentParameters, sizeof(presentParameters));

    presentParameters.Windowed = TRUE;
    presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    presentParameters.hDeviceWindow = my_wnd;
    presentParameters.EnableAutoDepthStencil = TRUE;
    presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
    presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        window,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &presentParameters,
        &device) < 0)
        return false;

    return true;
}

void gui::ResetDevice() noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();

    const auto result = device->Reset(&presentParameters);

    if (result == D3DERR_INVALIDCALL)
        IM_ASSERT(0);

    ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
    if (device)
    {
        device->Release();
        device = nullptr;
    }

    if (d3d)
    {
        d3d->Release();
        d3d = nullptr;
    }
}

void gui::CreateImGui() noexcept
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;
    ImGui::StyleColorsDark();

    const char* fontPath = "C:\\Windows\\Fonts\\segoeui.ttf";
    float fontSize = 18.0f;

    if (!io.Fonts->AddFontFromFileTTF(fontPath, fontSize)) {
       io.Fonts->AddFontDefault();
    }

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(device);
}



void gui::DestroyImGui() noexcept
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);

        if (message.message == WM_QUIT)
        {
            isRunning = !isRunning;
            return;
        }
    }

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    SetupImGuiStyle(); // Custom style function
    //SetupImGuiStyle(); // Style setup
  //  ImGui::Render(); // Finalize
}

void gui::EndRender() noexcept
{
    ImGui::EndFrame();

    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

    device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

    if (device->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }

    const auto result = device->Present(0, 0, 0, 0);

    if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        ResetDevice();
}

void gui::SetupImGuiStyle() noexcept {
    ImGuiStyle* style = &ImGui::GetStyle();

    // General Window
    style->WindowBorderSize = 1.0f;
    style->FrameBorderSize = 0.0f;
    style->WindowRounding = 0.0f;
    style->FrameRounding = 0.0f;
    style->WindowTitleAlign = ImVec2(0.5f, 0.5f);

    // Transparency
    style->Colors[ImGuiCol_WindowBg] = ImColor(30, 30, 30, 230);
    style->Colors[ImGuiCol_Border] = ImColor(10, 10, 10, 255);

    // Title Bar
    style->Colors[ImGuiCol_TitleBg] = ImColor(20, 20, 20, 255);
    style->Colors[ImGuiCol_TitleBgActive] = ImColor(20, 20, 20, 255);

    // Buttons
    style->Colors[ImGuiCol_Button] = ImColor(20, 20, 20, 255);
    style->Colors[ImGuiCol_ButtonHovered] = ImColor(40, 40, 40, 255);
    style->Colors[ImGuiCol_ButtonActive] = ImColor(50, 50, 50, 255);

    // Frame Background
    style->Colors[ImGuiCol_FrameBg] = ImColor(20, 20, 20, 255);
    style->Colors[ImGuiCol_FrameBgHovered] = ImColor(40, 40, 40, 255);
    style->Colors[ImGuiCol_FrameBgActive] = ImColor(50, 50, 50, 255);

    // Collapsing Header (Settings)
    style->Colors[ImGuiCol_Header] = ImColor(20, 20, 20, 255);
    style->Colors[ImGuiCol_HeaderHovered] = ImColor(40, 40, 40, 255);
    style->Colors[ImGuiCol_HeaderActive] = ImColor(50, 50, 50, 255);

    // Checkboxes and Sliders
    style->Colors[ImGuiCol_CheckMark] = ImColor(200, 200, 200, 255);
    style->Colors[ImGuiCol_SliderGrab] = ImColor(100, 100, 100, 255);
    style->Colors[ImGuiCol_SliderGrabActive] = ImColor(150, 150, 150, 255);

    // Separator Lines
    style->Colors[ImGuiCol_Separator] = ImColor(30, 30, 30, 255);
    style->Colors[ImGuiCol_SeparatorHovered] = ImColor(30, 30, 30, 255);
    style->Colors[ImGuiCol_SeparatorActive] = ImColor(30, 30, 30, 255);

    // Text
    style->Colors[ImGuiCol_Text] = ImColor(200, 200, 200, 255);


}

std::string GetKeyName(int vk) {
    UINT scanCode = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
    char keyName[128];
    int result = GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName));
    if (result > 0) {
        return std::string(keyName);
    }
    else {
        return "Unknown";
    }
}

void gui::Render() noexcept {
    static int currentTab = 0;
    ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(540, 295), ImGuiCond_Always);
    ImGui::Begin(
        "SofMain CS2 Base [Public/Free Build]",
        &globals::isRunning,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar
    );


    SetupImGuiStyle(); // Custom style function
   
    ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0, 0, 0, 255).Value);

    // Begin the Top Child (Button Panel)
    ImGui::BeginChild("##TopPanel", ImVec2(ImGui::GetContentRegionAvail().x, 50), true);
    {
        //ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 2.0f)); // Push custom padding for slimness
        if (ImGui::Button("LegitBot", ImVec2(150, 20))) currentTab = 0;
        ImGui::SameLine();
        if (ImGui::Button("Visual", ImVec2(150, 20))) currentTab = 1;
        ImGui::SameLine();
        if (ImGui::Button("Misc", ImVec2(150, 20))) currentTab = 2;
        
        ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    // Render the selected tab in the main area below
    ImGui::BeginChild("##MainPanel", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true);
    {
        if (currentTab == 0) {
            // Aimbiot Tab:
            ImGui::Text("LegitBot");
            ImGui::Separator();
            ImGui::Text("Aimbot");
            ImGui::SameLine();
            ImGui::Checkbox("##AimBotEnable", &globals::AimBot);
            ImGui::Text("TriggerBot");
            ImGui::SameLine();
            ImGui::Checkbox("##TriggerBotEnable", &globals::TriggerBot);

            if (globals::TriggerBot) {
                ImGui::Text("Key:");
                ImGui::SameLine();
                if (ImGui::Button(globals::TriggerBotKeyName)) {
                    ImGui::OpenPopup("Select Key");
                }

                if (ImGui::BeginPopup("Select Key")) {
                    ImGuiIO& io = ImGui::GetIO();
                    for (int i = ImGuiKey_NamedKey_BEGIN; i < ImGuiKey_NamedKey_END; i++) {
                        ImGuiKey key = static_cast<ImGuiKey>(i);
                        if (ImGui::IsKeyPressed(key)) {
                            globals::TriggerBotKey = key;
                            std::string keyName = GetKeyName(i);
                            snprintf(globals::TriggerBotKeyName, sizeof(globals::TriggerBotKeyName), "%s", keyName.c_str());
                            ImGui::CloseCurrentPopup();
                        }
                    }
                    ImGui::Text("Press a key to select.");
                    ImGui::EndPopup();
                }

                if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::PushItemWidth(150);
                    const char* modeItems[] = { "Hold", "Toggle" };
                    ImGui::Combo("Mode", &globals::TriggerBotMode, modeItems, IM_ARRAYSIZE(modeItems));
                    ImGui::PopItemWidth();

                    ImGui::SliderInt("Delay (ms)", &globals::TriggerBotDelay, 1, 1000);
                    ImGui::Checkbox("TeamCheck", &globals::TriggerBotTeamCheck);
                    ImGui::Checkbox("IgnoreFlash", &globals::TriggerBotIgnoreFlash);
                }
            }
        }
        else if (currentTab == 1) {
            // Visual Tab:
            ImGui::Text("Visual");
            ImGui::Separator();
            ImGui::SliderInt("FOV", &globals::FOV, 0, 160, "FOV: %d");
            ImGui::Checkbox("Glow", &globals::Glow);
            if (globals::Glow) {
                ImGui::ColorEdit4("Glow Color", (float*)&globals::GlowColor);
            }
            ImGui::Checkbox("NoFlash", &globals::NoFlashEnabled);
        }
        else if (currentTab == 2) {
            // Misc Tab:
            ImGui::Text("Misc");
            ImGui::Separator();
            ImGui::Text("Menu Color");
            ImGui::SameLine();
            if (ImGui::ColorEdit4("##AccentColor", (float*)&globals::MenuAccentColor, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions)) {
                globals::Rainbow = false;
            }
            ImGui::Checkbox("Rainbow", &globals::Rainbow);
            ImGui::Checkbox("BunnyHop", &globals::BunnyHopEnabled);

            if (globals::Rainbow) {
                static float hue = 0.0f;
                hue += ImGui::GetIO().DeltaTime * 0.1f;
                if (hue > 1.0f) hue = 0.0f;
                ImVec4 rainbowColor = ImColor::HSV(hue, 1.0f, 1.0f);
                globals::MenuAccentColor = rainbowColor;
            }
            //ImGui::SameLine(ImGui::GetContentRegionAvail().x - 110); // Align Exit button to the right
            if (ImGui::Button("Exit", ImVec2(196, 20))) {
                exit(0);
            }
        }
    }
    ImGui::EndChild();

    ImGui::PopStyleColor();
    ImGui::End();
}


void gui::ApplyCustomStyle() noexcept {
    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;
    ImVec4 accentColor = globals::MenuAccentColor;

    colors[ImGuiCol_Header] = accentColor;
    colors[ImGuiCol_HeaderHovered] = accentColor;
    colors[ImGuiCol_HeaderActive] = accentColor;
    colors[ImGuiCol_Button] = accentColor;
    colors[ImGuiCol_ButtonHovered] = accentColor;
    colors[ImGuiCol_ButtonActive] = accentColor;
    colors[ImGuiCol_Tab] = accentColor;
    colors[ImGuiCol_TabHovered] = accentColor;
    colors[ImGuiCol_TabActive] = accentColor;
    colors[ImGuiCol_TabUnfocused] = accentColor;
    colors[ImGuiCol_TabUnfocusedActive] = accentColor;
    colors[ImGuiCol_ScrollbarGrab] = accentColor;
    colors[ImGuiCol_ScrollbarGrabHovered] = accentColor;
    colors[ImGuiCol_ScrollbarGrabActive] = accentColor;
    colors[ImGuiCol_TitleBg] = accentColor;
    colors[ImGuiCol_TitleBgActive] = accentColor;
    colors[ImGuiCol_CheckMark] = accentColor;
    colors[ImGuiCol_SliderGrab] = accentColor;
    colors[ImGuiCol_SliderGrabActive] = accentColor;
}
