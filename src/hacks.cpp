#include "hacks.h"
#include <fstream>
#include <streambuf>

json hacksContent;

namespace hacks
{
    void render()
    {
        if (ImGui::BeginTabBar("Hacks")) {
            for (auto& item : hacksContent.items()) {
                if (ImGui::BeginTabItem(item.key().c_str())) {
                    ImGui::BeginChild("HacksChild");

                    json& tabContent = item.value();

                    for (size_t i = 0; i < tabContent.size(); i++) {
                        json& itemHack = tabContent.at(i);
                        bool enabled = itemHack["enabled"];

                        if (ImGui::Checkbox(itemHack["name"].get<string>().c_str(), &enabled)) {
                            itemHack["enabled"] = enabled;

                            json opcodes = itemHack["opcodes"];
                            for (auto& opcode : opcodes) {
                                string addrStr = opcode["addr"];
                                string bytesStr = enabled ? opcode["on"] : opcode["off"];

                                uintptr_t address;
                                sscanf_s(addrStr.c_str(), "%x", &address);

                                DWORD base = gd::base;
                                if (!opcode["lib"].is_null() && string(opcode["lib"]) == "libcocos2d.dll") {
                                    base = (DWORD)GetModuleHandleA("libcocos2d.dll");
                                }

                                writemem(base + address, bytesStr);
                            }

                            ofstream outputFile("ReplayEngine/hacks.json");
                            outputFile << hacksContent.dump(4);
                            outputFile.close();
                        }

                        if (ImGui::IsItemHovered())
                            ImGui::SetTooltip(itemHack["desc"].get<string>().c_str());
                    }

                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
            }

            ImGui::EndTabBar();
        }
    }

    void load() {
        ifstream file("ReplayEngine/hacks.json");
        if (!file.is_open())
            return;

        string file_content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();

        hacksContent = json::parse(file_content);

        for (auto item : hacksContent.items()) {
            json tabContent = item.value();
            for (size_t i = 0; i < tabContent.size(); i++) {
                json itemHack = tabContent.at(i);
                if (itemHack["enabled"]) {
                    json opcodes = itemHack["opcodes"];
                    for (int j = 0; j < (int)opcodes.size(); j++) {
                        json opcode = opcodes.at(j);
                        uintptr_t address;
                        sscanf_s(opcode["addr"].get<string>().c_str(), "%x", &address);
                        DWORD base = gd::base;
                        if (!opcode["lib"].is_null() && string(opcode["lib"]) == "libcocos2d.dll") {
                            base = (DWORD)GetModuleHandleA("libcocos2d.dll");
                        }

                        writemem(base + address, opcode["on"].get<string>());
                    }
                }
            }
        }

    }

    bool writemem(uintptr_t address, std::string bytes) {
        std::vector<unsigned char> byteVec;
        std::stringstream byteStream(bytes);
        std::string byteStr;

        while (getline(byteStream, byteStr, ' ')) {
            unsigned int byte = std::stoul(byteStr, nullptr, 16);
            byteVec.push_back(static_cast<unsigned char>(byte));
        }

        DWORD oldProtect;
        if (VirtualProtect(reinterpret_cast<void*>(address), byteVec.size(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
            memcpy(reinterpret_cast<void*>(address), byteVec.data(), byteVec.size());
            VirtualProtect(reinterpret_cast<void*>(address), byteVec.size(), oldProtect, &oldProtect);
            return true;
        }
        else {
            return false;
        }
    }
}
