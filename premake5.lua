
------------------------------------------------------
-- Global projects flags / defines:
------------------------------------------------------

-- Project-wide compiler flags for all builds:
local BUILD_OPTS = {
	-- Misc lang flags:
	"-std=c++14",
	"-fstrict-aliasing",
	-- Warnings (GCC and Clang compatible):
	"-Wall",
	"-Wextra",
	"-Weffc++",
	"-Winit-self",
	"-Wformat=2",
	"-Wstrict-aliasing",
	"-Wuninitialized",
	"-Wunused",
	"-Wswitch",
	"-Wswitch-default",
	"-Wpointer-arith",
	"-Wwrite-strings",
	"-Wmissing-braces",
	"-Wparentheses",
	"-Wsequence-point",
	"-Wreturn-type",
	"-Wunknown-pragmas",
	"-Wshadow",
	"-Wdisabled-optimization",
	-- Extended warnings (Clang specific):
	"-Wgcc-compat",
	"-Wheader-guard",
	"-Waddress-of-array-temporary",
	"-Wglobal-constructors",
	"-Wexit-time-destructors",
	"-Wheader-hygiene",
	"-Woverloaded-virtual",
	"-Wself-assign",
	"-Wweak-vtables",
	"-Wweak-template-vtables",
	"-Wshorten-64-to-32"
}

-- Project-wide Debug build switches:
local DEBUG_DEFS = {
	"DEBUG", "_DEBUG",  -- Enables assert()
	"_GLIBCXX_DEBUG",   -- GCC std lib debugging
	"_LIBCPP_DEBUG=0",  -- For Clang (libc++)
	"_LIBCPP_DEBUG2=0", -- Clang; See: http://stackoverflow.com/a/21847033/1198654
	"_SECURE_SCL"       -- For VS to enable STL bounds checking
}

-- Project-wide Release build switches:
local RELEASE_DEFS = { "NDEBUG" }

-- Project-wide configuration switches for all builds:
local COMMON_DEFS = { }

-- Target names:
local LIB_OL_NAME = "OL"

------------------------------------------------------
-- Common configurations for all projects:
------------------------------------------------------

workspace "Outlaws"
	configurations { "Debug", "Release" }
	buildoptions   { BUILD_OPTS  }
	defines        { COMMON_DEFS }
	language       "C++"
	location       "build"
	targetdir      "build"
	libdirs        "build"
	includedirs    "source"

	filter "configurations:Debug"
		optimize "Off"
		flags    "Symbols"
		defines  { DEBUG_DEFS }

	filter "configurations:Release"
		optimize "On"
		defines  { RELEASE_DEFS }

------------------------------------------------------
-- LibOL static library:
------------------------------------------------------

project (LIB_OL_NAME)
	kind  "StaticLib"
	files { "source/ol/**.hpp", "source/ol/**.cpp" }

------------------------------------------------------
-- lab_unpack command line tool:
------------------------------------------------------

project "lab_unpack"
	kind        "ConsoleApp"
	includedirs { "source/" }
	files       { "source/lab_unpack.cpp" }
	links       { LIB_OL_NAME }

------------------------------------------------------
-- lab_pack command line tool:
------------------------------------------------------

project "lab_pack"
	kind        "ConsoleApp"
	includedirs { "source/" }
	files       { "source/lab_pack.cpp" }
	links       { LIB_OL_NAME }

------------------------------------------------------
-- A temporary driver program:
------------------------------------------------------

project "ol_test"
	kind        "ConsoleApp"
	includedirs { "source/" }
	files       { "source/ol_test.cpp" }
	links       { LIB_OL_NAME }

