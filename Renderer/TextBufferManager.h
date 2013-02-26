#pragma once
#include "bgfx_font_types.h"
#include "TextBuffer.h"
#include <bx/handlealloc.h>
#include <bgfx.h>

namespace bgfx_font
{

class TextBufferManager
{
public:
	TextBufferManager(FontManager* fontManager = NULL);
	~TextBufferManager();
	
	void init(FontManager* fontManager, const char* shaderPath);

	TextBufferHandle createTextBuffer(FontType _type, BufferType bufferType);
	void destroyTextBuffer(TextBufferHandle handle);
	void submitTextBuffer(TextBufferHandle _handle, uint8_t _id, int32_t _depth = 0);
	void submitTextBufferMask(TextBufferHandle _handle, uint32_t _viewMask, int32_t _depth = 0);

	TextBuffer* getTextBuffer(TextBufferHandle handle);	
private:
	struct BufferCache
	{
		uint16_t indexBufferHandle;
		uint16_t vertexBufferHandle;
		TextBuffer textBuffer;
		BufferType bufferType;
		FontType fontType;
		bool initialized;
	};

	BufferCache* m_textBuffers;
	bx::HandleAlloc m_textBufferHandles;
	FontManager* m_fontManager;
	bgfx::VertexDecl m_vertexDecl;	
	bgfx::UniformHandle m_u_texColor;
	//shaders program
	bgfx::ProgramHandle m_basicProgram;
	bgfx::ProgramHandle m_distanceProgram;
};

}
