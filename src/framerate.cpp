#include "framerate.h"
#include "hooks.h"
#include "replayEngine.h"
#include "recorder.hpp"


bool keyPressed = false;
bool isFirstPress = true;
std::chrono::steady_clock::time_point lastKeyPressTime;

namespace framerate
{
    bool g_disable_render = false;
    float g_left_over = 0.f;
    bool enabled = true;
    bool enabled_fps = true;

    void(__thiscall *CCScheduler_update)(CCScheduler *, float);
    void __fastcall CCScheduler_update_H(CCScheduler* self, int, float dt) {
        const auto play_layer = gd::GameManager::sharedState()->getPlayLayer();
        const auto editor_layer = gd::GameManager::sharedState()->getEditorLayer();

        dt *= replay.speed_value;
        const float target_dt = 1.f / replay.fps_value;

        if (enabled_fps)
            CCDirector::sharedDirector()->setAnimationInterval(target_dt);

        if (!enabled) {
            CCScheduler_update(self, dt);
            return;
        }

        if (!play_layer && !editor_layer && (replay.mode == record || replay.mode == play || recorder.m_recording)) {
            CCScheduler_update(self, dt);
            return;
        }

        bool isPaused = play_layer ? play_layer->m_isPaused : true;

        if (!replay.real_time) {
            CCScheduler_update(self, target_dt);
            return;
        }

        if (frameAdvance.enabled) {
            if (frameAdvance.triggered) {
                frameAdvance.triggered = false;
                CCScheduler_update(self, target_dt);
            }
            return;
        }

        unsigned times = static_cast<int>((dt + g_left_over) / target_dt);
        if (dt == 0.f) {
            return;
        }

        auto start = std::chrono::high_resolution_clock::now();
        for (unsigned i = 0; i < times; ++i) {
            CCScheduler_update(self, target_dt);
            using namespace std::literals;
            if (std::chrono::high_resolution_clock::now() - start > 33.333ms) {
                times = i + 1;
                break;
            }
        }
        g_left_over += dt - target_dt * times;
    }


    void initHooks()
    {
        auto cocos = GetModuleHandleA("libcocos2d.dll");
        MH_CreateHook(GetProcAddress(cocos, "?update@CCScheduler@cocos2d@@UAEXM@Z"), CCScheduler_update_H, (void **)&CCScheduler_update);
    }
}