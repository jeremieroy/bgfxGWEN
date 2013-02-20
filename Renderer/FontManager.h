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
#include "RectanglePacker.h"

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

/// engine abstraction
class ITextureProvider
{
public:
	enum TextureType
	{
		ALPHA,
		HINTED,
		DISTANCE_FIELD
	};

    virtual uint16_t getWidth() = 0;
    virtual uint16_t getHeight() = 0;
    virtual uint32_t getDepth() = 0;
	virtual TextureType getType() = 0;
    
    virtual void update(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t* data) = 0;
    virtual void clear() = 0;
};

typedef uint32_t FontHandle;
typedef uint32_t TrueTypeHandle;
const uint32_t INVALID_HANDLE = -1;

class FontManager
{
public:
	FontManager(ITextureProvider* texture, uint32_t maxGlyphBitmapSize = 64);
	~FontManager();

	/// load a TrueType font from a file path
	/// @return INVALID_HANDLE if the loading fail
	TrueTypeHandle loadTrueTypeFromFile(const char* fontPath);

	/// load a TrueType font from a given buffer.
	/// the buffer must stays valid until the font is unloaded or the FontManager is destroyed
	/// @return INVALID_HANDLE if the loading fail
	TrueTypeHandle loadTrueTypeFromMemory(const char* buffer, uint32_t size);

	/// unload a TrueType font (free font memory) but keep loaded glyphs
	void unLoadTrueType(TrueTypeHandle handle);

	/// return a font descriptor whose height is a pixel size
	FontHandle getFontByPixelSize(TrueTypeHandle handle, float pixelSize );
	
	/// return a font descriptor whose height is an em size
	FontHandle getFontByEmSize(TrueTypeHandle handle, float emSize );

	/// Preload a set of glyphs from a TrueType file
	/// @return true if every glyph could be preloaded, false otherwise	
	/// if the Font is a baked font, this only do validation on the characters
	bool preloadGlyph(FontHandle handle, const wchar_t* _string);

	/// load a baked font (the set of glyph is fixed)
	/// @return INVALID_HANDLE if the loading fail
	FontHandle loadBakedFontFromFile(const char* imagePath, const char* descriptorPath);

	/// load a baked font (the set of glyph is fixed)
	/// @return INVALID_HANDLE if the loading fail
	FontHandle loadBakedFontFromMemory(const char* imageBuffer, uint32_t imageSize, const char* descriptorBuffer, uint32_t descriptorSize);

	/// bake a font to disk (the set of preloaded glyph)
	/// @return true if the baking succeed, false otherwise
	bool saveBakedFont(FontHandle handle, const char* fontDirectory, const char* fontName );
	
	/// Return the rendering informations about the glyph region
	/// Load the glyph from a TrueType font if possible
	/// @return true if the Glyph is available
	bool getGlyphInfo(FontHandle fontHandle, uint32_t codePoint, GlyphInfo& outInfo);

	/// Return a 4x4 glyph entirely opaque or white opaque.
	const GlyphInfo& getFillerGlyph() { return m_fillerGlyph; }
	
	/// return the font descriptor of a font
	/// @remark the handle is required to be valid
	FontInfo& getFontInfo(FontHandle handle){ return m_cachedFonts[handle]->fontInfo; }

private:
	uint32_t m_width;
    uint32_t m_height;
	uint32_t m_depth;
	RectanglePacker m_rectanglePacker;
	ITextureProvider* m_texture;

	GlyphInfo m_fillerGlyph;

	typedef stl::unordered_map<uint32_t, GlyphInfo> GlyphHash_t;

	// cache font data
	struct CachedFont
	{
		CachedFont( const FontInfo& _fontInfo, TrueTypeFont* _trueTypeFont = NULL)
		:fontInfo(_fontInfo), trueTypeFont(_trueTypeFont){}
		~CachedFont(){ delete trueTypeFont; trueTypeFont = NULL; }

		FontInfo fontInfo;
		TrueTypeFont* trueTypeFont;
		GlyphHash_t cachedGlyphs;
	};
	stl::vector<CachedFont*> m_cachedFonts;
	
	uint8_t* m_buffer;
};

}
