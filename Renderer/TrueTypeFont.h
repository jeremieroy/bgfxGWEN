#pragma once

#include <stdint.h> // uint32_t

namespace bgfx_font
{

// Glyph metrics:
// --------------
//
//                       xmin                     xmax
//                        |                         |
//                        |<-------- width -------->|
//                        |                         |    
//              |         +-------------------------+----------------- ymax
//              |         |    ggggggggg   ggggg    |     ^        ^
//              |         |   g:::::::::ggg::::g    |     |        | 
//              |         |  g:::::::::::::::::g    |     |        | 
//              |         | g::::::ggggg::::::gg    |     |        | 
//              |         | g:::::g     g:::::g     |     |        | 
//    offset_x -|-------->| g:::::g     g:::::g     |  offset_y    | 
//              |         | g:::::g     g:::::g     |     |        | 
//              |         | g::::::g    g:::::g     |     |        | 
//              |         | g:::::::ggggg:::::g     |     |        |  
//              |         |  g::::::::::::::::g     |     |      height
//              |         |   gg::::::::::::::g     |     |        | 
//  baseline ---*---------|---- gggggggg::::::g-----*--------      |
//            / |         |             g:::::g     |              | 
//     origin   |         | gggggg      g:::::g     |              | 
//              |         | g:::::gg   gg:::::g     |              | 
//              |         |  g::::::ggg:::::::g     |              | 
//              |         |   gg:::::::::::::g      |              | 
//              |         |     ggg::::::ggg        |              | 
//              |         |         gggggg          |              v
//              |         +-------------------------+----------------- ymin
//              |                                   |
//              |------------- advance_x ---------->|

typedef int32_t CodePoint_t;

struct FontInfo
{		
	/// The unscaled coordinate above the baseline the font extends (typically positive)
	int16_t ascender;
	/// The unscaled coordinate below the baseline the font extends (typically negative)
	int16_t descender;
	/// The unscaled spacing between one row's descent and the next row's ascent
	int16_t lineGap;
	/// Categorie of texture container for this font
	int16_t textureType;

	/// scale to apply to unscaled coordinates
	float scale;	
};

/// A structure that describe a glyph.
/// TODO handle kerning
struct GlyphInfo
{	
	/// Index for faster retrieval
	int32_t glyphIndex;
	
	/// Glyph's width in pixels.
    uint16_t width;

	/// Glyph's height in pixels.
	uint16_t height;
	
	/// Glyph's left offset in unscaled pixels
    int16_t offset_x;

	/// Glyph's top offset in unscaled pixels
    /// Remember that this is the distance from the baseline to the top-most
    /// glyph scan line, upwards y coordinates being positive.
    int16_t offset_y;

	/// For horizontal text layouts, this is the unscaled horizontal distance 
	/// used to increment the pen position when the glyph is drawn as part of a string of text.
	int16_t advance_x;
	
	/// For vertical text layouts, this is the unscaled vertical distance in unscaled coordinates 
    /// used to increment the pen position when the glyph is drawn as part of a string of text.
	int16_t advance_y;

	/// texture coordinates if glyph is in a texture alias
	uint16_t texture_x;

	/// texture coordinates if glyph is in a texture alias
	uint16_t texture_y;

	/// Id of the texture in which this glyph is stored
	uint32_t textureIndex;
	
		
	/*
    /// A vector of kerning pairs relative to this glyph.
    vector_t * kerning;

    /// Glyph outline type (0 = None, 1 = line, 2 = inner, 3 = outer)
    int outline_type;	    
    /// Glyph outline thickness
    float outline_thickness;
	*/
};
	
class TrueTypeFont
{
public:	
	TrueTypeFont();
	~TrueTypeFont();

	/// Initialize from  an external buffer
	/// @remark The ownership of the buffer stays external, and you must ensure it stays valid up to this object lifetime
	/// @return true if the initialization succeed
    bool init(const uint8_t* buffer, uint32_t size, int32_t fontIndex = 0);

	bool isValid() { return m_font!=NULL; }

	/// return a font descriptor for a given pixelSize
	FontInfo getFontInfoByPixelSize(float pixelSize);
	
	/// return a font descriptor for a given em pixelSize
	FontInfo getFontInfoByEmSize(float emSize);

	/// return the details of a glyph
	/// return false if the glyph couldn't be found
	bool getGlyphInfo(const FontInfo& fontInfo, CodePoint_t codePoint, GlyphInfo& outGlyphInfo);

	/// raster a glyph as 8bit alpha to a memory buffer
	/// @ remark buffer min size: glyphInfo.width * glyphInfo * height * sizeof(char)
    void bakeGlyphAlpha(const FontInfo& fontInfo, const GlyphInfo& glyphInfo, uint8_t* outBuffer);

	/// raster a glyph as 32bit rgba to a memory buffer
	/// @ remark buffer min size: glyphInfo.width * glyphInfo * height * sizeof(uint32_t)
    void bakeGlyphHinted(const FontInfo& fontInfo, const GlyphInfo& glyphInfo, uint32_t* outBuffer);

private:
	void* m_font;		
};

}
