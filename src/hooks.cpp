#include "hooks.h"
#include "replayEngine.h"
#include <imgui-hook.hpp>
#include <imgui.h>
#include "hacks.h"
#include "recorder.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace hooks
{
    bool __fastcall playLayer_initHook(gd::PlayLayer *self, int edx, gd::GJGameLevel *level)
    {
        auto ret = playLayer_init(self, level);
        recorder.update_song_offset(self);
        strcpy_s(replay.replay_name, self->m_level->levelName.c_str());
        strcpy_s(recorder.video_name, string(self->m_level->levelName + ".mp4").c_str());
        practiceFix.frame_offset = 0;
        practiceFix.clear();
        return ret;
    }

    void __fastcall playLayer_updateHook(gd::PlayLayer *self, int edx, float deltaTime)
    {
        if (recorder.m_recording)
            recorder.handle_recording(self, deltaTime);
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
        if (replay.mode == record)
            replay.mode = disable;
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
        if (replay.mode == play && replay.ignore_input)
            return false;

        bool ret = playLayer_pushButton(self, state, player);

        if (replay.mode == record && !self->m_isDead)
            replay.handle_recording2(player, true);

        if (replay.dual_clicks)
        {
            playLayer_pushButton(self, 0, !player);
            if (replay.mode == record)
            {
                replay.handle_recording2(!player, true);
            }
        }

        return ret;
    }

    bool __fastcall playLayer_releaseButtonHook(gd::PlayLayer *self, uintptr_t, int state, bool player)
    {
        if (replay.mode == play && replay.ignore_input)
            return false;

        bool ret = playLayer_releaseButton(self, state, player);

        unsigned frame = replay.get_frame();
        if (replay.mode == record && !self->m_isDead)
            replay.handle_recording2(player, false);

        if (replay.dual_clicks)
        {
            playLayer_releaseButton(self, 0, !player);
            if (replay.mode == record)
            {
                replay.handle_recording2(!player, false);
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

    void __fastcall dispatchKeyboardMSGHook(void* self, void*, int key, bool down) {
        dispatchKeyboardMSG(self, key, down);
        auto pl = gd::GameManager::sharedState()->getPlayLayer();

        if (pl && down && key == 'C') {
            frameAdvance.enabled = true;
            frameAdvance.triggered = true;
        }
        else if (pl && down && key == 'F') {
            frameAdvance.enabled = false;
        }
        else if (pl && down && key == 'S') {
            spamBot.enabled = !spamBot.enabled;
            spamBot.reset_temp();
            if (!spamBot.enabled)
            {
                hooks::playLayer_releaseButtonHook(pl, 0, 0, true);
                hooks::playLayer_releaseButtonHook(pl, 0, 0, false);
            }
        }
        else if (pl && down && key == 'D') {
            straightFly.enabled = !straightFly.enabled;
            straightFly.start(pl);
            if (!straightFly.enabled)
            {
                hooks::playLayer_releaseButtonHook(pl, 0, 0, true);
                hooks::playLayer_releaseButtonHook(pl, 0, 0, false);
            }
        }
        else if (pl && down && key == 'R') {
            hooks::playLayer_resetLevelHook(pl);
        }

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
        MH_CreateHook(
            (PVOID)(GetProcAddress(GetModuleHandleA("libcocos2d.dll"), "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z")),
            dispatchKeyboardMSGHook, (LPVOID*)&dispatchKeyboardMSG
        );
    }
}