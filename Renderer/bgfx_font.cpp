#include "bgfx_font.h"
#include "FontManager.h"
#include <assert.h>
#include <bx/macros.h>
#include <unordered_map>

namespace bgfx_text
{

//engine abstraction
class TextureProvider_bgfx : public bgfx_font::ITextureProvider
{
public:
    TextureProvider_bgfx(uint16_t width, uint16_t height, bgfx_font::TextureType type, uint32_t depth): m_width(width), m_height(height), m_type(type), m_depth(depth)
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

struct TextureCache
{
	TextureProvider_bgfx* textureProvider;
	bgfx_font::TextureAtlas* atlas;
};

struct BufferCache
{

};

typedef std::unordered_map<uint16_t, TextureCache> TextureMap_t ;

typedef std::unordered_map<uint16_t, BufferCache> BufferMap_t;

struct Context
{
	Context(){}
	
	bgfx_font::FontManager fontManager;
	//shaders program
	bgfx::ProgramHandle basicProgram;	
	
	//vertex & index Buffers
	bgfx::VertexDecl vertexDecl;

	TextureMap_t textureProviders;

	uint32_t bufferUID;
	BufferMap_t bufferProviders;

};
	
static Context* g_context = NULL;

long int fsize(FILE* _file)
{
	long int pos = ftell(_file);
	fseek(_file, 0L, SEEK_END);
	long int size = ftell(_file);
	fseek(_file, pos, SEEK_SET);
	return size;
}

static const bgfx::Memory* loadShader(const char* _shaderPath, const char* _shaderName)
{
	char out[512];
	strcpy(out, _shaderPath);
	strcat(out, _shaderName);
	strcat(out, ".bin");

	FILE* file = fopen(out, "rb");
	if (NULL != file)
	{
		uint32_t size = (uint32_t)fsize(file);
		const bgfx::Memory* mem = bgfx::alloc(size+1);
		size_t ignore = fread(mem->data, 1, size, file);
		BX_UNUSED(ignore);
		fclose(file);
		mem->data[mem->size-1] = '\0';
		return mem;
	}

	return NULL;
}


void init(const char* shaderPath)
{
	assert(g_context == NULL && "A context can only be initialized once");
	g_context = new Context();

	const bgfx::Memory* mem;
	mem = loadShader(shaderPath, "vs_font_basic");
	bgfx::VertexShaderHandle vsh = bgfx::createVertexShader(mem);
	mem = loadShader(shaderPath, "fs_font_basic");
	bgfx::FragmentShaderHandle fsh = bgfx::createFragmentShader(mem);
	
	g_context->basicProgram = bgfx::createProgram(vsh, fsh);
	bgfx::destroyVertexShader(vsh);
	bgfx::destroyFragmentShader(fsh);
}
		
void shutdown()
{
	assert(g_context != NULL && "Context not initialized");
	assert(g_context->textureProviders.empty() && "A texture isn't properly destroyed.");

	bgfx::destroyProgram(g_context->basicProgram);
	delete g_context;
	g_context = NULL;
}

bgfx::TextureHandle createTexture(TextureType _textureType, uint16_t _width, uint16_t _height)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	g_context = new Context();

	TextureCache cache;
	cache.textureProvider = new TextureProvider_bgfx(512, 512, (bgfx_font::TextureType) _textureType, 1);
	cache.atlas = new bgfx_font::TextureAtlas(cache.textureProvider);

	bgfx_font::TextureAtlasHandle atlasHandle = g_context->fontManager.addTextureAtlas(cache.atlas);
		
	g_context->textureProviders[cache.textureProvider->m_handle.idx] = cache;
	return cache.textureProvider->m_handle;
}

void destroyTexture(bgfx::TextureHandle _handle)
{
	assert( g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	assert( (_handle.idx != -1) && "Invalid texture handle");

	
	TextureMap_t::iterator iter = g_context->textureProviders.find(_handle.idx);
	assert( (iter != g_context->textureProviders.end()) && "Invalid texture handle");

	delete (*iter).second.atlas;
	delete (*iter).second.textureProvider;
	g_context->textureProviders.erase(iter);	
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
	return BGFX_INVALID_HANDLE;
}

void unloadTrueTypeFont(TrueTypeHandle _handle)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	g_context->fontManager.unLoadTrueType((bgfx_font::TrueTypeHandle)_handle);	
}
		
FontHandle getFontByPixelSize(TrueTypeHandle _handle, uint32_t _pixelSize, FontType _fontType)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	return (FontHandle) g_context->fontManager.getFontByPixelSize(_handle, (float) _pixelSize, _fontType);
}
	
FontHandle getFontByEmSize(TrueTypeHandle _handle, uint32_t _emSize, FontType _fontType)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	return (FontHandle) g_context->fontManager.getFontByEmSize(_handle, (float) _emSize, _fontType);
}

FontHandle loadBakedFont(const char * _fontPath, const char * _fontName)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	assert(false && "Not implemented yet");
	return BGFX_INVALID_HANDLE;
}

bool preloadGlyph(FontHandle _handle, const wchar_t* _string)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	 g_context->fontManager.preloadGlyph(_handle, _string);
}

void bakeAndSaveFont(FontHandle _handle, const char * _fontPath, const char * _fontName)
{
	assert(g_context != NULL && "Context not initialized. Call bgfx_text::init(); ");
	assert(false && "Not implemented yet");
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

