#include "pch.h"

namespace gui
{
    extern bool is_spambot;
    extern float current_time_for_keybind;

    extern bool show;
    extern bool inited;
    void BeginWindow(const char *title);
    void Render();
    void Toggle();
}