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

#include <stdint.h> // uint32_t
#include <stdlib.h> // size_t

#include "TrueTypeFont.h"
#include "TextureAtlas.h"

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

#	include <TINYSTL/vector.h>
#	include <TINYSTL/unordered_map.h>
//#	include <TINYSTL/unordered_set.h>
namespace stl = tinystl;
#else
#	include <vector>
#	include <unordered_map>
namespace std { namespace tr1 {} }
namespace stl {
	using namespace std;
	using namespace std::tr1;
}
#endif // BGFX_CONFIG_USE_TINYSTL


namespace bgfx_font
{

typedef uint32_t FontHandle;
typedef uint32_t TrueTypeHandle;
typedef uint32_t TextureAtlasHandle;
const uint32_t INVALID_HANDLE = -1;

class FontManager
{
public:
	FontManager(uint32_t maxGlyphBitmapSize = 64);
	~FontManager();

	/// Add a texture atlas ressource to the font manager
	/// Up to 4 atlas can be added
	/// The texture will be used as a texture atlas for storing glyph data
	/// The ownership of the texture provider stays external
	TextureAtlasHandle addTextureAtlas(TextureAtlas* atlas);

	/// retrieve a texture atlas ressource using it's handle
	TextureAtlas* getTextureAtlas(TextureAtlasHandle handle);

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
	FontHandle getFontByPixelSize(TrueTypeHandle handle, uint32_t pixelSize, TextureType textureType = TEXTURE_ALPHA);
	
	/// return a font descriptor whose height is a fiex em size
	FontHandle getFontByEmSize(TrueTypeHandle handle, uint32_t emSize, TextureType textureType = TEXTURE_ALPHA);

	/// Preload a set of glyphs from a TrueType file
	/// @return true if every glyph could be preloaded, false otherwise	
	/// if the Font is a baked font, this only do validation on the characters
	bool preloadGlyph(FontHandle handle, const wchar_t* _string);

	/// load a baked font (the set of glyph is fixed)
	/// @return INVALID_HANDLE if the loading fail
	FontHandle loadBakedFontFromFile(const char* imagePath, const char* descriptorPath);

	/// load a baked font (the set of glyph is fixed)
	/// @return INVALID_HANDLE if the loading fail
	FontHandle loadBakedFontFromMemory(const uint8_t* imageBuffer, uint32_t imageSize, const uint8_t* descriptorBuffer, uint32_t descriptorSize);

	/// bake a font to disk (the set of preloaded glyph)
	/// @return true if the baking succeed, false otherwise
	bool saveBakedFont(FontHandle handle, const char* fontDirectory, const char* fontName );
	
	/// return the font descriptor of a font
	/// @remark the handle is required to be valid
	const FontInfo& getFontInfo(FontHandle handle);

	/// Return the rendering informations about the glyph region
	/// Load the glyph from a TrueType font if possible
	/// @return true if the Glyph is available
	bool getGlyphInfo(FontHandle fontHandle, uint32_t codePoint, GlyphInfo& outInfo);
private:
	typedef stl::unordered_map<uint32_t, GlyphInfo> GlyphHash_t;

	// cache font data
	struct CachedFont
	{
		FontInfo fontInfo;
		TrueTypeFont* trueTypeFont;
		GlyphHash_t cachedGlyphs;
		uint8_t* fileBuffer;
	};	
	stl::vector<CachedFont*> m_cachedFonts;

	struct CachedFile
	{
		TrueTypeFont* trueType;
		uint8_t* buffer;
	};
	stl::vector<CachedFile> m_trueType;

	TextureAtlas* m_textures[4];
	uint32_t m_textureCount;

	uint8_t* m_buffer;
};

}

