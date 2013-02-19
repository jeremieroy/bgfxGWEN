#pragma once
/// true type font loader and glyph baker
/// use stb_truetype

#include <stdint.h> // uint32_t
struct stbtt_fontinfo; //forward decl.

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
	/// scale to apply to unscaled coordinates
	float scale;

	/// The unscaled coordinate above the baseline the font extends (typically positive)
	int16_t ascender;
	/// The unscaled coordinate below the baseline the font extends (typically negative)
	int16_t descender;
	/// The unscaled spacing between one row's descent and the next row's ascent
	int16_t lineGap;
	/// Index for faster retrieval
	uint16_t fontIndex;

	/// opaque data
	void* opaque_data;
};

/// A structure that describe a glyph.
//TODO separate what is font specific from what's not
struct GlyphInfo
{
	GlyphInfo(){}
	GlyphInfo(int32_t _glyphIndex, uint16_t _width, uint16_t _height, int16_t _offset_x, int16_t _offset_y, int16_t _advance_x, int16_t _advance_y )
		: glyphIndex(_glyphIndex), width(_width), height(_height), offset_x(_offset_x), offset_y(_offset_y), advance_x(_advance_x), advance_y(_advance_y) {}
    
	/// Index for faster retrieval
	int32_t glyphIndex;

	/// Glyph's width in pixels.
    uint16_t width;
	/// Glyph's height in pixels.
	uint16_t height;

	/// Glyph X coordinate in the atlas in pixels
	//uint16_t texture_x;
	/// Glyph Y coordinate in the atlas in pixeles
	//uint16_t texture_y;
	
	/// Glyph's left offset in unscaled pixels
    int16_t offset_x;

	/// Glyphs's top offset in unscaled pixels
    /// Remember that this is the distance from the baseline to the top-most
    /// glyph scanline, upwards y coordinates being positive.
    int16_t offset_y;

	/// For horizontal text layouts, this is the unscaled horizontal distance 
	/// used to increment the pen position when the glyph is drawn as part of a string of text.
	int16_t advance_x;
	
	/// For vertical text layouts, this is the unscaled vertical distance in unscaled coordinates 
    /// used to increment the pen position when the glyph is drawn as part of a string of text.
	int16_t advance_y;
	
		
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

	/// Can be initialized with an external buffer
	/// @remark The ownership of the buffer stays external
	/// @return true if the initialization succeed
    bool initFromBuffer(const char* extBuffer);

	/// Can be initialized with a file path, in that case, a buffer is allocated.
	/// @return true if the initialization succeed
	bool initFromFile(const char * fontPath);
	
	//    TODO add family etc...
	/// return NULL if the font doesnt exist
	const FontInfo* getFontInfo(float pixelSize, uint32_t fontIndex);

	/// return the details of a glyph
	GlyphInfo getGlyphInfo(const FontInfo& fontInfo, CodePoint_t codePoint);

	/// raster a glyph as 8bit alpha to a memory buffer
    void bakeGlyphAlpha(const FontInfo& fontInfo, const GlyphInfo& glyphInfo, uint8_t* outBuffer);
	/// raster a glyph as 32bit rgba to a memory buffer
    void bakeGlyphHinted(const FontInfo& fontInfo, const GlyphInfo& glyphInfo, uint32_t* outBuffer);
private:
	FontInfo* m_fonts; 
	uint32_t m_fontCount;
	const char* m_fileBuffer;
	bool m_ownBuffer;
};

}
