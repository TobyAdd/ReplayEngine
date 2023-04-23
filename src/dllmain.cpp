#include "pch.h"
#include <imgui-hook.hpp>
#include <imgui.h>
#include "gui.h"
#include "hooks.h"
#include "framerate.h"
#include "hacks.h"

bool show = true;
bool noclipEnabled = false;

void CheckDir(string dir)
{
    if (!std::filesystem::is_directory(dir) || !std::filesystem::exists(dir))
    {
        std::filesystem::create_directory(dir);
    }
}

DWORD WINAPI ThreadMain(void *hModule)
{
    Console::Init();
    CheckDir("ReplayEngine");
    CheckDir("ReplayEngine/Replays");
    CheckDir("ReplayEngine/Videos");
    CheckDir("ReplayEngine/Clicks");
    CheckDir("ReplayEngine/Converter");
    hacks::anticheat_bypass_f(true);
    ImGuiHook::setRenderFunction(gui::Render);
    ImGuiHook::setToggleCallback([]()
                                 { gui::Toggle(); });
    if (MH_Initialize() == MH_OK)
    {
        ImGuiHook::setupHooks([](void *target, void *hook, void **trampoline)
                              { MH_CreateHook(target, hook, trampoline); });

        hooks::initHooks();
        framerate::initHooks();
        MH_EnableHook(MH_ALL_HOOKS);
    }
    else
    {
        FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(hModule), 0);
    }
    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        CreateThread(0, 0x1000, ThreadMain, hModule, 0, 0);
    }
    return TRUE;
}