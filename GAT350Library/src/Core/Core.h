#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


#if defined(_WIN32) || defined(_WIN64)
	#define PLATFORM_WINDOWS
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	#if TARGET_OS_MAC == 1
		#define PLATFORM_MACOS
		#error "MacOS is not sapported"
	#endif
#elif defined(__linux__)
	#define PLATFORM_LINUX
	#error "Linux is not sapported"
#else
	#error "Unknown Platform!"
#endif

#define BIT(x) (1 << x)

namespace Engine
{
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	using Scope = std::unique_ptr<T>;
}

#include "Log.h"
#include "Util/Performance.h"
#include <filesystem>
namespace fs = std::filesystem;