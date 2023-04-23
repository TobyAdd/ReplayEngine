#include "hacks.h"

namespace hacks
{
    bool noclip = false;
    bool practice_music_hack = false;
    bool practice_coins = false;
    bool anticheat_bypass = true;
    bool ignore_esc = false;
    bool no_respawn_flash = false;
    bool disable_death_effects = false;

    void render()
    {
        if (ImGui::Checkbox("Noclip", &noclip))
        {
            if (noclip)
                nolcip_f(true);
            else
                nolcip_f(false);
        }

        if (ImGui::Checkbox("Practice Music Hack", &practice_music_hack))
        {
            if (practice_music_hack)
                practice_music_hack_f(true);
            else
                practice_music_hack_f(false);
        }

        if (ImGui::Checkbox("Practice Coins", &practice_coins))
        {
            if (practice_coins)
                practice_coins_f(true);
            else
                practice_coins_f(false);
        }

        if (ImGui::Checkbox("Anticheat Bypass", &anticheat_bypass))
        {
            if (anticheat_bypass)
                anticheat_bypass_f(true);
            else
                anticheat_bypass_f(false);
        }

        ImGui::Checkbox("Ignore ESC", &ignore_esc);

        if (ImGui::Checkbox("No respawn flash", &no_respawn_flash))
        {
            if (no_respawn_flash)
                no_respawn_flash_f(true);
            else
                no_respawn_flash_f(false);
        }

        if (ImGui::Checkbox("Disable Death Effects", &disable_death_effects))
        {
            if (disable_death_effects)
                disable_death_effects_f(true);
            else
                disable_death_effects_f(false);
        }
    }

    void nolcip_f(bool enable)
    {
        if (enable)
        {
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x20A23C), "\xE9\x79\x06\x00\x00", 5, NULL);
        }
        else
        {
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x20A23C), "\x6A\x14\x8B\xCB\xFF", 5, NULL);
        }
    }

    void practice_music_hack_f(bool enable)
    {
        if (enable)
        {
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x20C925), "\x90\x90\x90\x90\x90\x90", 6, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x20D143), "\x90\x90", 2, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x20A563), "\x90\x90", 2, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x20A595), "\x90\x90", 2, NULL);
        }
        else
        {
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x20C925), "\x0F\x85\xF7\x00\x00\x00", 6, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x20D143), "\x75\x41", 2, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x20A563), "\x75\x3E", 2, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x20A595), "\x75\x0C", 2, NULL);
        }
    }

    void practice_coins_f(bool enable)
    {
        if (enable)
        {
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x204F10), "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 13, NULL);
        }
        else
        {
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x204F10), "\x80\xBE\x95\x04\x00\x00\x00\x0F\x85\xDE\x00\x00\x00", 13, NULL);
        }
    }

    void anticheat_bypass_f(bool enable)
    {
        if (enable)
        {
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x202AAA), "\xEB\x2E", 2, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x15FC2E), "\xEB", 1, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x20D3B3), "\x90\x90\x90\x90\x90", 5, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1FF7A2), "\x90\x90", 2, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x18B2B4), "\xB0\x01", 2, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x20C4E6), "\xE9\xD7\x00\x00\x00\x90", 6, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1FD557), "\xEB\x0C", 2, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1FD742), "\xC7\x87\xE0\x02\x00\x00\x01\x00\x00\x00\xC7\x87\xE4\x02\x00\x00\x00\x00\x00\x00\x90\x90\x90\x90\x90\x90", 26, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1FD756), "\x90\x90\x90\x90\x90\x90", 6, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1FD79A), "\x90\x90\x90\x90\x90\x90", 6, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1FD7AF), "\x90\x90\x90\x90\x90\x90", 6, NULL);
        }
        else
        {
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x202AAA), "\x74\x2E", 2, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x15FC2E), "\x74", 1, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x20D3B3), "\xE8\x58\x04\x00\x00", 5, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1FF7A2), "\x74\x6E", 2, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x18B2B4), "\x88\xD8", 2, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x20C4E6), "\x0F\x85\xD6\x00\x00\x00", 6, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1FD557), "\x74\x0C", 2, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1FD742), "\x80\xBF\xDD\x02\x00\x00\x00\x0F\x85\x0A\xFE\xFF\xFF\x80\xBF\x34\x05\x00\x00\x00\x0F\x84\xFD\xFD\xFF\xFF", 26, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1FD557), "\x74\x0C", 2, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1FD756), "\x0F\x84\xFD\xFD\xFF\xFF", 6, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1FD79A), "\x0F\x84\xB9\xFD\xFF\xFF", 6, NULL);
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1FD7AF), "\x0F\x85\xA4\xFD\xFF\xFF", 6, NULL);
        }
    }

    void no_respawn_flash_f(bool enable)
    {
        if (enable)
        {
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1EF36D), "\xE9\xA8\x00\x00\x00\x90", 6, NULL);
        }
        else
        {
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1EF36D), "\x0F\x85\xA7\x00\x00\x00", 6, NULL);
        }
    }

    void disable_death_effects_f(bool enable)
    {
        if (enable)
        {
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1EFBA4), "\x90\x90\x90\x90\x90", 5, NULL);
        }
        else
        {
            WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void *>(gd::base + 0x1EFBA4), "\xE8\x37\x00\x00\x00", 5, NULL);
        }
    }

}