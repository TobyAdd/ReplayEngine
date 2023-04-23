#include "framerate.h"
#include "hooks.h"
#include "replayEngine.h"

namespace framerate
{
    bool g_disable_render = false;
    float g_left_over = 0.f;

    void(__thiscall *CCScheduler_update)(CCScheduler *, float);
    void __fastcall CCScheduler_update_H(CCScheduler *self, int, float dt)
    {
        auto pl = gd::GameManager::sharedState()->getPlayLayer();
        auto el = gd::GameManager::sharedState()->getEditorLayer();
        if (pl || el)
        {
            if (pl && frameAdvance.enabled && frameAdvance.triggered)
            {
                float newdt = 1.f / replay.fps_value / 1.f;
                CCScheduler_update(self, newdt);
                frameAdvance.triggered = false;
                return;
            }

            if (!frameAdvance.enabled)
            {
                dt *= replay.speed_value;
                const float newdt = 1.f / replay.fps_value / 1.f;
                if (!replay.real_time)
                    return CCScheduler_update(self, newdt);

                g_disable_render = true;
                const int times = min(static_cast<int>((dt + g_left_over) / newdt), 100);
                for (int i = 0; i < times; ++i)
                {
                    if (i == times - 1)
                        g_disable_render = false;
                    CCScheduler_update(self, newdt);
                }
                g_left_over += dt - newdt * times;
            }
        }
        else
        {
            return CCScheduler_update(self, dt);
        }
    }

    void(__thiscall *PlayLayer_updateVisibility)(void *);
    void __fastcall PlayLayer_updateVisibility_H(void *self)
    {
        if (!g_disable_render)
            PlayLayer_updateVisibility(self);
    }

    void initHooks()
    {
        auto cocos = GetModuleHandleA("libcocos2d.dll");
        MH_CreateHook((void *)(gd::base + 0x205460), PlayLayer_updateVisibility_H, (void **)&PlayLayer_updateVisibility);
        MH_CreateHook(GetProcAddress(cocos, "?update@CCScheduler@cocos2d@@UAEXM@Z"), CCScheduler_update_H, (void **)&CCScheduler_update);
    }
}