workspace "dotM"
	configurations { "Debug", "Release" }
	platforms { "Win32", "x64" }
	
	startproject "Run"
	outputdir = "%{cfg.buildcfg}/%{cfg.platform}"
	
project "dotM"
	location "dotM"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	staticruntime "Off"
	
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
	
	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		
	filter "configurations:Release"
		runtime "Release"
		optimize "On"

project "Run"
	location "Run"
	language "C++"
	cppdialect "C++20"
	kind "ConsoleApp"
	staticruntime "Off"
	
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin_obj/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h", 
		"%{prj.name}/src/**.hpp", 
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{ 
		"dotM/src", 
	}
	
	links
	{
		"dotM", 
	}
	
	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		
	filter "configurations:Release"
		runtime "Release"
		optimize "On"
	