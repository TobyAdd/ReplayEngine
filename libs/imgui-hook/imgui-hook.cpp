#include "imgui-hook.hpp"
#include "imgui_theme.hpp"
#include "font.hpp"
#include <functional>
#include <chrono>

WNDPROC originalWndProc;
bool isInitialized = false;
bool ImGuiHook::blockMetaInput = true;
std::function<void()> drawFunction = []() {};
std::function<void()> toggleFunction = []() {};
HWND hWnd;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HookedWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::chrono::steady_clock::time_point animationStartTime;
bool isAnimating = false;
bool isFadingIn = false;

void animateAlpha()
{
    ImGuiStyle& style = ImGui::GetStyle();

    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> diff = currentTime - animationStartTime;
    float elapsed = diff.count();

    if (elapsed >= 0.10f)
    {
        style.Alpha = isFadingIn ? 1.0f : 0.0f;
        isAnimating = false;

        if (!isFadingIn)
        {
            toggleFunction();
        }

        return;
    }

    float t = elapsed / 0.10f;
    float alpha = isFadingIn ? 0.0f + t * (1.0f - 0.0f) : 1.0f - t * (1.0f - 0.0f);
    style.Alpha = alpha;
}

void(__thiscall* CCEGLView_swapBuffers)(CCEGLView*);
void __fastcall CCEGLView_swapBuffers_H(CCEGLView* self)
{
    if (!isInitialized)
    {
        isInitialized = true;
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromMemoryTTF(fontData, sizeof(fontData), 14.f);
        io.IniFilename = nullptr;
        ApplyColor();
        ApplyStyle();
        hWnd = WindowFromDC(*reinterpret_cast<HDC*>(reinterpret_cast<uintptr_t>(self->getWindow()) + 0x244));
        originalWndProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)HookedWndProc);
        ImGui_ImplWin32_Init(hWnd);
        ImGui_ImplOpenGL3_Init();
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (isAnimating)
    {
        animateAlpha();
    }

    drawFunction();

    ImGui::EndFrame();
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glFlush();

    CCEGLView_swapBuffers(self);
}

LRESULT CALLBACK HookedWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

    if (msg == WM_KEYDOWN && wParam == 'K' && !ImGui::GetIO().WantCaptureKeyboard)
    {
        if (!isAnimating)
        {
            isAnimating = true;
            isFadingIn = !isFadingIn;
            animationStartTime = std::chrono::steady_clock::now();

            if (isFadingIn)
            {
                toggleFunction();
            }
        }
    }

    if (ImGui::GetIO().WantCaptureMouse && ImGuiHook::blockMetaInput)
    {
        if (msg == WM_MOUSEMOVE || msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_MOUSEWHEEL)
            return 0;
    }

    if (ImGui::GetIO().WantCaptureKeyboard)
    {
        if (msg == WM_KEYDOWN || msg == WM_KEYUP || msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP ||
            msg == WM_HOTKEY || msg == WM_KILLFOCUS || msg == WM_SETFOCUS)
            return 0;
    }

    return CallWindowProc(originalWndProc, hWnd, msg, wParam, lParam);
}

void(__thiscall* CCEGLView_toggleFullScreen)(void*, bool);
void __fastcall CCEGLView_toggleFullScreen_H(void* self, void*, bool toggle)
{
    ImGuiHook::Unload();
    CCEGLView_toggleFullScreen(self, toggle);
}

void ImGuiHook::Load(std::function<void(void*, void*, void**)> hookFunc)
{
    auto cocosBase = GetModuleHandleA("libcocos2d.dll");
    hookFunc(GetProcAddress(cocosBase, "?swapBuffers@CCEGLView@cocos2d@@UAEXXZ"), CCEGLView_swapBuffers_H, reinterpret_cast<void**>(&CCEGLView_swapBuffers));
    hookFunc(GetProcAddress(cocosBase, "?toggleFullScreen@CCEGLView@cocos2d@@QAEX_N@Z"), CCEGLView_toggleFullScreen_H, reinterpret_cast<void**>(&CCEGLView_toggleFullScreen));
}

void ImGuiHook::Unload()
{
    isInitialized = false;
    SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)originalWndProc);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();    
}


void ImGuiHook::setRenderFunction(std::function<void()> func)
{
    drawFunction = func;
}

void ImGuiHook::setToggleFunction(std::function<void()> func)
{
    toggleFunction = func;
}
