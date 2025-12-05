# <img width="128" height="128" alt="vcml" src="https://github.com/user-attachments/assets/0c8d62ce-6e79-409a-9016-905f514c04d1" /> VoxelCore Mod Loader

This is a simple mod loader for VoxelCore

You can create your own C++ native mods for this game engine

## Quick Start

1. **Download the latest release** from the [Releases](https://github.com/your-username/voxelcore-mod-loader/releases) page.
2. Extract the contents into your VoxelCore installation folder.
3. Place your compiled `.dll` (Windows) or `.so` (Linux) mod files into the `mods/` directory.
4. Launch the game — mods will be loaded automatically!

## How to create my own mod?

[Use this example for start] (https://github.com/VoxelCoreMods/ExampleMod):
```cpp
// example_mod.cpp
#include <engine/Engine.hpp>
#include <engine/ModLoader.hpp>
#include <world/Level.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
    #define MOD_EXPORT extern "C" __declspec(dllexport)
#else
    #define MOD_EXPORT extern "C" __attribute__((visibility("default")))
#endif

MOD_EXPORT modding::ModInfo mod_get_info()
{
    return modding::ModInfo{
        .name = "ExampleMod",
        .version = "1.0.0",
        .author = "VoxelCoreMods",
        .description = "This is an example mod for VoxelCore ModLoader"
    };
}

MOD_EXPORT void mod_initialize(Engine& engine)
{
    std::cout << "[ExampleMod] Initializing...\n";
}

MOD_EXPORT void mod_shutdown() 
{
    std::cout << "[ExampleMod] Shutdown complete.\n";
}

MOD_EXPORT void mod_on_world_open(Level* level, int64_t localPlayer) 
{
    std::cout << "[ExampleMod] World opened. Level pointer: " << level 
              << ", Local Player ID: " << localPlayer << "\n";
}

MOD_EXPORT void mod_on_world_closed() 
{
    std::cout << "[ExampleMod] World closed.\n";
}

MOD_EXPORT void mod_on_content_load() 
{
    std::cout << "[ExampleMod] Content loaded.\n";
}

MOD_EXPORT void mod_on_engine_shutdown() 
{
    std::cout << "[ExampleMod] Engine is shutting down.\n";
}
```

All mod examples here: https://github.com/orgs/VoxelCoreMods/repositories