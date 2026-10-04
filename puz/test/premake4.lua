project "puz_test"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++11"
    files {
        "*.cpp",
        "*.hpp",
        "*.h",
    }

    includedirs {
        ".",
        "../",
        "../../",
        "../../deps/doctest",
    }

    links { "puz" }

    configuration "windows"
        defines {
            "PUZ_API=__declspec(dllimport)",
        }

    configuration "linux"
        defines {
            "PUZ_API=",
        }
        buildoptions {
            "-std=c++11",
        }
        links { "dl", "yajl" }

    configuration "macosx"
        defines {
            "PUZ_API=",
        }
        buildoptions {
            "-std=c++11",
        }

    configuration "vs*"
        editandcontinue "Off"
        buildoptions {
            "/wd4251", -- DLL Exports
            "/wd4275", -- DLL Exports
        }
