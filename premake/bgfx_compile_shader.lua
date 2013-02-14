
--- compile a shader for a specific tarhet using bgfx shaderc tool
-- @remark Assume BGFX_DIR is defined
-- @param _input_dir Source directory of your shader files (eg: PROJECT_ROOT/shaders)
-- @param _output_dir Output directory of your compiled shader files (eg: PROJECT_RUNTIME_DIR) ("shaders/" will be appended)
-- @param _intermediate_dir Build directory for temporary files (eg: PROJECT_BUILD_DIR) ("shaders/" will be appended)
-- @param _target Target output options are (string): dx9 , dx11, nacl, android, linux
function bgfx_compile_shader( _input_dir, _output_dir, _intermediate_dir, _target)
	-- full path to shader compiler
	local SHADERC_PATH = path.getabsolute(BGFX_DIR) .. "/tools/bin/shaderc"
	local VS_FLAGS = ""
	local FS_FLAGS = ""
	local SHADER_PATH = ""

	if(_target == "dx9") then
		VS_FLAGS = "--platform windows -p vs_3_0 -O 3"
		FS_FLAGS = "--platform windows -p ps_3_0 -O 3"
		SHADER_PATH = "shaders/dx9"
	elseif  (_target == "dx11") then
		VS_FLAGS = "--platform windows -p vs_4_0 -O 3"
		FS_FLAGS = "--platform windows -p ps_4_0 -O 3"
		SHADER_PATH = "shaders/dx11"
	elseif  (_target == "nacl") then
		VS_FLAGS = "--platform nacl"
		FS_FLAGS = "--platform nacl"
		SHADER_PATH = "shaders/gles"
	elseif  (_target == "android") then
		VS_FLAGS = "--platform android"
		FS_FLAGS = "--platform android"
		SHADER_PATH = "shaders/gles"
	elseif  (_target == "linux") then
		VS_FLAGS = "--platform linux -p 120"
		FS_FLAGS = "--platform linux -p 120"
		SHADER_PATH = "shaders/glsl"
	end


	local INPUT_DIR = path.getabsolute(_input_dir) .. "/"	
	local OUTPUT_DIR = path.getabsolute(_output_dir) .. "/" .. SHADER_PATH .. "/"
	local INTERMEDIATE_DIR = path.getabsolute(_intermediate_dir) .. "/" .. SHADER_PATH .. "/"
		
	-- make sure the build directories exists
	os.mkdir(OUTPUT_DIR)
	os.mkdir(INTERMEDIATE_DIR)
	
	-- compile vertex shaders
	local VS_SOURCES = os.matchfiles( INPUT_DIR .. "vs_*.sc")
	
	for _, inputPath in ipairs(VS_SOURCES) do
		local buildPath = INTERMEDIATE_DIR .. "/" .. path.getbasename(inputPath) .. ".bin"
		local outputPath = OUTPUT_DIR .. "/" .. path.getbasename(inputPath) .. ".bin"
		local command = SHADERC_PATH .. " " .. VS_FLAGS .. " --type vertex --depends -o " .. buildPath .. " -f " .. inputPath .. " --disasm" .. " --varyingdef " .. INPUT_DIR .. "varying.def.sc"
		
		-- execute in a separate thread to get the output
		local handle = io.popen(command)
		local result = handle:read("*a")
		handle:close()
		-- TODO understand auto dependency effect (.bin.d) and apply it
		os.copyfile(buildPath, outputPath)
	end

	local FS_SOURCES = os.matchfiles( INPUT_DIR .. "fs_*.sc")
	for _, inputPath in ipairs(FS_SOURCES) do
		local buildPath = INTERMEDIATE_DIR .. "/" .. path.getbasename(inputPath) .. ".bin"
		local outputPath = OUTPUT_DIR .. "/" .. path.getbasename(inputPath) .. ".bin"
		local command = SHADERC_PATH .. " " .. FS_FLAGS .. " --type fragment --depends -o " .. buildPath .. " -f " .. inputPath .. " --disasm" .. " --varyingdef " .. INPUT_DIR .. "varying.def.sc"
		-- execute in a separate thread to get the output
		local handle = io.popen(command)
		local result = handle:read("*a")
		handle:close()
		-- TODO understand auto dependency effect (.bin.d) and apply it
		os.copyfile(buildPath, outputPath)
	end

end