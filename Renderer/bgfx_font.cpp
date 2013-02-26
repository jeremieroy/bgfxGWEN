#include "bgfx_font.h"
#include "FontManager.h"
#include "TextBufferManager.h"
#include <bx/macros.h>

#include <assert.h>
#include <stdarg.h> // va_list
#include <alloca.h>
#include <unordered_map>

namespace bgfx_font
{

struct Context
{	
	bgfx_font::FontManager fontManager;	
	bgfx_font::TextBufferManager textManager;	
};

static Context* g_context = NULL;

void init(const char* shaderPath)
{
	assert(g_context == NULL && "A context can only be initialized once");
	g_context = new Context;	
	g_context->textManager.init(&g_context->fontManager, shaderPath);
}
		
void shutdown()
{
	assert(g_context != NULL && "Context not initialized");		
	delete g_context;
	g_context = NULL;	
}

TextureAtlasHandle createTextureAtlas(TextureType _type, uint16_t _width, uint16_t _height)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	return g_context->fontManager.createTextureAtlas(_type, _width, _height);
}

bgfx::TextureHandle getTextureHandle(TextureAtlasHandle _handle)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	return g_context->fontManager.getTextureHandle(_handle);
}

void destroyTextureAtlas(TextureAtlasHandle _handle)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	g_context->fontManager.destroyTextureAtlas(_handle);
}

TrueTypeHandle loadTrueTypeFont(const char * _fontPath)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	return (TrueTypeHandle)g_context->fontManager.loadTrueTypeFromFile(_fontPath, 0);	
}

TrueTypeHandle loadTrueTypeFont(const bgfx::Memory* _mem)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	assert(false && "Not implemented yet");
	return TrueTypeHandle(INVALID_HANDLE_ID);
}

void unloadTrueTypeFont(TrueTypeHandle _handle)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	g_context->fontManager.unLoadTrueType((bgfx_font::TrueTypeHandle)_handle);	
}
		
FontHandle createFontByPixelSize(TrueTypeHandle _handle, uint32_t _pixelSize, FontType _fontType)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	return (FontHandle) g_context->fontManager.createFontByPixelSize(_handle, _pixelSize, _fontType);
}
	
FontHandle createFontByEmSize(TrueTypeHandle _handle, uint32_t _pixelSize, FontType _fontType)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	return (FontHandle) g_context->fontManager.createFontByEmSize(_handle, _pixelSize, _fontType);
}

FontHandle loadBakedFont(const char * _fontPath, const char * _fontName)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	assert(false && "Not implemented yet");
	return FontHandle(INVALID_HANDLE_ID);
}

void destroyFont(FontHandle _handle)
{
	g_context->fontManager.destroyFont(_handle);
}

bool preloadGlyph(FontHandle _handle, const wchar_t* _string)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	return g_context->fontManager.preloadGlyph(_handle, _string);
}

void bakeAndSaveFont(FontHandle _handle, const char * _fontPath, const char * _fontName)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	assert(false && "Not implemented yet");
}

TextBufferHandle createTextBuffer(FontType _type, BufferType bufferType)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	return g_context->textManager.createTextBuffer(_type,bufferType);
}

void destroyTextBuffer(TextBufferHandle _handle)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	g_context->textManager.destroyTextBuffer(_handle);	
}

void setTextStyle(TextBufferHandle _handle, uint32_t flags)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	TextBuffer* txt = g_context->textManager.getTextBuffer(_handle);
	txt->setStyle(flags);
}

void setTextColor(TextBufferHandle _handle, uint32_t _rgba)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	TextBuffer* txt = g_context->textManager.getTextBuffer(_handle);
	txt->setTextColor(_rgba);
}
	
void setTextBackgroundColor(TextBufferHandle _handle, uint32_t _rgba)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	TextBuffer* txt = g_context->textManager.getTextBuffer(_handle);
	txt->setBackgroundColor(_rgba);
}

void setOverlineColor(TextBufferHandle _handle, uint32_t _rgba)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	TextBuffer* txt = g_context->textManager.getTextBuffer(_handle);
	txt->setOverlineColor(_rgba);
}

void setUnderlineColor(TextBufferHandle _handle, uint32_t _rgba)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	TextBuffer* txt = g_context->textManager.getTextBuffer(_handle);
	txt->setUnderlineColor(_rgba);
}
	
void setStrikeThroughColor(TextBufferHandle _handle, uint32_t _rgba )
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	TextBuffer* txt = g_context->textManager.getTextBuffer(_handle);
	txt->setStrikeThroughColor(_rgba);
}
	
void setPenPosition(TextBufferHandle _handle, float x, float y)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	TextBuffer* txt = g_context->textManager.getTextBuffer(_handle);
	txt->setPenPosition(x,y);
}
	
void appendText(TextBufferHandle _handle, FontHandle fontHandle, const char * _string)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	TextBuffer* txt = g_context->textManager.getTextBuffer(_handle);
	txt->appendText(fontHandle, _string);
}

void appendText(TextBufferHandle _handle, FontHandle fontHandle, const wchar_t * _string)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	TextBuffer* txt = g_context->textManager.getTextBuffer(_handle);
	txt->appendText(fontHandle, _string);
}

void appendTextPrintf(TextBufferHandle _handle, FontHandle fontHandle, const char * _format, ...)
{
	/*
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	assert(false && "Not implemented yet");
	va_list argList;
	va_start(argList, _format);

	char* temp = (char*)alloca(m_width);
	uint32_t num = vsnprintf(temp, m_width, _format, _argList);

	uint8_t* mem = &m_mem[(_y*m_width+_x)*2];
	for (uint32_t ii = 0, xx = _x; ii < num && xx < m_width; ++ii, ++xx)
	{
		mem[0] = temp[ii];
		mem[1] = _attr;
		mem += 2;
	}
	va_end(argList);
	*/
}

void appendTextPrintf(TextBufferHandle _handle, FontHandle fontHandle, const wchar_t * format, ...)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	assert(false && "Not implemented yet");
}

void clearTextBuffer(TextBufferHandle _handle)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	TextBuffer* txt = g_context->textManager.getTextBuffer(_handle);
	txt->clearTextBuffer();
}
	
void submitTextBuffer(TextBufferHandle _handle, uint8_t _id,  int32_t _depth)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	g_context->textManager.submitTextBuffer(_handle, _id, _depth);	
}

void submitTextBufferMask(TextBufferHandle _handle, uint32_t _viewMask, int32_t _depth)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	g_context->textManager.submitTextBufferMask(_handle, _viewMask, _depth);
}

}

