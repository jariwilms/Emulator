workspace "dotM"
	architecture "x64"
	
	configurations
	{
		"Debug", 
		"Release"
	}
	
	outputdir = "%{cfg.buildcfg}/%{cfg.system}"
	
project "dotM"
	location "dotM"
	language "C++"
	cppdialect "C++20"
	kind "ConsoleApp"
	staticruntime "On"
	
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin_obj/" .. outputdir .. "/%{prj.name}")
	
	pchheader "stdafx.hpp"
	pchsource "dotM/src/stdafx.cpp"
	
	files
	{
		"%{prj.name}/src/**.h", 
		"%{prj.name}/src/**.hpp", 
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"%{prj.name}/src", 
	}
	
	defines
	{
		"SOLUTION_DIR=R\"($(SolutionDir.TrimEnd('\\')))\"", 
	}
		
	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		
	filter "configurations:Release"
		runtime "Release"
		optimize "On"
