project "XWord"
    -- --------------------------------------------------------------------
    -- General
    -- --------------------------------------------------------------------
    kind "WindowedApp"
    language "C++"

    files { "**.hpp", "**.cpp", "**.h" }

    dpiawareness "HighPerMonitor"

    local xword_version = os.getenv("XWORD_VERSION")
    if xword_version == nil or xword_version == "" then
        local version_file = assert(io.open("../VERSION", "r"))
        local version = version_file:read("*all")
        version_file:close()
        xword_version = string.gsub(version, '%s+', '')
    end

    configuration {}
        defines { [[XWORD_VERSION_STRING="]]..xword_version..[["]] }

    configuration "windows"
        -- Use WinMain() instead of main() for windows apps
        entrypoint "WinMainCRTStartup"

    configuration "macosx"
        files { "**.mm" }
        excludes { "dialogs/wxFB_PreferencesPanels.*" }

    configuration "not macosx"
        excludes { "dialogs/wxFB_PreferencesPanelsOSX.*" }

    configuration { }

    -- --------------------------------------------------------------------
    -- puz
    -- --------------------------------------------------------------------
    includedirs { "../" }
    links { "puz" }
    configuration "windows"
        defines {
            "PUZ_API=__declspec(dllimport)",
            "LUAPUZ_API=__declspec(dllimport)",
        }

    configuration "linux"
        defines {
            "PUZ_API=",
            "LUAPUZ_API=",
        }
        -- TODO: Why is yajl necessary, when it's only needed by puz?
        -- libpuz.so seems to link to libyajl.so via a bad relative path
        links { "dl", "yajl" }

    configuration "macosx"
        defines {
            "PUZ_API=",
            "LUAPUZ_API=",
            "USE_FILE32API" -- for minizip
        }

    -- Disable some warnings
    configuration "vs*"
        buildoptions {
            "/wd4800", -- implicit conversion to bool
            "/wd4251", -- DLL Exports
        }
    -- --------------------------------------------------------------------
    -- wxLua
    -- --------------------------------------------------------------------

    if not _OPTIONS["disable-lua"] then
        configuration {}
            defines "XWORD_USE_LUA"

            sysincludedirs {
                DEPS.lua.include,
                "../lua",
                "../lua/wxbind/setup",
            }

            libdirs { DEPS.lua.lib }

            links {
                "lua51",
                "wxlua",
                "wxbindbase",
                "wxbindcore",
                "wxbindadv",
                "wxbindaui",
                "wxbindhtml",
                "wxbindnet",
                "wxbindxml",
                "wxbindxrc",
                "luapuz",
            }

        -- These link options ensure that the wxWidgets libraries are
        -- linked in the correct order under linux.
        configuration "linux"
            linkoptions {
                "-lwxbindxrc",
                "-lwxbindxml",
                "-lwxbindnet",
                "-lwxbindhtml",
                "-lwxbindaui",
                "-lwxbindadv",
                "-lwxbindcore",
                "-lwxbindbase",
                "-lwxlua",
                "-llua51",
            }

        -- Postbuild: copy lua51.dll to XWord directory

        configuration { "windows", "Debug" }
           postbuildcommands { DEPS.lua.copydebug }

        configuration { "windows", "Release" }
           postbuildcommands { DEPS.lua.copyrelease }

    else -- disable-lua
        configuration {}
            excludes {
                "xwordbind/*",
                "xwordlua*",
            }
    end

    -- --------------------------------------------------------------------
    -- wxWidgets
    -- --------------------------------------------------------------------

    -- Note: This must come after the link options above to ensure correct
    -- link order on Linux.
    dofile "../premake4_wxdefs.lua"
    dofile "../premake4_wxlibs.lua"

    -- --------------------------------------------------------------------
    -- Resources
    -- --------------------------------------------------------------------
    configuration "windows"
        files { "**.rc" }
        resincludedirs { ".." }

    configuration { "macosx" }
        postbuildcommands {
            "cd $TARGET_BUILD_DIR",
            -- Symlink images and scripts
            "mkdir -p $PLUGINS_FOLDER_PATH",
            "ln -sFh ../../../../../scripts $PLUGINS_FOLDER_PATH/scripts",
            "mkdir -p $UNLOCALIZED_RESOURCES_FOLDER_PATH",
            "ln -sFh ../../../../../images $UNLOCALIZED_RESOURCES_FOLDER_PATH/images",
            -- Copy Info.plist, xword.icns, and default_config.ini
            "sed 's/{XWORD_VERSION}/"..xword_version.."/' ../../src/Info.plist > $INFOPLIST_PATH",
            "cp ../../images/xword.icns $UNLOCALIZED_RESOURCES_FOLDER_PATH",
            "cp ../../default_config.ini $UNLOCALIZED_RESOURCES_FOLDER_PATH",
        }
        if not _OPTIONS["disable-lua"] then
            postbuildcommands {
                -- Build the rest of the projects
                "cd ../../build/" .. _ACTION,
                'xcodebuild -project lfs.xcodeproj -configuration "$CONFIGURATION"',
                'xcodebuild -project luacurl.xcodeproj -configuration "$CONFIGURATION"',
                'xcodebuild -project luatask.xcodeproj -configuration "$CONFIGURATION"',
                'xcodebuild -project lxp.xcodeproj -configuration "$CONFIGURATION"',
                'xcodebuild -project luayajl.xcodeproj -configuration "$CONFIGURATION"',
                'xcodebuild -project lyaml.xcodeproj -configuration "$CONFIGURATION"',
            }
        end
