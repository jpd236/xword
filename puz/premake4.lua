project "puz"
    -- --------------------------------------------------------------------
    -- General
    -- --------------------------------------------------------------------
    kind "SharedLib"
    language "C++"
    files {
        "**.hpp", -- ** means all subdirs
        "**.cpp",
        "**.c",
        "**.h",
    }

    configuration "not windows"
        excludes { "utils/minizip/iowin32.*" }
    configuration {}

    includedirs {
        -- This allows #include "Puzzle.hpp" from the formats subdirs.
        -- The alternative would be #include "../../Puzzle.hpp", which is a pain
        ".",
    }

    sysincludedirs { "../yajl/build/yajl-2.0.2/include" }

    links { "yajl" }

    configuration "windows"
        defines {
            "_USRDLL",
            "YAJL_SHARED",
            "PUZ_API=__declspec(dllexport)",
        }
        -- zlib
        includedirs { DEPS.zlib.include }
        libdirs { DEPS.zlib.lib }
        links { "zdll" }

    configuration "linux"
        defines { 
            "PUZ_API=",
	    "LUAPUZ_API=",
        }
        links { "dl", "z" }

    configuration "macosx"
        defines {
            "PUZ_API=",
            "LUAPUZ_API=",
            "USE_FILE32API" -- for minizip
        }
        links { "z" }

    -- Disable some warnings
    configuration "vs*"
        buildoptions {
            "/wd4251", -- DLL Exports
            "/wd4275", -- DLL Exports
        }

    -- Postbuild: copy zlib1.dll to XWord directory

    configuration { "windows", "Debug" }
       postbuildcommands { DEPS.zlib.copydebug }

    configuration { "windows", "Release" }
       postbuildcommands { DEPS.zlib.copyrelease }
