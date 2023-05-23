#include "pch.h"

namespace hooks
{
    inline bool(__thiscall *playLayer_init)(gd::PlayLayer *self, gd::GJGameLevel *GJGameLevel);
    bool __fastcall playLayer_initHook(gd::PlayLayer *self, int edx, gd::GJGameLevel *GJGameLevel);

    inline void(__thiscall *playLayer_update)(gd::PlayLayer *self, float deltatime);
    void __fastcall playLayer_updateHook(gd::PlayLayer *self, int edx, float deltatime);

    inline void(__thiscall *playLayer_resetLevel)(gd::PlayLayer *self);
    void __fastcall playLayer_resetLevelHook(gd::PlayLayer *self);

    inline void(__thiscall *playLayer_onQuit)(gd::PlayLayer *self);
    void __fastcall playLayer_onQuitHook(gd::PlayLayer *self);

    inline void(__thiscall *playLayer_levelComplete)(gd::PlayLayer *self);
    void __fastcall playLayer_levelCompleteHook(gd::PlayLayer *self);

    inline bool(__thiscall *playLayer_pushButton)(gd::PlayLayer *self, int state, bool player);
    bool __fastcall playLayer_pushButtonHook(gd::PlayLayer *self, uintptr_t, int state, bool player);

    inline bool(__thiscall *playLayer_releaseButton)(gd::PlayLayer *self, int state, bool player);
    bool __fastcall playLayer_releaseButtonHook(gd::PlayLayer *self, uintptr_t, int state, bool player);

    inline int(__thiscall *playLayer_createCheckpoint)(gd::PlayLayer *self);
    int __fastcall playLayer_createCheckpointHook(gd::PlayLayer *self);

    inline int(__thiscall *playLayer_removeCheckpoint)(gd::PlayLayer *self);
    int __fastcall playLayer_removeCheckpointHook(gd::PlayLayer *self);

    inline void(__thiscall *playLayer_togglePractice)(void *self, bool practice);
    void __fastcall playLayer_togglePracticeHook(void *self, int edx, bool practice);

    inline void(__thiscall* dispatchKeyboardMSG)(void* self, int key, bool down);
    void __fastcall dispatchKeyboardMSGHook(void* self, void*, int key, bool down);

    void initHooks();
}