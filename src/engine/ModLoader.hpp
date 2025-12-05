#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#define MOD_LOADER_VERSION "0.2.1"

namespace io {
    class path;
}

class Engine;

namespace modding 
{
	using ModInitFunc = void (*)(Engine&);
	using ModShutdownFunc = void (*)();

	using OnWorldOpenFunc = void (*)(Level*, int64_t localPlayer);
	using OnWorldClosedFunc = void (*)();
	using OnContentLoadFunc = void (*)();
	using OnEngineShutdownFunc = void (*)();

	struct ModInfo {
		std::string name;
		std::string version;
		std::string author;
		std::string description;
	};

	class LoadedMod 
	{
		void* handle = nullptr;
		ModInfo info;
		bool initialized = false;

	public:
		ModInitFunc initFunc = nullptr;
		ModShutdownFunc shutdownFunc = nullptr;
		
		OnWorldOpenFunc onWorldOpen = nullptr;
		OnWorldClosedFunc onWorldClosed = nullptr;
		OnContentLoadFunc onContentLoad = nullptr;
		OnEngineShutdownFunc onEngineShutdown = nullptr;

		LoadedMod(void* handle, ModInfo info);
		~LoadedMod();

		LoadedMod(const LoadedMod&) = delete;
		LoadedMod& operator=(const LoadedMod&) = delete;
		LoadedMod(LoadedMod&&) noexcept;
		LoadedMod& operator=(LoadedMod&&) noexcept;

		bool initialize(Engine& engine);
		void shutdown();
		
		const ModInfo& getInfo() const { return info; }
		bool isInitialized() const { return initialized; }
	};

	class ModLoader 
	{
		std::vector<std::unique_ptr<LoadedMod>> loadedMods;
		
		static void* loadLibrary(const std::string& path);
		static void* getFunction(void* handle, const std::string& name);
		static void unloadLibrary(void* handle);
		static std::string getLibraryExtension();
		
	public:
		ModLoader() = default;
		~ModLoader();

		void create_version_label(gui::GUI& gui);

		/// @brief Load all mods from the specified directory
		/// @param modsPath Path to the mods directory
		/// @param engine Engine instance to pass to mod initialization
		/// @return Number of successfully loaded mods
		size_t loadMods(const io::path& modsPath, Engine& engine);
		
		/// @brief Load a single mod library
		/// @param modPath Path to the mod library file
		/// @param engine Engine instance to pass to mod initialization
		/// @return true if loaded successfully
		bool loadMod(const io::path& modPath, Engine& engine);
		
		/// @brief Shutdown and unload all mods
		void unloadAll();
		
		/// @brief Get list of loaded mods
		const std::vector<std::unique_ptr<LoadedMod>>& getMods() const {
			return loadedMods;
		}
		
		/// @brief Get number of loaded mods
		size_t getModCount() const { return loadedMods.size(); }

		/// @brief Broadcast world events to all mods
		/// @param level 
		/// @param localPlayer 
		void broadcastWorldOpen(Level* level, int64_t localPlayer);
		void broadcastWorldClosed();
		void broadcastContentLoad();
		void broadcastEngineShutdown();
	};
}