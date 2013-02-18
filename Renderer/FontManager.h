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
#include <vector>
#include "TrueTypeFont.h"
#include "RectanglePacker.h"
#include <unordered_map>

namespace bgfx_font
{
const size_t MAX_FONT_SIZE = 64;
enum FontFlag
{
	FONT_NORMAL = 0,
	FONT_ITALIC = 1,
	FONT_BOLD   = 2
};

enum FontType
{
	ALPHA,
	HINTED,
	DISTANCE_FIELD
};

/// engine abstraction
class ITextureProvider
{
public:
    virtual uint16_t getWidth() = 0;
    virtual uint16_t getHeight() = 0;
    virtual uint32_t getDepth() = 0;
    
    virtual void update(Rect16 rect, const uint8_t* data) = 0;
    virtual void clear() = 0;
};

typedef int32_t FontHandle;
const int32_t INVALID_FONT_HANDLE = -1;

class FontManager
{
public:
	FontManager(ITextureProvider* texture);
	~FontManager();
	/// load a truetype font
	/// @return INVALID_FONT_HANDLE if the loading fail
	FontHandle loadTrueTypeFont(const char* fontPath, uint16_t fontFlags, uint16_t pixelSize);

	/// unload a truetype font (free font memory) but keep loaded glyphs
	void unLoadTrueTypeFont(FontHandle handle);

	//FontHandle searchTrueType(const char* fontPath);
	
	/// load a baked font
	/// @return INVALID_FONT_HANDLE if the loading fail
	FontHandle loadBakedFont(const char* fontPath);

	/// bake a font to disk (the set of preloaded glyph)
	/// @return true if the baking succeede, false oherwise
	bool saveBakedFont(FontHandle handle, const char* fontDirectory, const char* fontName );

	/// Preload a set of glyphs
	/// @return true if every glyph could be preloaded, false otherwise
	/// if the Font is a baked font, this only do validation.
	bool preloadGlyph(FontHandle handle, const wchar_t* _string);
	
	/// Return the rendering informations about the glyph region
	/// Load the glyph from a truetype font if possible
	/// @return true if the Glyph is available
	bool getGlyphInfo(FontHandle fontHandle, uint32_t codePoint, GlyphInfo& outInfo);	

private:

	uint32_t m_width;
    uint32_t m_height;
	uint32_t m_depth;
	RectanglePacker m_rectanglePacker;
	ITextureProvider* m_texture;
		
	typedef std::unordered_map<uint32_t, GlyphInfo> GlyphHash_t;

	/// font caching
	struct CachedFont
	{
		CachedFont(uint16_t _pixelSize = 0, uint16_t _flags = 0, TrueTypeFont* _trueTypeFont = NULL):pixelSize(_pixelSize), flags(_flags), trueTypeFont(_trueTypeFont){}
		~CachedFont(){ delete trueTypeFont; trueTypeFont = NULL; }
				
		uint16_t pixelSize;
		uint16_t flags;
		TrueTypeFont* trueTypeFont;

		//float emSize;
		//float scale;
		GlyphHash_t cachedGlyphs;
	};
	std::vector<CachedFont*> m_cachedFonts;
	
	/*
	/// file buffer caching	
	struct FontBuffer
	{		
		FontBuffer(const char* filePath, size_t fileSize);
		~FontBuffer();
		char filePath[512];
		char* buffer;
	};
	std::vector<FontBuffer*> m_cachedBuffer;
	*/
	
	uint8_t* m_buffer;
	
};

}
