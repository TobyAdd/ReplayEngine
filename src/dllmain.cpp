#include "pch.h"
#include <imgui-hook.hpp>
#include <imgui.h>
#include "gui.h"
#include "hooks.h"
#include "framerate.h"
#include "hacks.h"
#include <functional>
#include <winternl.h>

bool show = true;
bool noclipEnabled = false;

void CheckDir(string dir)
{
    if (!filesystem::is_directory(dir) || !filesystem::exists(dir))
    {
        filesystem::create_directory(dir);
    }
}

bool IsWindows81orHigher() {
	NTSTATUS(WINAPI * RtlGetVersion)(LPOSVERSIONINFOEXW);
	OSVERSIONINFOEXW osInfo;

	*(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");

	if (NULL != RtlGetVersion)
	{
		osInfo.dwOSVersionInfoSize = sizeof(osInfo);
		RtlGetVersion(&osInfo);
		return (osInfo.dwMajorVersion >= 6 && osInfo.dwMinorVersion >= 3) || osInfo.dwMajorVersion >= 10;
	}

	return false;
}

inline void (__thiscall* LoadingLayer_init)(cocos2d::CCLayer*, char);
void __fastcall LoadingLayer_initHook(cocos2d::CCLayer* layer, void*, char boolean) {
    CCLabelBMFont* label = cocos2d::CCLabelBMFont::create("Replay Engine requires Windows 8.1 or higher. Sorry :(\nUnload Replay Engine to use Geometry Dash", "chatfont.fnt");

    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    label->setPosition(winSize.width / 2, winSize.height / 2);
    label->setAlignment(kCCTextAlignmentCenter);
    layer->addChild(label);
}

DWORD WINAPI ThreadMain(LPVOID lpParam)
{
    CheckDir("ReplayEngine");
    CheckDir("ReplayEngine/Replays");
    CheckDir("ReplayEngine/Videos");
    CheckDir("ReplayEngine/Clicks");
    CheckDir("ReplayEngine/Converter");
    CheckDir("ReplayEngine/Temp");
    ImGuiHook::setRenderFunction(gui::Render);
    ImGuiHook::setToggleFunction([]() { gui::Toggle(); });
    if (MH_Initialize() == MH_OK)
    {
        if (!IsWindows81orHigher()) {
            MH_CreateHook((PVOID)(gd::base + 0x18C080), LoadingLayer_initHook, (LPVOID*)&LoadingLayer_init);
        }
        else {
            ImGuiHook::Load([](void *target, void *hook, void **trampoline)
                                { MH_CreateHook(target, hook, trampoline); });

            hooks::initHooks();
            framerate::initHooks();
        }
        MH_EnableHook(MH_ALL_HOOKS);
    }
    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        CloseHandle(CreateThread(0, 0, &ThreadMain, 0, 0, 0));
    }
    return true;
}