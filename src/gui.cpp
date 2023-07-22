#define IMGUI_DEFINE_MATH_OPERATORS
#include "gui.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui-hook.hpp>
#include <shellapi.h>
#include <chrono>
#include "replayEngine.h"
#include "hooks.h"
#include "hacks.h"
#include "recorder.hpp"
#include "framerate.h"

static int currentTab = 0;
bool gui::show = false;
bool gui::inited = false;
bool meta = true;

void MetaRender()
{
    if (meta)
    {
        auto pl = gd::GameManager::sharedState()->getPlayLayer();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1);
        ImGui::Begin("Meta", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
        if (pl)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 200).Value);
            auto dual = pl->m_isDualMode;
            if (!dual)
            {
                ImGui::Text("Replay Engine Beta\nFrame: %i\nPosition X: %f\nPosition Y: %f\nRotation %f\nY Accel: %f",
                            replay.get_frame(), pl->m_player1->m_position.x, pl->m_player1->m_position.y, pl->m_player1->getRotation(),
                            pl->m_player1->m_yAccel);
            }
            else
            {
                ImGui::Text("Replay Engine Beta\nFrame: %i\nPosition X: %f, %f\nPosition Y: %f, %f\nRotation %f, %f\nY Accel: %f, %f",
                            replay.get_frame(), pl->m_player1->m_position.x, pl->m_player2->m_position.x,
                            pl->m_player1->m_position.y, pl->m_player2->m_position.y, pl->m_player1->getRotation(), pl->m_player2->getRotation(),
                            pl->m_player1->m_yAccel, pl->m_player2->m_yAccel);
            }
            ImGui::PopStyleColor();
        }
        ImGuiHook::blockMetaInput = !ImGui::IsWindowHovered(ImGuiHoveredFlags_None);
        ImGui::End();
        ImGui::PopStyleVar();
    }
}

bool opennedSP = false;
vector<string> replay_list;

void SelectReplay()
{
    auto itemRectMin = ImGui::GetItemRectMin();
    auto itemRectMax = ImGui::GetItemRectMax();
    auto itemRectSize = ImGui::GetItemRectSize();

    ImGui::SameLine();

    if (ImGui::ArrowButton("##comboopen", opennedSP ? ImGuiDir_Up : ImGuiDir_Down))
    {
        opennedSP = !opennedSP;
        if (opennedSP)
        {
            replay_list.clear();
            for (const auto &entry : filesystem::directory_iterator("ReplayEngine/Replays"))
            {
                string replay = entry.path().filename().string();
                if (replay.substr(replay.size() - 3, replay.size()) == ".re")
                {
                    replay_list.push_back(entry.path().filename().string().erase(replay.size() - 3, replay.size()));
                }
            }
        }
    }

    if (opennedSP)
    {
        ImGui::SetNextWindowPos(ImVec2(itemRectMin.x, itemRectMax.y + 4));
        ImGui::SetNextWindowSize(ImVec2(itemRectSize.x + ImGui::GetItemRectSize().x + 5, 300));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1);
        ImGui::Begin("##replaylist", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        for (int i = 0; i < (int)replay_list.size(); i++)
        {
            if (ImGui::MenuItem(replay_list[i].c_str()))
            {
                strcpy_s(replay.replay_name, replay_list[i].c_str());
                opennedSP = false;
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }
}

char *items[] = {"General", "Assist", "Hacks", "Editor", "Recorder", "Converter", "Sequence", "Clickbot (Beta)", "About"};
int items_index = 0;

int editor_indexInputs = 0;
int editor_indexPhysics = 0;
char *items_editor[] = {"Inputs", "Physics"};
int itemsEditor_index = 0;

char *converterTypes[] = {"Plain Text (.txt)"};

void gui::Render()
{
    MetaRender();

    if (gui::show)
    {
        if (!gui::inited)
        {
            gui::inited = true;
            ImGui::SetNextWindowSize(ImVec2(600, 400));
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            hacks::load();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Replay Engine", nullptr, ImGuiWindowFlags_NoTitleBar);
        ImGui::PopStyleVar();

        ImGui::PushStyleColor(ImGuiCol_Border, 1);
        ImGui::BeginChild("##menu", ImGui::GetContentRegionAvail(), true);
        ImGui::PopStyleColor();

        ImGui::BeginChild("##leftsideMain", ImVec2(150, NULL), false, ImGuiWindowFlags_NoScrollbar);

        ImGui::BeginChild("##leftside", ImVec2(150, (items_index == 3) ? (ImGui::GetContentRegionAvail().y - 40) : NULL), false);
        ImGui::Text("Replay Engine");
        ImGui::Separator();
        ImGui::Spacing();
        for (int i = 0; i < IM_ARRAYSIZE(items); i++)
        {
            bool is_selected = (items_index == i);
            if (ImGui::Selectable(items[i], is_selected))
                items_index = i;
        }
        ImGui::EndChild();

        if (items_index == 3)
        {
            ImGui::BeginChild("##leftside2", ImVec2(150, NULL), false);
            for (int i = 0; i < IM_ARRAYSIZE(items_editor); i++)
            {
                bool is_selected = (itemsEditor_index == i);
                if (ImGui::Selectable(items_editor[i], is_selected))
                    itemsEditor_index = i;
            }
            ImGui::EndChild();
        }

        ImGui::EndChild();

        ImGui::SameLine(0, 5);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine(0, 5);

        ImGui::BeginChild("##rightside", ImVec2(ImGui::GetContentRegionAvail().x, NULL));

        if (items_index == 0)
        {
            int mode = (int)replay.mode;

            if (ImGui::RadioButton("Disable", &mode, 0))
                replay.mode = (state)mode;
            ImGui::SameLine();

            if (ImGui::RadioButton("Record", &mode, 1))
            {
                if (gd::GameManager::sharedState()->getGameVariable("0027"))
                { // Disabling Auto-Checkpoints option
                    gd::GameManager::sharedState()->setGameVariable("0027", false);
                }
                replay.mode = (state)mode;
                replay.clear();
            }
            ImGui::SameLine();

            if (ImGui::RadioButton("Play", &mode, 2))
                replay.mode = (state)mode;

            ImGui::SameLine();

            if (ImGui::RadioButton("Continue", &mode, 3))
                replay.mode = (state)mode;
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("This mode will allow you to continue recording level. First, you need to load unfinished replay and switch to the \"Continue\" mode\nAfter the next attempt, the mode will switch to \"Play\", and when the level reaches the frame that is the last frame of the replay, the mode will switch to \"Record\"\nPractice mode is activated automatically after switching to the \"Continue\" recording mode. Additionally, you can place checkpoints");

            ImGui::Separator();

            ImGui::InputText("##ReplayName", replay.replay_name, IM_ARRAYSIZE(replay.replay_name));
            SelectReplay();

            if (ImGui::Button("Save", ImVec2(80, NULL)))
            {
                if ((replay.replay_name != NULL) && (replay.replay_name[0] == '\0'))
                    gd::FLAlertLayer::create(nullptr, "Info", "Ok", nullptr, "Replay name is empty")->show();
                else
                {
                    gd::FLAlertLayer::create(nullptr, "Info", "Ok", nullptr, replay.save((string)replay.replay_name))->show();
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Load", ImVec2(80, NULL)))
            {
                if ((replay.replay_name != NULL) && (replay.replay_name[0] == '\0'))
                    gd::FLAlertLayer::create(nullptr, "Info", "Ok", nullptr, "Replay name is empty")->show();
                else
                {
                    gd::FLAlertLayer::create(nullptr, "Info", "Ok", nullptr, replay.load((string)replay.replay_name))->show();
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Clear Replay", ImVec2(119, NULL)))
            {
                replay.clear();
            }

            ImGui::Separator();

            ImGui::PushItemWidth(135.f);
            ImGui::DragFloat("##FPS", &replay.fps_value, 1.f, 1.f, FLT_MAX, "FPS: %.2f");

            ImGui::SameLine();

            ImGui::PushItemWidth(137.f);
            ImGui::DragFloat("##Speed", &replay.speed_value, 0.01f, 0.f, FLT_MAX, "Speed: %.2f");
            ImGui::Separator();

            ImGui::Checkbox("Practice Fix", &replay.practice_fix);
            ImGui::SameLine();

            ImGui::Checkbox("Accuracy Fix", &replay.accuracy_fix);

            if (replay.accuracy_fix)
            {
                ImGui::SameLine();
                ImGui::Checkbox("Disable Rotation Fix", &replay.disable_rotationfix);
            }

            ImGui::Separator();

            ImGui::Checkbox("FPS Bypass", &framerate::enabled);
            ImGui::SameLine();
            ImGui::Checkbox("FPS Multiplier", &framerate::enabled_fps);
            ImGui::SameLine();
            ImGui::Checkbox("Orb Fix", &practiceFix.orb_fix);

            ImGui::Separator();

            ImGui::Checkbox("Ignore Inputs on Playing", &replay.ignore_input);
            ImGui::SameLine();
            ImGui::Checkbox("Real Time", &replay.real_time);
            ImGui::SameLine();
            ImGui::Checkbox("Meta", &meta);
            ImGui::Separator();
            if (ImGui::Button("Show Cursor Fix"))
            {
                gd::GameManager::sharedState()->setGameVariable("0024", true);
                cocos2d::CCEGLView::sharedOpenGLView()->showCursor(true);
            }
            ImGui::SameLine();
            if (ImGui::Button("Show Restart Button"))
            {
                gd::GameManager::sharedState()->setGameVariable("0074", true);
            }
            ImGui::SameLine();
            if (ImGui::Button("Hide Practice Buttons"))
            {
                gd::GameManager::sharedState()->setGameVariable("0071", true);
            }
            ImGui::Separator();
            ImGui::Text("Frame: %i\nReplay Size: %i", replay.get_frame(), replay.replay2.size());

            ImGui::PushItemWidth(135.f);
            ImGui::End();
        }
        else if (items_index == 1)
        {
            ImGui::Checkbox("Frame Advance", &frameAdvance.enabled);

            ImGui::Separator();

            if (ImGui::Checkbox("Spambot", &spamBot.enabled))
            {
                spamBot.reset_temp();
                auto pl = gd::GameManager::sharedState()->getPlayLayer();
                if (pl)
                {
                    if (!spamBot.enabled)
                    {
                        hooks::playLayer_releaseButtonHook(pl, 0, 0, true);
                        hooks::playLayer_releaseButtonHook(pl, 0, 0, false);
                    }
                }
            }

            ImGui::SameLine();
            ImGui::PushItemWidth(100.f);
            ImGui::DragInt("##spamhold", &spamBot.hold, 1, 1, INT_MAX, "Hold: %i");

            ImGui::SameLine();
            ImGui::PushItemWidth(100.f);
            ImGui::DragInt("##spamrelease", &spamBot.release, 1, 1, INT_MAX, "Release: %i");

            ImGui::Checkbox("Player 1", &spamBot.player_p1);
            ImGui::SameLine();
            ImGui::Checkbox("Player 2", &spamBot.player_p2);

            ImGui::Separator();

            ImGui::Checkbox("Dual Clicks", &replay.dual_clicks);

            ImGui::Separator();

            if (ImGui::Checkbox("Straight Fly Bot", &straightFly.enabled))
            {
                auto pl = gd::GameManager::sharedState()->getPlayLayer();
                straightFly.start(pl);
                if (pl)
                {
                    if (!straightFly.enabled)
                    {
                        hooks::playLayer_releaseButtonHook(pl, 0, 0, true);
                        hooks::playLayer_releaseButtonHook(pl, 0, 0, false);
                    }
                }
            }

            ImGui::SameLine();
            ImGui::DragInt("##StraightFlyAcc", &straightFly.accuracy, 1, 0, 100, "Y Accuracy: %i");
            ImGui::Text("Note: Straight Fly Bot works only on first player");

            ImGui::Separator();

            ImGui::Text("Noclip");
            ImGui::Checkbox("Player 1##2", &hacks::noclipP1);
            ImGui::SameLine();
            ImGui::Checkbox("Player 2##2", &hacks::noclipP2);

            ImGui::Separator();
            if (ImGui::Checkbox("Disable achievements", &hacks::disable_achievements))
            {
                hacks::disable_achievements_f(hacks::disable_achievements);
            }
            // ImGui::Separator();
            // if (ImGui::Button("Crash GD")) {
            //     exit(-1);
            // }
        }
        else if (items_index == 2)
        {
            hacks::render();
        }
        else if (items_index == 3)
        {
            auto size = ImGui::GetWindowSize();
            auto pos = ImGui::GetWindowPos();
            ImGui::SetNextWindowPos({size.x + pos.x + 20, pos.y - 8});
            ImGui::SetNextWindowSize({185, (float)(itemsEditor_index == 0 ? 180 : 250)});
            ImGui::Begin("Editor", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

            if (itemsEditor_index == 0)
            {
                if (!replay.replay2.empty())
                {
                    if (ImGui::Button("Add Action") && editor_indexInputs >= 0 && editor_indexInputs <= int(replay.replay2.size()))
                    {
                        if (!replay.replay2.empty() || editor_indexInputs != replay.replay2.size())
                        {
                            replay.replay2.insert(replay.replay2.begin() + editor_indexInputs + 1, {(unsigned)editor_indexInputs + 1, false, true});
                            editor_indexInputs++;
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Remove Action") && editor_indexInputs >= 0 && editor_indexInputs < int(replay.replay2.size()))
                    {
                        replay.replay2.erase(replay.replay2.begin() + editor_indexInputs);
                        if (editor_indexInputs > 0)
                        {
                            editor_indexInputs--;
                        }
                    }

                    if (ImGui::Button("Move Up", {75, NULL}) && editor_indexInputs > 0)
                    {
                        swap(replay.replay2[editor_indexInputs], replay.replay2[editor_indexInputs - 1]);
                        editor_indexInputs = (editor_indexInputs - 1 >= 0) ? (editor_indexInputs - 1) : 0;
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Move Down", {90, NULL}) && editor_indexInputs < int(replay.replay2.size()) - 1)
                    {
                        swap(replay.replay2[editor_indexInputs], replay.replay2[editor_indexInputs + 1]);
                        editor_indexInputs = (editor_indexInputs + 1 < int(replay.replay2.size())) ? (editor_indexInputs + 1) : (int(replay.replay2.size()) - 1);
                    }

                    ImGui::Separator();
                    int frame = replay.replay2[editor_indexInputs].frame;
                    ImGui::PushItemWidth(165.f);
                    if (ImGui::DragInt("##frame_editor", &frame, 1, 0, INT_MAX, "Frame %i"))
                    {
                        replay.replay2[editor_indexInputs].frame = frame;
                    }

                    ImGui::Checkbox(replay.replay2[editor_indexInputs].hold ? "Push" : "Release", &replay.replay2[editor_indexInputs].hold);
                    ImGui::Checkbox(replay.replay2[editor_indexInputs].player ? "First" : "Second", &replay.replay2[editor_indexInputs].player);
                }
                else
                {
                    ImGui::Text("No Actions!");
                    if (ImGui::Button("Add action"))
                    {
                        replay.replay2.push_back({0, false, true});
                    }
                }
            }
            else if (itemsEditor_index == 1)
            {
                if (!replay.replay.empty())
                {
                    if (ImGui::Button("Add Action") && editor_indexPhysics >= 0 && editor_indexPhysics <= int(replay.replay.size()))
                    {
                        if (!replay.replay.empty() || editor_indexPhysics != replay.replay.size())
                        {
                            replay.replay.insert(replay.replay.begin() + editor_indexPhysics + 1, {(unsigned)editor_indexPhysics + 1, 0, 0, 0, 0, true});
                            editor_indexPhysics++;
                        }
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Remove Action") && editor_indexPhysics >= 0 && editor_indexPhysics < int(replay.replay.size()))
                    {
                        replay.replay.erase(replay.replay.begin() + editor_indexPhysics);
                        if (editor_indexPhysics > 0)
                        {
                            editor_indexPhysics--;
                        }
                    }

                    if (ImGui::Button("Move Up", {75, NULL}) && editor_indexPhysics > 0)
                    {
                        swap(replay.replay[editor_indexPhysics], replay.replay[editor_indexPhysics - 1]);
                        editor_indexPhysics = (editor_indexPhysics - 1 >= 0) ? (editor_indexPhysics - 1) : 0;
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Move Down", {90, NULL}) && editor_indexPhysics < int(replay.replay.size()) - 1)
                    {
                        swap(replay.replay[editor_indexPhysics], replay.replay[editor_indexPhysics + 1]);
                        editor_indexPhysics = (editor_indexPhysics + 1 < int(replay.replay.size())) ? (editor_indexPhysics + 1) : (int(replay.replay.size()) - 1);
                    }

                    ImGui::Separator();
                    int frame = replay.replay[editor_indexPhysics].frame;
                    ImGui::PushItemWidth(165.f);
                    if (ImGui::DragInt("##frame_editor", &frame, 1, 0, INT_MAX, "Frame %i"))
                    {
                        replay.replay[editor_indexPhysics].frame = frame;
                    }

                    ImGui::DragFloat("##x_editor", &replay.replay[editor_indexPhysics].x, 0.000010f, 0, FLT_MAX, "X Position %f");
                    ImGui::DragFloat("##y_editor", &replay.replay[editor_indexPhysics].y, 0.000010f, 0, FLT_MAX, "Y Position %f");
                    ImGui::DragFloat("##rotation_editor", &replay.replay[editor_indexPhysics].rotation, 0.000010f, 0, FLT_MAX, "Rotation %f");

                    float y_accel = static_cast<float>(replay.replay[editor_indexPhysics].y_accel);
                    if (ImGui::DragFloat("##yaccel_editor", &y_accel, 0.000010f, 0, FLT_MAX, "Y Accel: %f"))
                    {
                        replay.replay[editor_indexPhysics].y_accel = y_accel;
                    }

                    ImGui::Checkbox(replay.replay[editor_indexPhysics].player ? "First" : "Second", &replay.replay[editor_indexPhysics].player);
                }
                else
                {
                    ImGui::Text("No Actions!");
                    if (ImGui::Button("Add action"))
                    {
                        replay.replay.push_back({0, 0, 0, 0, 0, true});
                    }
                }
            }

            ImGui::End();

            if (itemsEditor_index == 0)
            {
                if (ImGui::BeginTable("##editor_table", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | 2))
                {
                    ImGui::TableNextColumn();
                    ImGui::TableHeader("Frame");
                    ImGui::TableNextColumn();
                    ImGui::TableHeader("Player");
                    ImGui::TableNextColumn();
                    ImGui::TableHeader("Action");
                    for (size_t i = 0; i < replay.replay2.size(); i++)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        bool is_selected = (editor_indexInputs == i);
                        string frame = to_string(replay.replay2[i].frame) + "##" + to_string(i);
                        if (ImGui::Selectable(frame.c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns))
                        {
                            editor_indexInputs = i;
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text(replay.replay2[i].player ? "First" : "Second");
                        ImGui::TableNextColumn();
                        ImGui::Text(replay.replay2[i].hold ? "Push" : "Release");
                    }
                    ImGui::EndTable();
                }
            }
            if (itemsEditor_index == 1)
            {
                if (ImGui::BeginTable("##editor_table", 6, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | ImGuiWindowFlags_NoResize))
                {
                    ImGui::TableNextColumn();
                    ImGui::TableHeader("Frame");
                    ImGui::TableNextColumn();
                    ImGui::TableHeader("Position X");
                    ImGui::TableNextColumn();
                    ImGui::TableHeader("Position Y");
                    ImGui::TableNextColumn();
                    ImGui::TableHeader("Rotation");
                    ImGui::TableNextColumn();
                    ImGui::TableHeader("Y Accel");
                    ImGui::TableNextColumn();
                    ImGui::TableHeader("Player");
                    for (size_t i = 0; i < replay.replay.size(); i++)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        bool is_selected = (editor_indexPhysics == i);
                        string frame = to_string(replay.replay[i].frame) + "##" + to_string(i);
                        if (ImGui::Selectable(frame.c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns))
                        {
                            editor_indexPhysics = i;
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text("%f", replay.replay[i].x);
                        ImGui::TableNextColumn();
                        ImGui::Text("%f", replay.replay[i].y);
                        ImGui::TableNextColumn();
                        ImGui::Text("%f", replay.replay[i].rotation);
                        ImGui::TableNextColumn();
                        ImGui::Text("%f", replay.replay[i].y_accel);
                        ImGui::TableNextColumn();
                        ImGui::Text(replay.replay[i].player ? "First" : "Second");
                    }
                    ImGui::EndTable();
                }
            }
        }
        else if (items_index == 4)
        {
            auto pl = gd::GameManager::sharedState()->getPlayLayer();
            if (!pl)
            {
                ImGui::Text("Enter the level to record it");
            }
            else
            {
                ImGui::Text("Action");
                ImGui::Separator();

                static bool isRecording = false;
                if (ImGui::Checkbox("Record", &isRecording))
                {
                    if (filesystem::exists("ffmpeg.exe"))
                    {
                        Console::Init();
                        if (isRecording)
                        {
                            recorder.start("ReplayEngine/Videos/" + string(recorder.video_name));
                        }
                        else
                        {
                            recorder.stop();
                        }
                    }
                    else
                    {
                        gd::FLAlertLayer::create(nullptr, "Info", "Ok", nullptr, "ffmpeg.exe doesn't exits")->show();
                        isRecording = false;
                    }
                }

                ImGui::SameLine();

                ImGui::Checkbox("Render Until the End", &recorder.m_until_end);
                ImGui::SameLine();

                ImGui::Checkbox("Include Audio", &recorder.m_include_audio);
                ImGui::PushItemWidth(260);
                ImGui::InputText("##replay_name", recorder.video_name, IM_ARRAYSIZE(recorder.video_name));

                ImGui::Spacing();

                ImGui::Text("Resolution:");
                ImGui::Separator();

                ImGui::PushItemWidth(40);
                ImGui::InputInt("##width", &recorder.m_width, 0);
                ImGui::SameLine(0, 5);

                ImGui::Text("x");
                ImGui::SameLine(0, 5);

                ImGui::PushItemWidth(40);
                ImGui::InputInt("##height", &recorder.m_height, 0);
                ImGui::SameLine(0, 5);

                ImGui::Text("@");
                ImGui::SameLine(0, 5);

                ImGui::PushItemWidth(30);
                ImGui::InputInt("##fps", &recorder.m_fps, 0);

                ImGui::Spacing();

                ImGui::Text("Encoding Settings");
                ImGui::Separator();

                char bitrate[128];
                strcpy_s(bitrate, recorder.m_bitrate.c_str());
                ImGui::PushItemWidth(50);
                if (ImGui::InputText("Bitrate", bitrate, sizeof(bitrate)))
                {
                    recorder.m_bitrate = string(bitrate);
                }

                ImGui::SameLine();

                char codec[128];
                strcpy_s(codec, recorder.m_codec.c_str());
                ImGui::PushItemWidth(80);
                if (ImGui::InputText("Codec", codec, sizeof(codec)))
                {
                    recorder.m_codec = string(codec);
                }

                char extra_args[512];
                strcpy_s(extra_args, recorder.m_extra_args.c_str());
                ImGui::PushItemWidth(260);
                if (ImGui::InputText("Extra Arguments", extra_args, sizeof(extra_args)))
                {
                    recorder.m_extra_args = string(extra_args);
                }

                char extra_args_audio[512];
                strcpy_s(extra_args_audio, recorder.m_extra_audio_args.c_str());
                ImGui::PushItemWidth(260);
                if (ImGui::InputText("Extra Arguments (Audio)", extra_args_audio, sizeof(extra_args_audio)))
                {
                    recorder.m_extra_audio_args = string(extra_args_audio);
                }

                ImGui::Spacing();

                ImGui::Text("Level Settings");
                ImGui::Separator();

                ImGui::InputFloat("Second to Render After", &recorder.m_after_end_duration, 1);

                ImGui::Spacing();

                ImGui::Text("Presets");
                ImGui::Separator();

                if (ImGui::Button("HD"))
                {
                    recorder.m_width = 1280;
                    recorder.m_height = 720;
                    recorder.m_fps = 60;
                    recorder.m_bitrate = "15M";
                }

                ImGui::SameLine();

                if (ImGui::Button("FULL HD"))
                {
                    recorder.m_width = 1920;
                    recorder.m_height = 1080;
                    recorder.m_fps = 60;
                    recorder.m_bitrate = "50M";
                }

                ImGui::SameLine();

                if (ImGui::Button("4K"))
                {
                    recorder.m_width = 3840;
                    recorder.m_height = 2160;
                    recorder.m_fps = 60;
                    recorder.m_bitrate = "70M";
                }
            }
        }
        else if (items_index == 5)
        {
            ImGui::Combo("##ConverterType", &converter.converterType, converterTypes, IM_ARRAYSIZE(converterTypes));

            ImGui::InputText(".txt##Replay Name", converter.replay_name, sizeof(converter.replay_name));

            if (ImGui::Button("Convert"))
            {
                if (strlen(converter.replay_name) == 0)
                {
                    ImGui::Text("Please enter a replay name.");
                }
                else
                {
                    converter.convert();
                }
            }

            ImGui::SameLine();

            bool import_hovered = false;
            if (ImGui::Button("Import"))
            {
                if (strlen(converter.replay_name) != 0)
                {
                    converter.import();
                }
            }
            if (ImGui::IsItemHovered())
                import_hovered = true;

            ImGui::SameLine();

            if (ImGui::Button("Matcool Converter"))
            {
                ShellExecuteA(0, "open", "https://matcool.github.io/gd-macro-converter/", 0, 0, SW_SHOWNORMAL);
            }

            if (converter.converterType == 0)
            {
                if (strlen(converter.replay_name) == 0)
                {
                    ImGui::Text("Please enter replay name");
                }
                else
                {
                    if (!import_hovered)
                        ImGui::Text("Replay will be saved to \"ReplayEngine/Converter/%s.txt\"", converter.replay_name);
                    else
                        ImGui::Text("Replay will be converted to Replay Engine Replay");
                }
            }

            float windowHeight = ImGui::GetWindowSize().y;
            float textHeight = ImGui::GetTextLineHeight();
            float textPosY = windowHeight - textHeight * 3;
            ImGui::SetCursorPosY(textPosY);
            ImGui::Text("I'm too lazy to make conversions for every bot because my code\nwould become shitcoded\nInstead you can use Mat's converter to convert plain text to another replay");
        }
        else if (items_index == 6)
        {
            ImGui::Checkbox("Toggle", &sequence.enable_sqp);
            ImGui::SameLine();
            ImGui::Checkbox("Random", &sequence.random_sqp);
            ImGui::InputText("##sqp_replay", sequence.replay_sq_name, IM_ARRAYSIZE(sequence.replay_sq_name));
            ImGui::SameLine();
            if (ImGui::Button("Add"))
            {
                string path = "ReplayEngine/Replays/" + (string)sequence.replay_sq_name + ".re";
                if (filesystem::exists(path))
                {
                    sequence.replays.push_back(sequence.replay_sq_name);
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Remove"))
            {
                if (sequence.replays.size() > (size_t)sequence.current_idx)
                {
                    sequence.replays.erase(sequence.replays.begin() + sequence.current_idx);
                    sequence.first_sqp = true;
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Remove All"))
            {
                sequence.replays.clear();
                sequence.first_sqp = true;
            }

            ImGui::BeginChild("##sequence_window", {NULL, NULL}, true);
            for (size_t n = 0; n < sequence.replays.size(); n++)
            {
                bool is_selected = (sequence.current_idx == n);
                string anticonflict = sequence.replays[n] + "##" + to_string(n);
                if (ImGui::Selectable(anticonflict.c_str(), is_selected))
                    sequence.current_idx = n;
            }
            ImGui::EndChild();
        }
        else if (items_index == 7)
        {
            static vector<string> clicks;
            static vector<string> videos;
            static int idx_clicks = 0;
            static int idx_videos = 0;
            static char output_clicks[128];
            static bool first = true;
            static float audio_volume = 0.25;
            static float clicks_volume = 5;
            static bool soft_clicks_e = true;
            static bool hard_clicks_e = false;
            static int soft_clicks = 200;
            static int hard_clicks = 500;

            auto updateClicksAndVideos = [&]()
            {
                clicks.clear();
                videos.clear();

                for (const auto &entry : filesystem::directory_iterator("ReplayEngine\\Clicks"))
                {
                    if (filesystem::is_directory(entry))
                    {
                        clicks.push_back(entry.path().string());
                    }
                }

                for (const auto &entry : filesystem::directory_iterator("ReplayEngine\\Videos"))
                {
                    if (!filesystem::is_directory(entry))
                    {
                        videos.push_back(entry.path().filename().string());
                    }
                }
            };

            if (first)
            {
                first = false;
                updateClicksAndVideos();
            }

            if (clicks.empty())
            {
                ImGui::Text("No clickpacks! \"ReplayEngine/Clicks\" empty");
                if (ImGui::Button("Update"))
                {
                    updateClicksAndVideos();
                }
            }
            else
            {
                if (ImGui::BeginTabBar("Clickbot"))
                {
                    if (ImGui::BeginTabItem("Render##Tab"))
                    {
                        ImGui::BeginChild("##clickpack", {0, 200}, true);
                        for (size_t n = 0; n < clicks.size(); n++)
                        {
                            bool is_selected = (idx_clicks == n);
                            string anticonflict = clicks[n] + "##" + to_string(n);
                            if (ImGui::Selectable(anticonflict.c_str(), is_selected))
                                idx_clicks = n;
                        }
                        ImGui::EndChild();

                        if (ImGui::Button("Update"))
                        {
                            updateClicksAndVideos();
                        }
                        ImGui::SameLine();
                        ImGui::InputText(".mp3", output_clicks, IM_ARRAYSIZE(output_clicks));
                        ImGui::SameLine();
                        if (ImGui::Button("Render"))
                        {
                            if (!filesystem::exists("clicks.exe"))
                            {
                                gd::FLAlertLayer::create(nullptr, "Info", "Ok", nullptr, "clicks.exe doesn't exist")->show();
                            }
                            else
                            {
                                Console::Init();
                                if (strlen(output_clicks) != 0)
                                {
                                    if (replay.save("converted") != "Replay doesn't have actions")
                                    {
                                        std::thread([&]()
                                                    {
                                            std::stringstream stream;
                                            stream << "\"clicks\" -r\"ReplayEngine\\Replays\\converted.re\" -c\"" <<
                                                clicks[idx_clicks] << "\" ";

                                            if (soft_clicks_e) {
                                                stream << "-softc" << soft_clicks << " ";
                                            }

                                            if (hard_clicks_e) {
                                                stream << "-hardc" << hard_clicks << " ";
                                            }

                                            stream << "-end3 ";

                                            stream << "-o\"ReplayEngine\\Clicks\\" << static_cast<string>(output_clicks) << ".mp3\"";

                                            auto process = subprocess::Popen(stream.str());
                                            if (process.close() != 0) {
                                                Console::WriteLine("Clicks render went wrong :(");
                                            } })
                                            .detach();
                                    }
                                    else
                                    {
                                        Console::WriteLine("Replay doesn't have actions");
                                    }
                                }
                                else
                                {
                                    Console::WriteLine("Output filename is empty");
                                }
                            }
                        }

                        ImGui::Checkbox("Softclicks", &soft_clicks_e);
                        ImGui::SameLine();
                        ImGui::PushItemWidth(120.f);
                        ImGui::DragInt("##softclicks", &soft_clicks, 1, 0, INT_MAX, "ms: %i");

                        ImGui::Checkbox("Hardclicks", &hard_clicks_e);
                        ImGui::SameLine();
                        ImGui::PushItemWidth(120.f);
                        ImGui::DragInt("##hardclicks", &hard_clicks, 1, 0, INT_MAX, "ms: %i");

                        float windowHeight = ImGui::GetWindowSize().y;
                        float textHeight = ImGui::GetTextLineHeight();
                        float textPosY = windowHeight - textHeight;
                        ImGui::SetCursorPosY(textPosY);
                        ImGui::Text("All outputs will be saved to \"ReplayEngine/Clicks\" folder");
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Merge##Tab"))
                    {
                        ImGui::BeginChild("##videos", {0, 200}, true);
                        for (size_t n = 0; n < videos.size(); n++)
                        {
                            bool is_selected = (idx_videos == n);
                            string anticonflict = videos[n] + "##" + to_string(n);
                            if (ImGui::Selectable(anticonflict.c_str(), is_selected))
                                idx_videos = n;
                        }
                        ImGui::EndChild();

                        if (ImGui::Button("Update"))
                        {
                            updateClicksAndVideos();
                        }
                        ImGui::SameLine();
                        ImGui::InputText(".mp3", output_clicks, IM_ARRAYSIZE(output_clicks));
                        ImGui::SameLine();
                        if (ImGui::Button("Merge"))
                        {
                            if (!filesystem::exists("ffmpeg.exe"))
                            {
                                gd::FLAlertLayer::create(nullptr, "Info", "Ok", nullptr, "ffmpeg.exe doesn't exist")->show();
                            }
                            else
                            {
                                Console::Init();
                                std::thread([&]()
                                            {
                                    string ffmpeg_command = "ffmpeg -i \"ReplayEngine/Videos/" + videos[idx_videos] +
                                        "\" -i \"ReplayEngine/Clicks/" + static_cast<string>(output_clicks) +
                                        ".mp3\" -filter_complex \"[0:a]volume=" + to_string(audio_volume) +
                                        "[video_audio];[1:a]volume=" + to_string(clicks_volume) +
                                        "[clicks_audio];[video_audio][clicks_audio]amix=inputs=2:duration=longest[a]\" " +
                                        "-map 0:v -map \"[a]\" -c:v copy -c:a aac -strict experimental -y \"ReplayEngine/Videos/clicks_" + videos[idx_videos] + "\"";

                                    auto process = subprocess::Popen(ffmpeg_command);
                                    if (process.close()) {
                                        Console::WriteLine("R.I.P. FFmpeg");
                                    }
                                    else {
                                        Console::WriteLine("Audio combine should be done");
                                    } })
                                    .detach();
                            }
                        }
                        ImGui::DragFloat("##audio_volume", &audio_volume, 0.01f, 0, FLT_MAX, "Audio Volume %.2f");
                        ImGui::DragFloat("##clicks_volume", &clicks_volume, 0.01f, 0, FLT_MAX, "Clicks Volume %.2f");

                        float windowHeight = ImGui::GetWindowSize().y;
                        float textHeight = ImGui::GetTextLineHeight();
                        float textPosY = windowHeight - textHeight;
                        ImGui::SetCursorPosY(textPosY);
                        ImGui::Text("Video will be saved to \"ReplayEngine/Videos/clicks_%s\"", videos[idx_videos].c_str());
                        ImGui::EndTabItem();
                    }

                    ImGui::EndTabBar();
                }
            }
        }
        else if (items_index == 8)
        {
            ImGui::Text("About");
            ImGui::Separator();
            ImGui::Text("Replay Engine Beta 3 by TobyAdd");
            if (ImGui::MenuItem("Graphical interface is made using ImGui"))
                ShellExecuteA(NULL, "open", "https://github.com/ocornut/imgui", NULL, NULL, SW_SHOWNORMAL);

            ImGui::Text("Made in ");
            ImGui::SameLine();
            ImVec2 flagSize(ImGui::GetTextLineHeight() * 1.5f, ImGui::GetTextLineHeight() * 1.125f);
            ImVec2 startPos = ImGui::GetCursorScreenPos();
            ImVec2 endPos = ImVec2(startPos.x + flagSize.x, startPos.y + flagSize.y);
            ImDrawList *drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(startPos, endPos, IM_COL32(255, 215, 0, 255));
            float stripeHeight = flagSize.y / 2.0f;
            drawList->AddRectFilled(ImVec2(startPos.x, startPos.y), ImVec2(endPos.x, startPos.y + stripeHeight), IM_COL32(0, 87, 183, 255));

            ImGui::Spacing();
            ImGui::Text("Keybinds");
            ImGui::Separator();
            ImGui::Text("K - Toggle UI");
            ImGui::Text("C - Enable Frame Advance + Next Frame");
            ImGui::Text("V - Disable Frame Advance");
            ImGui::Text("P - Toggle Playback/Disable");
            ImGui::Spacing();
            ImGui::Text("Special Thanks");
            ImGui::Separator();
            ImGui::Text("FireMario211 - For the GD Mod example");
            ImGui::Text("HJfod - Help with some coding stuff");
            ImGui::Text("Mat - Internal Recorder");
            ImGui::Text("Absolute - Hacks");
            ImGui::Text("Acid - Clickbot");
        }

        ImGui::EndChild();
        ImGui::End();
    }
}

void gui::Toggle()
{
    gui::show = !gui::show;
    opennedSP = false;
}