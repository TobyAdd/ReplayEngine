#include <Windows.h>
#include <cocos2d.h>
#include <gl/GL.h>
#include <functional>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_win32.h"

using namespace cocos2d;

namespace ImGuiHook {
    extern bool blockMetaInput;
    void Load(std::function<void(void*, void*, void**)> hookFunc);
    void Unload();
    void setRenderFunction(std::function<void()> func);
    void setToggleFunction(std::function<void()> func);
}

