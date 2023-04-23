#include "hooks.h"
#include "replayEngine.h"
#include <imgui-hook.hpp>
#include <imgui.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace hooks
{
    bool __fastcall playLayer_initHook(gd::PlayLayer *self, int edx, gd::GJGameLevel *level)
    {
        auto ret = playLayer_init(self, level);
        practiceFix.frame_offset = 0;
        practiceFix.clear();
        return ret;
    }

    void __fastcall playLayer_updateHook(gd::PlayLayer *self, int edx, float deltaTime)
    {
        playLayer_update(self, deltaTime);
        if (replay.mode == play)
        {
            replay.handle_playing(self);
        }
        else if (replay.mode == record)
        {
            if (replay.get_frame() == 0 && replay.empty())
            {
                replay.handle_recording(self, true);
                replay.handle_recording(self, false);
            }

            if (replay.get_frame() != 0 && !self->m_isDead)
            {
                replay.handle_recording(self, true);
                replay.handle_recording(self, false);
            }
        }
        spamBot.handle_spambot(self);
        straightFly.handle_straightfly(self);
    }

    void __fastcall playLayer_resetLevelHook(gd::PlayLayer *self)
    {
        playLayer_resetLevel(self);
        replay.handle_reseting(self);
    }

    void __fastcall playLayer_onQuitHook(gd::PlayLayer *self)
    {
        playLayer_onQuit(self);
        frameAdvance.enabled = false;
    }

    void __fastcall playLayer_levelCompleteHook(gd::PlayLayer *self)
    {
        playLayer_levelComplete(self);
        if (replay.mode == record)
            replay.mode = disable;
    }

    bool __fastcall playLayer_pushButtonHook(gd::PlayLayer *self, uintptr_t, int state, bool player)
    {
        if (self->m_isDead && replay.mode == record)
            return false;
        else if (replay.mode == play && replay.ignore_input)
            return false;

        bool ret = playLayer_pushButton(self, state, player);

        unsigned frame = replay.get_frame();
        if (replay.mode == record && !self->m_isDead)
            replay.handle_recording2(frame, player, true);

        if (replay.dual_clicks)
        {
            playLayer_pushButton(self, 0, !player);
            if (replay.mode == record)
            {
                replay.handle_recording2(frame, !player, true);
            }
        }

        return ret;
    }

    bool __fastcall playLayer_releaseButtonHook(gd::PlayLayer *self, uintptr_t, int state, bool player)
    {
        if (self->m_isDead && replay.mode == record)
            return false;
        else if (replay.mode == play && replay.ignore_input)
            return false;

        bool ret = playLayer_releaseButton(self, state, player);

        unsigned frame = replay.get_frame();
        if (replay.mode == record && !self->m_isDead)
            replay.handle_recording2(frame, player, false);

        if (replay.dual_clicks)
        {
            playLayer_releaseButton(self, 0, !player);
            if (replay.mode == record)
            {
                replay.handle_recording2(frame, !player, false);
            }
        }

        return ret;
    }

    int __fastcall playLayer_createCheckpointHook(gd::PlayLayer *self)
    {
        auto ret = playLayer_createCheckpoint(self);
        practiceFix.handle_checkpoint(self);
        return ret;
    }

    int __fastcall playLayer_removeCheckpointHook(gd::PlayLayer *self)
    {
        auto ret = playLayer_removeCheckpoint(self);
        practiceFix.delete_last();
        return ret;
    }

    void __fastcall playLayer_togglePracticeHook(void *self, int edx, bool practice)
    {
        playLayer_togglePractice(self, practice);
        practiceFix.clear();
        practiceFix.update_frame_offset();
    }

    void(__thiscall *CCEGLView_pollEvents)(CCEGLView *);
    void __fastcall CCEGLView_pollEvents_H(CCEGLView *self)
    {
        auto &io = ImGui::GetIO();

        auto pl = gd::GameManager::sharedState()->getPlayLayer();

        bool blockInput = false;
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);

            if (io.WantCaptureMouse)
            {
                switch (msg.message)
                {
                case WM_LBUTTONDBLCLK:
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                case WM_MBUTTONDBLCLK:
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                case WM_MOUSEACTIVATE:
                case WM_MOUSEHOVER:
                case WM_MOUSEHWHEEL:
                case WM_MOUSELEAVE:
                case WM_MOUSEMOVE:
                case WM_MOUSEWHEEL:
                case WM_NCLBUTTONDBLCLK:
                case WM_NCLBUTTONDOWN:
                case WM_NCLBUTTONUP:
                case WM_NCMBUTTONDBLCLK:
                case WM_NCMBUTTONDOWN:
                case WM_NCMBUTTONUP:
                case WM_NCMOUSEHOVER:
                case WM_NCMOUSELEAVE:
                case WM_NCMOUSEMOVE:
                case WM_NCRBUTTONDBLCLK:
                case WM_NCRBUTTONDOWN:
                case WM_NCRBUTTONUP:
                case WM_NCXBUTTONDBLCLK:
                case WM_NCXBUTTONDOWN:
                case WM_NCXBUTTONUP:
                case WM_RBUTTONDBLCLK:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                case WM_XBUTTONDBLCLK:
                case WM_XBUTTONDOWN:
                case WM_XBUTTONUP:
                    blockInput = ImGuiHook::blockMetaInput;
                }
            }

            if (io.WantCaptureKeyboard)
            {
                switch (msg.message)
                {
                case WM_HOTKEY:
                case WM_KEYDOWN:
                case WM_KEYUP:
                case WM_KILLFOCUS:
                case WM_SETFOCUS:
                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                    blockInput = true;
                }
            }
            else if (msg.message == WM_KEYDOWN && msg.wParam == 'K')
            {
                ImGuiHook::g_toggleCallback();
            }
            else if (msg.message == WM_KEYDOWN && msg.wParam == 'C' && pl)
            {
                frameAdvance.enabled = true;
                frameAdvance.triggered = true;
            }
            else if (msg.message == WM_KEYDOWN && msg.wParam == 'F' && pl)
            {
                frameAdvance.enabled = false;
            }
            else if (msg.message == WM_KEYDOWN && msg.wParam == 'S' && pl)
            {
                spamBot.enabled = !spamBot.enabled;
                spamBot.reset_temp();
                if (!spamBot.enabled)
                {
                    hooks::playLayer_releaseButtonHook(pl, 0, 0, true);
                    hooks::playLayer_releaseButtonHook(pl, 0, 0, false);
                }
            }
            else if (msg.message == WM_KEYDOWN && msg.wParam == 'D' && pl)
            {
                straightFly.enabled = !straightFly.enabled;
                straightFly.start(pl);
                if (!straightFly.enabled)
                {
                    hooks::playLayer_releaseButtonHook(pl, 0, 0, true);
                    hooks::playLayer_releaseButtonHook(pl, 0, 0, false);
                }
            }
            else if (msg.message == WM_KEYDOWN && msg.wParam == 'R' && pl)
            {
                hooks::playLayer_resetLevelHook(pl);
            }

            if (!blockInput)
                DispatchMessage(&msg);

            ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam);
        }

        CCEGLView_pollEvents(self);
    }

    void initHooks()
    {
        MH_CreateHook((PVOID)(gd::base + 0x01FB780), playLayer_initHook, (LPVOID *)&playLayer_init);
        MH_CreateHook((PVOID)(gd::base + 0x2029C0), playLayer_updateHook, (LPVOID *)&playLayer_update);
        MH_CreateHook((PVOID)(gd::base + 0x20BF00), playLayer_resetLevelHook, (LPVOID *)&playLayer_resetLevel);
        MH_CreateHook((PVOID)(gd::base + 0x20D810), playLayer_onQuitHook, (LPVOID *)&playLayer_onQuit);
        MH_CreateHook((PVOID)(gd::base + 0x1FD3D0), playLayer_levelCompleteHook, (LPVOID *)&playLayer_levelComplete);
        MH_CreateHook((PVOID)(gd::base + 0x111500), playLayer_pushButtonHook, (LPVOID *)&playLayer_pushButton);
        MH_CreateHook((PVOID)(gd::base + 0x111660), playLayer_releaseButtonHook, (LPVOID *)&playLayer_releaseButton);
        MH_CreateHook((PVOID)(gd::base + 0x20B050), playLayer_createCheckpointHook, (LPVOID *)&playLayer_createCheckpoint);
        MH_CreateHook((PVOID)(gd::base + 0x20B830), playLayer_removeCheckpointHook, (LPVOID *)&playLayer_removeCheckpoint);
        MH_CreateHook((PVOID)(gd::base + 0x20D0D0), playLayer_togglePracticeHook, (LPVOID *)&playLayer_togglePractice);
        MH_CreateHook(GetProcAddress(GetModuleHandleA("libcocos2d.dll"), "?pollEvents@CCEGLView@cocos2d@@QAEXXZ"), CCEGLView_pollEvents_H,
            reinterpret_cast<void **>(&CCEGLView_pollEvents));
    }
}