#pragma once
/// Glyph stash implementation
/// Inspired from texture-atlas from freetype-gl (http://code.google.com/p/freetype-gl/)
/// by Nicolas Rougier (Nicolas.Rougier@inria.fr)

/// The actual implementation is based on the article by Jukka Jylänki : "A
/// Thousand Ways to Pack the Bin - A Practical Approach to Two-Dimensional
/// Rectangle Bin Packing", February 27, 2010.
/// More precisely, this is an implementation of the Skyline Bottom-Left
/// algorithm based on C++ sources provided by Jukka Jylänki at:
/// http://clb.demon.fi/files/RectangleBinPack/

#include "bgfx_font_types.h"
#include "TrueTypeFont.h"
#include "RectanglePacker.h"
//#include "TextureAtlas.h"

#include <bx/handlealloc.h>
#include <bgfx.h>

#include <stdlib.h> // size_t

#if BGFX_CONFIG_USE_TINYSTL
namespace tinystl
{
	//struct bgfx_allocator
	//{
		//static void* static_allocate(size_t _bytes);
		//static void static_deallocate(void* _ptr, size_t /*_bytes*/);
	//};
} // namespace tinystl
//#	define TINYSTL_ALLOCATOR tinystl::bgfx_allocator
#	include <TINYSTL/unordered_map.h>
//#	include <TINYSTL/unordered_set.h>
namespace stl = tinystl;
#else
#	include <unordered_map>
namespace std { namespace tr1 {} }
namespace stl {
	using namespace std;
	using namespace std::tr1;
}
#endif // BGFX_CONFIG_USE_TINYSTL


namespace bgfx_font
{

class FontManager
{
public:
	FontManager();
	~FontManager();

	/// Add a texture atlas resource to the font manager
	/// The texture will be used as a texture atlas for storing glyph data
	/// The ownership of the texture provider stays external
	TextureAtlasHandle createTextureAtlas(TextureType type, uint16_t width, uint16_t height);
	
	/// retrieve a texture resource using the atlas handle
	bgfx::TextureHandle getTextureHandle(TextureAtlasHandle handle);
	void getTextureSize(TextureAtlasHandle handle, uint16_t& width, uint16_t& height);
	void getBlackGlyphUV(TextureAtlasHandle handle, int16_t& x0, int16_t& y0, int16_t& x1, int16_t& y1);
	/// destroy a texture atlas
	void destroyTextureAtlas(TextureAtlasHandle handle);

	/// load a TrueType font from a file path
	/// @return INVALID_HANDLE if the loading fail
	TrueTypeHandle loadTrueTypeFromFile(const char* fontPath, int32_t fontIndex = 0);

	/// load a TrueType font from a given buffer.
	/// the buffer must stays valid until the font is unloaded or the FontManager is destroyed
	/// @return INVALID_HANDLE if the loading fail
	TrueTypeHandle loadTrueTypeFromMemory(const uint8_t* buffer, uint32_t size, int32_t fontIndex = 0);

	/// unload a TrueType font (free font memory) but keep loaded glyphs
	void unLoadTrueType(TrueTypeHandle handle);

	/// return a font descriptor whose height is a fixed pixel size	
	FontHandle createFontByPixelSize(TrueTypeHandle handle, uint32_t pixelSize, FontType fontType = FONT_TYPE_ALPHA);
	
	/// return a font descriptor whose height is a fixed pixel size using em mapping
	FontHandle createFontByEmSize(TrueTypeHandle handle, uint32_t pixelSize, FontType fontType = FONT_TYPE_ALPHA);
		
	/// load a baked font (the set of glyph is fixed)
	/// @return INVALID_HANDLE if the loading fail
	FontHandle loadBakedFontFromFile(const char* imagePath, const char* descriptorPath);

	/// load a baked font (the set of glyph is fixed)
	/// @return INVALID_HANDLE if the loading fail
	FontHandle loadBakedFontFromMemory(const uint8_t* imageBuffer, uint32_t imageSize, const uint8_t* descriptorBuffer, uint32_t descriptorSize);

	/// destroy a font (truetype or baked)
	void destroyFont(FontHandle _handle);

	/// Preload a set of glyphs from a TrueType file
	/// @return true if every glyph could be preloaded, false otherwise	
	/// if the Font is a baked font, this only do validation on the characters
	bool preloadGlyph(FontHandle handle, const wchar_t* _string);

	bool preloadGlyph(FontHandle handle, CodePoint_t character);

	/// bake a font to disk (the set of preloaded glyph)
	/// @return true if the baking succeed, false otherwise
	bool saveBakedFont(FontHandle handle, const char* fontDirectory, const char* fontName );
	
	/// return the font descriptor of a font
	/// @remark the handle is required to be valid
	const FontInfo& getFontInfo(FontHandle handle);

	/// Return the rendering informations about the glyph region
	/// Load the glyph from a TrueType font if possible
	/// @return true if the Glyph is available
	bool getGlyphInfo(FontHandle fontHandle, CodePoint_t codePoint, GlyphInfo& outInfo);
private:
	
	
	typedef stl::unordered_map<CodePoint_t, GlyphInfo> GlyphHash_t;	
	// cache font data
	struct CachedFont
	{
		FontInfo fontInfo;
		GlyphHash_t cachedGlyphs;
		TrueTypeFont* trueTypeFont;
	};
	bx::HandleAlloc m_fontHandles;
	CachedFont* m_cachedFonts;
	
	struct CachedFile
	{
		TrueTypeFont* trueType;
		uint8_t* buffer;
	};	
	bx::HandleAlloc m_filesHandles;
	CachedFile* m_cachedFiles;	
	
	struct TextureAtlas
	{
		TextureType type;
		RectanglePacker rectanglePacker;
		bgfx::TextureHandle textureHandle;
		
		uint16_t width;
		uint16_t height;
		uint16_t depth;
		uint16_t _padding_;

		int16_t m_black_x0;
		int16_t m_black_y0;
		int16_t m_black_x1;
		int16_t m_black_y1;
	};	
	TextureAtlas* m_atlas;
	bx::HandleAlloc m_atlasHandles;	

	bgfx::TextureHandle createTexture(TextureType textureType, uint16_t width, uint16_t height);
	void destroyTexture(bgfx::TextureHandle textureHandle);
	bool addBitmap(TextureAtlas& atlas, GlyphInfo& glyphInfo, const uint8_t* data);	

	//temporary buffer to raster glyph
	uint8_t* m_buffer;
};

}
