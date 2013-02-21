#include "bgfx_font.h"
#include "FontManager.h"
#include <assert.h>

#include <unordered_map>

namespace bgfx_text
{

//engine abstraction
class TextureProvider_bgfx : public bgfx_font::ITextureProvider
{
public:
    TextureProvider_bgfx(uint16_t width, uint16_t height, uint32_t type, uint32_t depth): m_width(width), m_height(height), m_type(type), m_depth(depth)
    {
		const bgfx::Memory* mem = NULL;		
		uint32_t flags = BGFX_TEXTURE_MIN_POINT|BGFX_TEXTURE_MAG_POINT|BGFX_TEXTURE_U_CLAMP|BGFX_TEXTURE_V_CLAMP;
		//uint32_t flags = BGFX_TEXTURE_NONE;
		if(depth==1)
			m_handle = bgfx::createTexture2D(width, height, 1, bgfx::TextureFormat::L8, flags, mem);
		else
			m_handle = bgfx::createTexture2D(width, height, 1, bgfx::TextureFormat::BGRA8, flags, mem);		
    }

	~TextureProvider_bgfx()
	{
		bgfx::destroyTexture(m_handle);
	}

    uint16_t getWidth() { return m_width; }
    uint16_t getHeight() { return m_height; }
    uint32_t getDepth() { return m_depth; }
	bgfx_font::TextureType getTextureType() { return m_type; }

	void update(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t* data)
    {	
		const bgfx::Memory* mem = bgfx::alloc(width*height*m_depth);
		memcpy(mem->data, data, width*height*m_depth);
		bgfx::updateTexture2D(m_handle, 0, x, y, width, height, mem);
    }    
    uint16_t m_width;
    uint16_t m_height;
    uint32_t m_depth;
	bgfx_font::TextureType m_type;
    bgfx::TextureHandle m_handle;
};


struct Context
{
	Context():textureUID(0){}
	bgfx_font::FontManager fontManager;
	uint32_t textureUID;
	std::unordered_map< bgfx::TextureHandle, TextureProvider_bgfx*> textureProviders;
};
	
static Context* g_context = NULL;

void init()
{
	assert(g_context == NULL && "A context can only be initialized once");
	g_context = new Context();
}
		
void shutdown()
{
	delete g_context;
	g_context = NULL;
}

///  allocate a texture of this type
bgfx::TextureHandle createTexture(TextureType _textureType, uint16_t _width, uint16_t _height)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	g_context = new Context();
	
	
	TextureProvider_bgfx* text_provider = new TextureProvider_bgfx(512, 512, _textureType, 1);
	bgfx_font::TextureAtlas* atlas = new bgfx_font::TextureAtlas(text_provider);
	bgfx_font::TextureAtlasHandle atlasHandle = g_context->fontManager.addTextureAtlas(atlas);
		
	g_context->textureProviders[g_context->textureUID] = text_provider;
	g_context->textureUID++;
}

void destroyTexture(bgfx::TextureHandle _handle)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	assert(_handle != BGFX_INVALID_HANDLE && "Invalid texture handle");
	delete g_context->textureProviders[_handle];
	g_context->textureProviders[g_context->textureUID] = text_provider;
	g_context->textureUID++;
	g_context->textureProviders[_handle] = NULL;
}
		

TrueTypeHandle loadTrueTypeFont(const char * _fontPath)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

TrueTypeHandle loadTrueTypeFont(const bgfx::Memory* _mem)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

void unloadTrueTypeFont(TrueTypeHandle _handle)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}
		
FontHandle getFontByPixelSize(TrueTypeHandle _handle, uint32_t _pixelSize, FontType _fontType)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}
	
FontHandle getFontByEmSize(TrueTypeHandle _handle, uint32_t _emSize, FontType _fontType)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

FontHandle loadBakedFont(const char * _fontPath, const char * _fontName)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

bool preloadGlyph(FontHandle _handle, const wchar_t* _string)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

void bakeAndSaveFont(FontHandle _handle, const char * _fontPath, const char * _fontName)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

TextBufferHandle createTextBuffer(FontType _type, BufferType bufferType, uint32_t _maxCharacterCount)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

void destroyTextBuffer(TextBufferHandle _handle)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

void setTextStyle(TextBufferHandle _handle, uint32_t flags)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

void setTextColor(TextBufferHandle _handle, uint32_t _rgba)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}
	
void setTextBackgroundColor(TextBufferHandle _handle, uint32_t rgba)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

void setOverlineColor(TextBufferHandle _handle, uint32_t rgba)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

void setUnderlineColor(TextBufferHandle _handle, uint32_t rgba)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}
	
void setStrikeThroughColor(TextBufferHandle _handle, uint32_t rgba )
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}
	
void setPenPosition(TextBufferHandle _handle, float x, float y)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}
	
void appendText(TextBufferHandle _handle, const char * _string)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

void appendText_wchar(TextBufferHandle _handle, const wchar_t * _string)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

void appendTextPrintf(TextBufferHandle _handle, const char * format, ...)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

void appendTextPrintf_wchar(TextBufferHandle _handle, const char * format, ...)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

void clearTextBuffer(TextBufferHandle _handle)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}
	
void submitTextBuffer(TextBufferHandle _handle, uint8_t _id,  int32_t _depth)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

void submitTextBufferMask(TextBufferHandle _handle, uint32_t _viewMask, int32_t _depth)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
}

}

