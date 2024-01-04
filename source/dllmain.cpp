#include "include.hpp"
#include <imgui-hook.hpp>
#include <imgui.h>
#include "gui.hpp"
#include "hooks.hpp"
#include "hacks.hpp"

DWORD WINAPI ThreadMain(LPVOID lpParam)
{
    // Console::Init();
    ImGuiHook::setRenderFunction(gui::Render);
    gui::Toggle();
    ImGuiHook::setKeyPressHandler([](int keyCode) 
        {
            switch (keyCode)
            {
            case 16:
                gui::Toggle();
                break;
            }

            if (gui::recording == true)
                gui::currentkeycode = keyCode;
        });
    if (MH_Initialize() == MH_OK)
    {
        ImGuiHook::Load([](void *target, void *hook, void **trampoline)
                            { MH_CreateHook(target, hook, trampoline); });
        hooks::init();
        hacks::load();
        MH_EnableHook(MH_ALL_HOOKS);
        hacks::inject_extensions();
    }
    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        CloseHandle(CreateThread(0, 0, &ThreadMain, 0, 0, 0));
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
        if (!hacks::content.is_null()) {
            ofstream outputFile("ReplayEngine/settings.json");
            outputFile << hacks::content.dump(4);
            outputFile.close();
        }
    }
    return true;
}
