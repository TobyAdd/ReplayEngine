#include "pch.h"
#include <imgui.h>
#include <json.hpp>

extern json hacksContent;

namespace hacks
{
    void load();
    void unload();
    void render();
    bool writemem(uintptr_t address, string bytes);
}