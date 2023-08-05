#include "hooks.h"
#include "replayEngine.h"
#include <imgui-hook.hpp>
#include <imgui.h>
#include "hacks.h"
#include "recorder.hpp"

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
        practiceFix.activated_objects_p1.clear();
        practiceFix.activated_objects_p2.clear();
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
            if (replay.continue_toggled && !replay.replay.empty() && replay.get_frame() >= replay.replay.back().frame)
            {
                replay.mode = record;
                replay.continue_toggled = false;
            }
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
        sequence.do_some_magic();
        playLayer_resetLevel(self);
        replay.handle_reseting(self);
        if (replay.mode == continue_record)
        {
            replay.continue_toggled = true;
            replay.mode = play;
            if (!self->m_isPracticeMode)
                playLayer_togglePractice(self, true);
        }
    }

    void __fastcall playLayer_onQuitHook(gd::PlayLayer *self)
    {
        playLayer_onQuit(self);
        if (replay.mode == record || replay.mode == continue_record)
        {
            replay.mode = disable;
            replay.continue_toggled = false;
        }
        frameAdvance.enabled = false;
        sequence.first_sqp = true;
    }

    void __fastcall playLayer_levelCompleteHook(gd::PlayLayer *self)
    {
        playLayer_levelComplete(self);
        if (replay.mode == record)
            replay.mode = disable;
    }

    int __fastcall playLayer_deathHook(gd::PlayLayer *self, void *, gd::PlayerObject *player, gd::GameObject *obj)
    {
        int ret = 0;

        if (player == self->m_player1 && hacks::noclipP1)
        {
            return ret;
        }
        else if (player == self->m_player2 && hacks::noclipP2)
        {
            return ret;
        }

        ret = playLayer_death(self, player, obj);
        return ret;
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

    bool __fastcall levelEditorLayer_initHook(gd::LevelEditorLayer *self, int edx, gd::GJGameLevel *level)
    {
        if (!levelEditorLayer_init(self, level))
            return false;
        if (replay.mode == record || replay.mode == continue_record)
        {
            replay.mode = disable;
            replay.continue_toggled = false;
        }
        sequence.first_sqp = true;
        frameAdvance.enabled = false;
        return true;
    }

    void __fastcall dispatchKeyboardMSGHook(void *self, void *, int key, bool down)
    {
        dispatchKeyboardMSG(self, key, down);
        auto pl = gd::GameManager::sharedState()->getPlayLayer();

        if (pl && down && key == 'C')
        {
            frameAdvance.enabled = true;
            frameAdvance.triggered = true;
        }
        else if (pl && down && key == 'V')
        {
            frameAdvance.enabled = false;
        }
        else if (pl && down && key == 'R')
        {
            hooks::playLayer_resetLevelHook(pl);
        }
        else if (pl && down && key == 'P')
        {
            if (replay.mode == play)
            {
                replay.mode = disable;
            }
            else
            {
                replay.mode = play;
            }
        }
    }

    void __fastcall PlayerObject_ringJumpHook(gd::PlayerObject *self, void *, gd::GameObject *ring)
    {
        bool a = ring->m_hasBeenActivated;
        bool b = ring->m_hasBeenActivatedP2;
        PlayerObject_ringJump(self, ring);
        practiceFix.handle_activated_object(a, b, ring);
    }

    void __fastcall GameObject_activateObjectHook(gd::GameObject *self, void *, gd::PlayerObject *player)
    {
        bool a = self->m_hasBeenActivated;
        bool b = self->m_hasBeenActivatedP2;
        GameObject_activateObject(self, player);
        practiceFix.handle_activated_object(a, b, self);
    }

    void __fastcall GJBaseGameLayer_bumpPlayerHook(gd::GJBaseGameLayer *self, void *, gd::PlayerObject *player, gd::GameObject *object)
    {
        bool a = object->m_hasBeenActivated;
        bool b = object->m_hasBeenActivatedP2;
        GJBaseGameLayer_bumpPlayer(self, player, object);
        practiceFix.handle_activated_object(a, b, object);
    }

    void initHooks()
    {
        MH_CreateHook((PVOID)(gd::base + 0x01FB780), playLayer_initHook, (LPVOID *)&playLayer_init);
        MH_CreateHook((PVOID)(gd::base + 0x2029C0), playLayer_updateHook, (LPVOID *)&playLayer_update);
        MH_CreateHook((PVOID)(gd::base + 0x20BF00), playLayer_resetLevelHook, (LPVOID *)&playLayer_resetLevel);
        MH_CreateHook((PVOID)(gd::base + 0x20D810), playLayer_onQuitHook, (LPVOID *)&playLayer_onQuit);
        MH_CreateHook((PVOID)(gd::base + 0x1FD3D0), playLayer_levelCompleteHook, (LPVOID *)&playLayer_levelComplete);
        MH_CreateHook((PVOID)(gd::base + 0x20A1A0), playLayer_deathHook, (LPVOID *)&playLayer_death);
        MH_CreateHook((PVOID)(gd::base + 0x111500), playLayer_pushButtonHook, (LPVOID *)&playLayer_pushButton);
        MH_CreateHook((PVOID)(gd::base + 0x111660), playLayer_releaseButtonHook, (LPVOID *)&playLayer_releaseButton);
        MH_CreateHook((PVOID)(gd::base + 0x20B050), playLayer_createCheckpointHook, (LPVOID *)&playLayer_createCheckpoint);
        MH_CreateHook((PVOID)(gd::base + 0x20B830), playLayer_removeCheckpointHook, (LPVOID *)&playLayer_removeCheckpoint);
        MH_CreateHook((PVOID)(gd::base + 0x20D0D0), playLayer_togglePracticeHook, (LPVOID *)&playLayer_togglePractice);
        MH_CreateHook((PVOID)(gd::base + 0x15EE00), levelEditorLayer_initHook, (LPVOID *)&levelEditorLayer_init);
        MH_CreateHook(
            (PVOID)(GetProcAddress(GetModuleHandleA("libcocos2d.dll"), "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z")),
            dispatchKeyboardMSGHook, (LPVOID *)&dispatchKeyboardMSG);
        MH_CreateHook((PVOID)(gd::base + 0x1f4ff0), PlayerObject_ringJumpHook, (LPVOID *)&PlayerObject_ringJump);
        MH_CreateHook((PVOID)(gd::base + 0xef0e0), GameObject_activateObjectHook, (LPVOID *)&GameObject_activateObject);
        MH_CreateHook((PVOID)(gd::base + 0x10ed50), GJBaseGameLayer_bumpPlayerHook, (LPVOID *)&GJBaseGameLayer_bumpPlayer);
    }
}