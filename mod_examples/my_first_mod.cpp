#include <iostream>
#include <string>

class Engine;

struct ModInfo {
    std::string name;
    std::string version;
    std::string author;
    std::string description;
};

#ifdef _WIN32
    #define MOD_EXPORT extern "C" __declspec(dllexport)
#else
    #define MOD_EXPORT extern "C" __attribute__((visibility("default")))
#endif

MOD_EXPORT ModInfo mod_get_info() {
    ModInfo info;
    info.name = "My First Mod";
    info.version = "1.0.0";
    info.author = "VoxelCoreMods";
    info.description = "My first mod!";
    return info;
}

MOD_EXPORT void mod_initialize(Engine& engine) {
    std::cout << "Hello from my first mod!" << std::endl;
}

MOD_EXPORT void mod_shutdown() {
    std::cout << "Goodbye from my first mod!" << std::endl;
}