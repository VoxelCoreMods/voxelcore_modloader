#include "ModLoader.hpp"
#include "engine/Engine.hpp"
#include "io/io.hpp"
#include "debug/Logger.hpp"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

static debug::Logger logger("modding");

namespace modding 
{
	LoadedMod::LoadedMod(void* handle, ModInfo info) 
		: handle(handle), info(std::move(info)) {}

	LoadedMod::~LoadedMod() 
	{
		if (initialized) {
			shutdown();
		}
	}

	LoadedMod::LoadedMod(LoadedMod&& other) noexcept
		: handle(other.handle), 
		info(std::move(other.info)),
		initFunc(other.initFunc),
		shutdownFunc(other.shutdownFunc),
		initialized(other.initialized) 
	{
		other.handle = nullptr;
		other.initialized = false;
	}

	LoadedMod& LoadedMod::operator=(LoadedMod&& other) noexcept 
	{
		if (this != &other) 
		{
			if (initialized) {
				shutdown();
			}

			handle = other.handle;

			info = std::move(other.info);

			initFunc = other.initFunc;
			shutdownFunc = other.shutdownFunc;

			initialized = other.initialized;

			other.handle = nullptr;
			other.initialized = false;
		}

		return *this;
	}

	bool LoadedMod::initialize(Engine& engine) 
	{
		if (initialized) {
			return true;
		}
		
		if (initFunc) 
		{
			try {
				logger.info() << "initializing mod: " << info.name;
				initFunc(engine);
				initialized = true;
				return true;
			} catch (const std::exception& e) {
				logger.error() << "failed to initialize mod " << info.name 
							<< ": " << e.what();
				return false;
			}
		}

		return false;
	}

	void LoadedMod::shutdown() 
	{
		if (!initialized) {
			return;
		}
		
		if (shutdownFunc) 
		{
			try {
				logger.info() << "shutting down mod: " << info.name;
				shutdownFunc();
			} catch (const std::exception& e) {
				logger.error() << "error during mod shutdown " << info.name 
							<< ": " << e.what();
			}
		}

		initialized = false;
	}

	ModLoader::~ModLoader() {
		unloadAll();
	}

	void* ModLoader::loadLibrary(const std::string& path) 
	{
		#ifdef _WIN32
			return LoadLibraryA(path.c_str());
		#else
			return dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
		#endif
	}

	void* ModLoader::getFunction(void* handle, const std::string& name) 
	{
		if (!handle) {
			return nullptr;
		}

		#ifdef _WIN32
			return reinterpret_cast<void*>(GetProcAddress(
				static_cast<HMODULE>(handle), name.c_str()
			));
		#else
			return dlsym(handle, name.c_str());
		#endif
	}

	void ModLoader::unloadLibrary(void* handle) 
	{
		if (!handle) {
			return;
		}
		
		#ifdef _WIN32
			FreeLibrary(static_cast<HMODULE>(handle));
		#else
			dlclose(handle);
		#endif
	}

	std::string ModLoader::getLibraryExtension() {
		#ifdef _WIN32
			return ".dll";
		#else
			return ".so";
		#endif
	}

	bool ModLoader::loadMod(const io::path& modPath, Engine& engine) 
	{
		std::string pathStr;

		try {
			pathStr = io::resolve(modPath).string();
		} catch (const std::exception& e) {
			logger.error() << "failed to resolve mod path: " << modPath.string()
						<< " (" << e.what() << ")";
			return false;
		}
		
		logger.info() << "loading mod from: " << pathStr;
		
		void* handle = loadLibrary(pathStr);
		if (!handle) 
		{
			#ifdef _WIN32
				DWORD error = GetLastError();
				logger.error() << "failed to load library: " << pathStr 
							<< " (error code: " << error << ")";
			#else
				logger.error() << "failed to load library: " << pathStr 
							<< " (" << dlerror() << ")";
			#endif

			return false;
		}
		
		using GetInfoFunc = ModInfo (*)();
		auto getInfo = reinterpret_cast<GetInfoFunc>(
			getFunction(handle, "mod_get_info")
		);
		
		ModInfo info;

		if (getInfo) 
		{
			info = getInfo();
		} 
		else 
		{
			logger.warning() << "mod_get_info not found, using defaults";
			info.name = modPath.name();
			info.version = "unknown";
			info.author = "unknown";
			info.description = "";
		}
		
		auto mod = std::make_unique<LoadedMod>(handle, info);
		
		mod->initFunc = reinterpret_cast<ModInitFunc>(
			getFunction(handle, "mod_initialize")
		);

		mod->shutdownFunc = reinterpret_cast<ModShutdownFunc>(
			getFunction(handle, "mod_shutdown")
		);
		
		if (!mod->initFunc) 
		{
			logger.error() << "mod_initialize function not found in: " << pathStr;
			unloadLibrary(handle);
			return false;
		}
		
		if (!mod->initialize(engine)) 
		{
			unloadLibrary(handle);
			return false;
		}
		
		logger.info() << "successfully loaded mod: " << info.name 
					<< " v" << info.version << " by " << info.author;
		
		loadedMods.push_back(std::move(mod));
		return true;
	}

	size_t ModLoader::loadMods(const io::path& modsPath, Engine& engine) 
	{
		if (!io::is_directory(modsPath)) {
			logger.warning() << "mods directory not found: " << modsPath.string();
			return 0;
		}
		
		logger.info() << "scanning mods directory: " << modsPath.string();
		
		std::string extension = getLibraryExtension();
		size_t loadedCount = 0;
		
		try {
			for (const auto& entry : io::directory_iterator(modsPath)) 
			{
				std::string filename = entry.name();
				
				if (filename.size() >= extension.size() && filename.substr(filename.size() - extension.size()) == extension) 
				{
					if (loadMod(entry, engine)) {
						loadedCount++;
					}
				}
			}
		} catch (const std::exception& e) {
			logger.error() << "error scanning mods directory: " << e.what();
		}
		
		logger.info() << "loaded " << loadedCount << " mod(s)";

		return loadedCount;
	}

	void ModLoader::unloadAll() 
	{
		logger.info() << "unloading all mods";

		for (auto it = loadedMods.rbegin(); it != loadedMods.rend(); ++it) {
			(*it)->shutdown();
		}
		
		loadedMods.clear();
	}
}