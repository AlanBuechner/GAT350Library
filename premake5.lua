workspace "GAT350Library"
	architecture "x64"
	startproject "GAT350Library"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
--IncludeDir["ImGui"] = "GAT350Library/vendor/imgui"
IncludeDir["glm"] = "GAT350Library/vendor/Glm"
IncludeDir["stb_image"] = "GAT350Library/vendor/Stb_Image"
IncludeDir["assimp"] = "GAT350Library/vendor/assimp"
IncludeDir["entt"] = "GAT350Library/vendor/entt/include"
--IncludeDir["yaml"] = "GAT350Library/vendor/yaml-cpp/include"
--IncludeDir["ImGuizmo"] = "GAT350Library/vendor/ImGuizmo"

--include "GAT350Library/vendor/imgui"
--include "GAT350Library/vendor/yaml-cpp"

project "GAT350Library"
	location "GAT350Library"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

--	pchheader "pch.h"
--	pchsource "GameEngine/src/pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/Glm/glm/**.hpp",
		"%{prj.name}/vendor/Glm/glm/**.inl",
--		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
--		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp",
		"%{prj.name}/Assets/Shaders/**.hlsl"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
--		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.assimp}/include",
		"%{IncludeDir.entt}",
--		"%{IncludeDir.yaml}",
--		"%{IncludeDir.ImGuizmo}"
	}

	links 
	{ 
--		"ImGui",
--		"Yaml",
		"%{prj.location}/vendor/assimp/assimp-vc140-mt.lib",
		"dxguid.lib"
	}

	-- TODO : fix environment variables. need to set manually
	environmentVariables =
	{
		"PATH=%PATH%;$(ProjectDir)vendor\assimp"
	}

	filter { "files:**.hlsl" }
		shadermodel "5.0"
		shaderobjectfileoutput("ShaderBin/".."%{file.basename}"..".cso")

	filter { "files:**.pixel.hlsl" }
		shadertype "Pixel"

	filter { "files:**.vertex.hlsl" }
		shadertype "Vertex"

--	filter "files:vendor/ImGuizmo/**.cpp"
--	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

		defines
		{
			
		}

	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "on"

--include "GAT350Library/vendor/assimp"