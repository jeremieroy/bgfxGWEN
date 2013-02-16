-- Dependencies root directories
BGFX_DIR = "../../bgfx/"
BX_DIR = "../../bx/"
GWEN_DIR = "../../GWEN/"

-- other directories

ENV_DIR = "../_env/"
BIN_DIR = "../_bin/"
BUILD_DIR = "../_build/"

INPUT_SHADERS_DIR = "../shaders/"

solution "bgfxGWEN"
configurations {
		"Debug",
		"Release",
	}

	platforms {
		"x32"
	}

	language "C++"


-- bgfx project

dofile (BX_DIR .. "premake/toolchain.lua")

local BGFX_BUILD_DIR = BUILD_DIR
local BGFX_THIRD_PARTY_DIR = (BGFX_DIR .. "3rdparty/")
toolchain(BGFX_BUILD_DIR, BGFX_THIRD_PARTY_DIR)


dofile "bgfx_compile_shader.lua"
function compileShaders()
	bgfx_compile_shader(INPUT_SHADERS_DIR, ENV_DIR, BUILD_DIR, "dx9" )
	bgfx_compile_shader(INPUT_SHADERS_DIR, ENV_DIR, BUILD_DIR, "dx11" )
	bgfx_compile_shader(INPUT_SHADERS_DIR, ENV_DIR, BUILD_DIR, "nacl" )
	bgfx_compile_shader(INPUT_SHADERS_DIR, ENV_DIR, BUILD_DIR, "android" )
	bgfx_compile_shader(INPUT_SHADERS_DIR, ENV_DIR, BUILD_DIR, "linux" )
end

newaction {
    trigger = 'shaders',
    description = 'Bake shaders',
    shortname = "Bake shaders",
    --valid_kinds = premake.action.get("*").valid_kinds,
    --valid_languages = premake.action.get("*").valid_languages,
    --valid_tools = premake.action.get("*").valid_tools,
    execute = function()
		compileShaders()
    end
}





function copyLib()
end
dofile (BGFX_DIR .. "premake/bgfx.lua")


-- gwen static project

project "GWEN_Static"
	defines { "GWEN_COMPILE_STATIC" }
	includedirs { GWEN_DIR .. "gwen/include/" }
	files { GWEN_DIR .. "gwen/src/**.*", GWEN_DIR .. "gwen/include/Gwen/**.*" }
	flags { "Symbols" }
	kind "StaticLib"	

project "GWEN_UnitTest"
	includedirs { GWEN_DIR .. "gwen/include/" }
	files { GWEN_DIR .. "gwen/UnitTest/**.*" }
	flags { "Symbols" }
	kind "StaticLib"

-- bgfxGwenRenderer project
project ( "bgfxGwenRenderer" )
	includedirs { GWEN_DIR .. "gwen/include/" , BX_DIR .. "include", BGFX_DIR .. "include" }
	files( "../Renderer/**.*" )
	flags( { "Symbols" } )
	kind( "StaticLib" )	



function exampleProject(_name, _uuid)

	project (_name)
		uuid (_uuid)
		kind "WindowedApp"

	configuration {}

	debugdir (BGFX_DIR .. "examples/runtime/")

	includedirs {
		BX_DIR .. "include",
		BGFX_DIR .. "include",
	}

	files {
		BGFX_DIR .. "examples/common/**.cpp",
		BGFX_DIR .. "examples/common/**.h",
		BGFX_DIR .. "examples/" .. _name .. "/**.cpp",
		BGFX_DIR .. "examples/" .. _name .. "/**.h",
	}

	links {
		"bgfx",
	}

	configuration { "emscripten" }
		targetextension ".bc"

	configuration { "nacl or nacl-arm or pnacl" }
		targetextension ".nexe"
		links {
			"ppapi",
			"ppapi_gles2",
			"pthread",
		}

	configuration { "nacl", "Release" }
		postbuildcommands {
			"@echo Stripping symbols.",
			"@$(NACL)/bin/x86_64-nacl-strip -s \"$(TARGET)\""
		}

	configuration { "linux" }
		links {
			"GL",
			"pthread",
		}

	configuration { "macosx" }
		files {
			BGFX_DIR .. "examples/common/**.mm",
		}
		links {
			"Cocoa.framework",
			"OpenGL.framework",
		}
end

-- bgfxGwenSample project
--exampleProject("01-cubes", "ff2c8450-ebf4-11e0-9572-0800200c9a66")


-- bgfxGwenSample project
exampleProject("bgfxGwenSample", "ff2c8450-ebf4-11e0-9572-0800200c9a66")
configuration {}
includedirs { BGFX_DIR .. "examples/", "../Renderer/", GWEN_DIR .. "gwen/include/" }
files( "../Sample/**.*" )
links { "GWEN_UnitTest", "bgfxGwenRenderer", "GWEN_Static" } --, "FreeImage"} 	

--project ( "bgfxGwenSample" )
	--includedirs { "../Renderer/", GWEN_DIR .. "gwen/include/" , BX_DIR .. "include", BGFX_DIR .. "include" }
	--files( "../Sample/**.*" )
	--flags( { "Symbols" } )
	--kind( "WindowedApp" )
	--links { "bgfx", "GWEN_UnitTest", "bgfxGwenRenderer", "GWEN_Static" } --, "FreeImage" } 	
