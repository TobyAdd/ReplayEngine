#define IMGUI_DEFINE_MATH_OPERATORS
#include "gui.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui-hook.hpp>
#include "hooks.hpp"
#include "hacks.hpp"
#include <shellapi.h>
#include "xorstr.hpp"

bool gui::show = false;
bool gui::inited = false;
bool gui::recording = true;
int gui::currentkeycode = -1;
int oldkeycode;
bool secret = false;

int anim_durr = -1;
auto anim_starttime = std::chrono::steady_clock::now();
bool binding_mode = false;
std::string binding_now = "please dont create a hack with this name i will be very depressed";

extern "C"
{
    __declspec(dllexport) int __stdcall GDH()
    {
        secret = true;
        return secret;
    };
}

bool isEmpty(char *str)
{
    return (str != NULL && str[0] == '\0');
}

ImVec4 colorMultiply(float color[4], float multiplier) {
    return ImVec4(std::clamp(color[0] * multiplier, 0.0f, 1.0f),
                  std::clamp(color[1] * multiplier, 0.0f, 1.0f),
                  std::clamp(color[2] * multiplier, 0.0f, 1.0f), color[3]);
}

void setStyle(ImGuiStyle &style, float accent[4])
{
    float average = (accent[0] + accent[1] + accent[2]) / 3.0f;
    ImVec4 color = ImVec4(0.0f, 0.0f, 0.0f, accent[3]);;

    if (average < 0.5f) color = ImVec4(1.0f, 1.0f, 1.0f, accent[3]);

    style.Colors[ImGuiCol_Text] = color;
    style.Colors[ImGuiCol_TextSelectedBg] = colorMultiply(accent, 0.8f);

    style.Colors[ImGuiCol_CheckMark] = color;

    style.Colors[ImGuiCol_WindowBg] = colorMultiply(accent, 0.8f);

    style.Colors[ImGuiCol_FrameBg] = colorMultiply(accent, 0.6f);
    style.Colors[ImGuiCol_FrameBgHovered] = colorMultiply(accent, 0.4f);
    style.Colors[ImGuiCol_FrameBgActive] = colorMultiply(accent, 0.6f);

    style.Colors[ImGuiCol_TitleBg] = colorMultiply(accent, 0.9f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = colorMultiply(accent, 0.8f);
    style.Colors[ImGuiCol_TitleBgActive] = colorMultiply(accent, 1.0f);

    style.Colors[ImGuiCol_ScrollbarBg] = colorMultiply(accent, 0.6f);
    style.Colors[ImGuiCol_ScrollbarGrab] = colorMultiply(accent, 0.8f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = colorMultiply(accent, 0.9f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = colorMultiply(accent, 1.0f);

    style.Colors[ImGuiCol_SliderGrab] = colorMultiply(accent, 0.8f);
    style.Colors[ImGuiCol_SliderGrabActive] = colorMultiply(accent, 1.0f);

    style.Colors[ImGuiCol_Button] = colorMultiply(accent, 0.6f);
    style.Colors[ImGuiCol_ButtonHovered] = colorMultiply(accent, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = colorMultiply(accent, 1.0f);

    style.Colors[ImGuiCol_Separator] = colorMultiply(accent, 0.6f);

    style.Colors[ImGuiCol_ResizeGrip] = colorMultiply(accent, 1.0f);
    style.Colors[ImGuiCol_ResizeGripHovered] = colorMultiply(accent, 1.2f);
    style.Colors[ImGuiCol_ResizeGripActive] = colorMultiply(accent, 1.2f);

    style.Colors[ImGuiCol_PopupBg] = colorMultiply(accent, 0.8f);
}

float get_opacity() {
    auto now = std::chrono::steady_clock::now();

    long long diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - anim_starttime).count();
    if (gui::show)
        return (float)diff / (float)anim_durr;
    else
        return 1.0f - ((float)diff / (float)anim_durr);
}

void gui::Render()
{
    if (!gui::inited) {
        gui::inited = true;
        ImGui::GetStyle().Alpha = 0;
    }

    ImGui::GetStyle().Alpha = get_opacity();
    if (!gui::show && ImGui::GetStyle().Alpha <= 0.0f) {
        return;
    }

    static std::vector<std::string> stretchedWindows;
    // IMGUI pinkish colors
    static float color[4] = { 0.337f, 0.176f, 0.464f, 1.000f };
    static float default_color[4] = { 0.337f, 0.176f, 0.464f, 1.000f };

    for (auto &item : hacks::content.items())
    {
        std::string windowName = item.key();

        if (std::find(stretchedWindows.begin(), stretchedWindows.end(), windowName) == stretchedWindows.end())
        {
            ImVec2 windowSize = ImVec2(float(item.value()["windowSize"]["width"]),
                                       float(item.value()["windowSize"]["height"]));
            ImVec2 windowPos = ImVec2(float(item.value()["windowPosition"]["x"]),
                                      float(item.value()["windowPosition"]["y"]));

            ImGui::SetNextWindowSize(windowSize);
            ImGui::SetNextWindowPos(windowPos);

            stretchedWindows.push_back(windowName);
        }

        ImGui::Begin(windowName.c_str());

        item.value()["windowSize"]["width"] = ImGui::GetWindowSize().x;
        item.value()["windowSize"]["height"] = ImGui::GetWindowSize().y;
        item.value()["windowPosition"]["x"] = ImGui::GetWindowPos().x;
        item.value()["windowPosition"]["y"] = ImGui::GetWindowPos().y;

        auto &components = item.value()["components"];
        if (!components.is_null())
        {
            for (auto &component : components)
            {
                std::string type = component["type"];
                if (type == "replay_engine")
                {
                    static string log = "Record/Replay or Save/Load Macros!";
                    int mode = (int)engine.mode;

                    if (ImGui::RadioButton("Disable", &mode, 0))
                        engine.mode = state::disable;
                    ImGui::SameLine();

                    if (ImGui::RadioButton("Record", &mode, 1))
                    {
                        if (engine.mode != state::record)
                            engine.replay.clear();
                        engine.mode = state::record;
                    }
                    ImGui::SameLine();

                    if (ImGui::RadioButton("Play", &mode, 2))
                        engine.mode = state::play;

                    ImGui::Separator();

                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::InputText("##replay_name", engine.replay_name, IM_ARRAYSIZE(engine.replay_name));

                    if (ImGui::Button("Save", {60, NULL}))
                    {
                        if (engine.replay.empty())
                        {
                            log = "Replay doens't have actions";
                        }
                        else
                        {
                            if (isEmpty(engine.replay_name))
                            {
                                log = "Replay name is empty";
                            }
                            else
                            {
                                std::ofstream file("GDH/macros/" + (string)engine.replay_name + ".txt");

                                if (file.is_open())
                                {
                                    file << engine.fps << "\n";
                                    for (auto &action : engine.replay)
                                    {
                                        file << action.frame << " " << action.hold << " " << action.player_button << " " << action.player << "\n";
                                    }
                                    file.close();
                                    log = "Replay Saved";
                                }
                            }
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Load", {60, NULL}))
                    {
                        if (!engine.replay.empty())
                        {
                            log = "Please clear replay before loading another";
                        }
                        else
                        {
                            if (isEmpty(engine.replay_name))
                            {
                                log = "Replay name is empty";
                            }
                            else
                            {
                                std::ifstream file("GDH/macros/" + (string)engine.replay_name + ".txt");
                                std::string line;

                                if (file.is_open())
                                {
                                    std::getline(file, line);
                                    engine.fps = stof(line);
                                    while (std::getline(file, line))
                                    {
                                        std::istringstream iss(line);
                                        int value1, value2, value3, value4;

                                        if (iss >> value1 >> value2 >> value3 >> value4)
                                        {
                                            engine.replay.push_back({(unsigned)value1, (bool)value2, (int)value3, (bool)value4});
                                        }
                                    }
                                    file.close();
                                    log = "Replay Loaded";
                                }
                                else
                                {
                                    log = "Replay doens't exists";
                                }
                            }
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Clear Replay", {80, NULL}))
                    {
                        log = "Replay has been cleared";
                        engine.replay.clear();
                    }

                    ImGui::Separator();

                    ImGui::Text("Replay size: %i", engine.replay.size());
                    ImGui::Text("Frame: %i", hooks::frame);

                    ImGui::Separator();

                    ImGui::Text(log.c_str());
                }
            }
        }

        ImGui::End();
    }
    ImGui::End();
}

void gui::Toggle()
{
    gui::show = !gui::show;
    anim_starttime = std::chrono::steady_clock::now();
}
