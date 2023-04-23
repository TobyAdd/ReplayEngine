#define IMGUI_DEFINE_MATH_OPERATORS
#include "gui.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui-hook.hpp>
#include <shellapi.h>
#include "replayEngine.h"
#include "hooks.h"
#include "hacks.h"

static int currentTab = 0;
bool gui::show = true;
bool gui::inited = false;
bool meta = true;

void MetaRender()
{
    if (meta)
    {
        auto pl = gd::GameManager::sharedState()->getPlayLayer();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
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
    }
}

bool opennedSP = false;
vector<string> replay_list;

void SelectReplay()
{
    auto itemRectMin = ImGui::GetItemRectMin();
    auto itemRectMax = ImGui::GetItemRectMax();
    auto itemRectSize = ImGui::GetItemRectSize();

    ImVec2 inputTextPos = itemRectMax;
    ImGui::GetWindowDrawList()->AddLine(ImVec2(inputTextPos.x, inputTextPos.y - itemRectSize.y - 1),
                                        ImVec2(inputTextPos.x, inputTextPos.y - 1),
                                        ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)));

    ImGui::SameLine(0, 0);

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
        ImGui::SetNextWindowSize(ImVec2(itemRectSize.x + ImGui::GetItemRectSize().x, NULL));
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

char *items[] = {"General", "Assist", "Hacks", "About"};
int items_index = 0;

void gui::BeginWindow(const char *title)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    ImGui::PopStyleVar();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 endPos = ImVec2(pos.x + ImGui::GetContentRegionAvailWidth(), pos.y + 4);
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImU32 startColor = ImColor(131, 96, 195, 255);
    ImU32 endColor = ImColor(46, 191, 145, 255);
    drawList->AddRectFilledMultiColor(pos, endPos, startColor, endColor, endColor, startColor);
    ImGui::Spacing();
}

void gui::Render()
{
    MetaRender();
    if (gui::show)
    {
        if (!gui::inited)
        {
            ImGui::SetNextWindowSize(ImVec2(600, 400));
            ImGui::SetNextWindowPos(ImVec2(10, 10));
            gui::inited = true;
        }
        BeginWindow("Replay Engine");
        ImGui::PushStyleColor(ImGuiCol_Border, 1);
        ImGui::BeginChild("##menu", ImGui::GetContentRegionAvail(), true);
        ImGui::PopStyleColor();

        ImGui::BeginChild("##leftside", ImVec2(150, NULL));
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
            ImGui::Text("Frame: %i\nReplay Size: %i", replay.get_frame(), replay.replay_size());

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
            ImGui::DragInt("##StraightFlyAcc", &straightFly.accuracy, 1, 0, 100, "Accuracy: %i");
            ImGui::Text("Note: Straight Fly Bot works only on first player");
        }
        else if (items_index == 2)
        {
            hacks::render();
        }
        else if (items_index == 3)
        {
            ImGui::Text("About");
            ImGui::Separator();
            ImGui::Text("Replay Engine v1.0 by TobyAdd");
            if (ImGui::MenuItem("Graphical interface is made using ImGui"))
                ShellExecute(NULL, "open", "https://github.com/ocornut/imgui", NULL, NULL, SW_SHOWNORMAL);
            ImGui::Spacing();
            ImGui::Text("Keybinds");
            ImGui::Separator();
            ImGui::Text("K - Toggle UI");
            ImGui::Text("C - Enable Frame Advance + Next Frame");
            ImGui::Text("F - Disable Frame Advance");
            ImGui::Text("S - Toggle Spam Bot");
            ImGui::Text("D - Toggle Straight Fly Bot");
            ImGui::Spacing();
            ImGui::Text("Special Thanks");
            ImGui::Separator();
            ImGui::Text("FireMario211 - For the GD Mod example");
            ImGui::Text("HJfod - Help with some coding stuff");
        }

        ImGui::EndChild();
        ImGui::End();
    }
}

void gui::Toggle()
{
    gui::show = !gui::show;
}