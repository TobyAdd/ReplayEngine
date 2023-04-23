#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cocos2d.h>
#include "imgui-hook.hpp"
#include "../../src/data.h"

using namespace cocos2d;

void ApplyColor() {
    auto* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.15f, 0.15f, 0.22f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.15f, 0.15f, 0.22f, 1.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.15f, 0.15f, 0.22f, 1.00f);
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.31f, 0.29f, 0.39f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.36f, 0.35f, 0.47f, 0.54f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.45f, 0.43f, 0.56f, 0.54f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.22f, 0.21f, 0.30f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.22f, 0.21f, 0.30f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.22f, 0.21f, 0.30f, 1.00f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.22f, 0.21f, 0.30f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.20f, 0.20f, 0.27f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.29f, 0.39f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.39f, 0.37f, 0.49f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.46f, 0.44f, 0.58f, 0.54f);
    colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    colors[ImGuiCol_Button]                 = ImVec4(0.31f, 0.29f, 0.39f, 0.54f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.36f, 0.35f, 0.47f, 0.54f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.45f, 0.43f, 0.56f, 0.54f);
    colors[ImGuiCol_Header]                 = ImVec4(0.31f, 0.29f, 0.39f, 0.54f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.36f, 0.35f, 0.47f, 0.54f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.45f, 0.43f, 0.56f, 0.54f);
    colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.31f, 0.29f, 0.39f, 0.54f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.36f, 0.35f, 0.47f, 0.54f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.31f, 0.29f, 0.39f, 0.54f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.36f, 0.35f, 0.47f, 0.54f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.22f, 0.21f, 0.30f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.17f, 0.16f, 0.23f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.22f, 0.21f, 0.30f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void ApplyStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0;
}

bool ImGuiHook::blockMetaInput = true;

void _stub() {}
std::function<void()> g_drawFunc = _stub;
std::function<void()> ImGuiHook::g_toggleCallback = _stub;

void ImGuiHook::setRenderFunction(std::function<void()> func) {
    g_drawFunc = func;
}

void ImGuiHook::setToggleCallback(std::function<void()> func) {
    g_toggleCallback = func;
}

bool g_inited = false;

void (__thiscall* CCEGLView_swapBuffers)(CCEGLView*);
void __fastcall CCEGLView_swapBuffers_H(CCEGLView* self) {
    auto window = self->getWindow();

    if (!g_inited) {
        g_inited = true;
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromMemoryTTF(fontData, sizeof(fontData), 14.f);
        io.IniFilename = NULL;
        ApplyColor();
        ApplyStyle();
        auto hwnd = WindowFromDC(*reinterpret_cast<HDC*>(reinterpret_cast<uintptr_t>(window) + 0x244));
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplOpenGL3_Init();
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    g_drawFunc();

    ImGui::EndFrame();
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    glFlush();

    CCEGLView_swapBuffers(self);
}


void ImGuiHook::setupHooks(std::function<void(void*, void*, void**)> hookFunc) {
    auto cocosBase = GetModuleHandleA("libcocos2d.dll");
    hookFunc(
        GetProcAddress(cocosBase, "?swapBuffers@CCEGLView@cocos2d@@UAEXXZ"),
        CCEGLView_swapBuffers_H,
        reinterpret_cast<void**>(&CCEGLView_swapBuffers)
    );
}