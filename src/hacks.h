#include "pch.h"
#include <imgui.h>

namespace hacks
{
    void render();
    extern bool noclip;
    extern bool practice_music_hack;
    extern bool practice_coins;
    extern bool anticheat_bypass;
    extern bool ignore_esc;
    extern bool no_respawn_flash;
    extern bool disable_death_effects;
    void nolcip_f(bool enable);
    void practice_music_hack_f(bool enable);
    void practice_coins_f(bool enable);
    void anticheat_bypass_f(bool enable);
    void no_respawn_flash_f(bool enable);
    void disable_death_effects_f(bool enable);
}