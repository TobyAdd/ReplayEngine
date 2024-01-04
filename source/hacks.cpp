#include "hacks.hpp"
#include "hooks.hpp"
#include <filesystem>

json hacks::content;

void CheckDir(string dir)
{
    if (!filesystem::is_directory(dir) || !filesystem::exists(dir))
    {
        filesystem::create_directory(dir);
    }
}

void hacks::load()
{
    CheckDir("ReplayEngine");
    CheckDir("ReplayEngine/extensions");
    CheckDir("ReplayEngine/macros");
    ifstream file("ReplayEngine/settings.json");
    if (!file.is_open())
        return;
    string file_content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    hacks::content = json::parse(file_content);
}

bool hacks::push_write(const uintptr_t address, const DWORD destination_address)
{
    DWORD oldProtect, newProtect;

    if (VirtualProtect(reinterpret_cast<LPVOID>(address), 4, PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        if (WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<LPVOID>(address), &destination_address, 4, nullptr))
        {
            return VirtualProtect(reinterpret_cast<LPVOID>(address), 4, oldProtect, &newProtect);
        }
    }

    return false;
}

bool hacks::writemem(uintptr_t address, std::string bytes)
{
    std::vector<unsigned char> byteVec;
    std::stringstream byteStream(bytes);
    std::string byteStr; 

    while (getline(byteStream, byteStr, ' '))
    {
        unsigned int byte = std::stoul(byteStr, nullptr, 16);
        byteVec.push_back(static_cast<unsigned char>(byte));
    }

    DWORD oldProtect;
    if (VirtualProtect(reinterpret_cast<void *>(address), byteVec.size(), PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        memcpy(reinterpret_cast<void *>(address), byteVec.data(), byteVec.size());
        VirtualProtect(reinterpret_cast<void *>(address), byteVec.size(), oldProtect, &oldProtect);
        return true;
    }
    else
    {
        return false;
    }
}

void hacks::inject_extensions() {  
    for (const auto& file : std::filesystem::directory_iterator("GDH/extensions"))
	{
        if (file.path().extension() == ".dll")
        {
		    LoadLibraryA(file.path().generic_string().c_str());
        }
	}
}