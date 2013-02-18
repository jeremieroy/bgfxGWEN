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

/// A structure that describe a glyph.
//TODO separate what is font specific from what's not
struct GlyphInfo
{
	GlyphInfo(){}
	GlyphInfo(uint16_t _width, uint16_t _height, float _offset_x, float _offset_y, float _advance_x, float _advance_y)
		: width(_width), height(_height), offset_x(_offset_x), offset_y(_offset_y), advance_x(_advance_x), advance_y(_advance_y){}
    
	/// Index for faster retrieval
	int32_t glyphIndex;

	/// Glyph's width in pixels.
    uint16_t width;
	/// Glyph's height in pixels.
	uint16_t height;
	
	//TODO Check if I can make the offsets uint16
	/// Glyph's left offset in pixels.
    float offset_x;
	    
    /// Glyphs's top offset in pixels.
    /// Remember that this is the distance from the baseline to the top-most
    /// glyph scanline, upwards y coordinates being positive.
    float offset_y;

	float advance_x;
	float advance_y;
	
	//position in the atlas if any.
	uint16_t texture_x;
	uint16_t texture_y;
	
	/*
    /// For horizontal text layouts, this is the horizontal distance (in
    /// fractional pixels) used to increment the pen position when the glyph is
    /// drawn as part of a string of text.
    float advance_x;

    /// For vertical text layouts, this is the vertical distance (in fractional
    /// pixels) used to increment the pen position when the glyph is drawn as
    /// part of a string of text.
    //float advance_y;

	
    ///First normalized texture coordinate (x) of top-left corner
    float s0;

    /// Second normalized texture coordinate (y) of top-left corner
    float t0;

    /// First normalized texture coordinate (x) of bottom-right corner
    float s1;

    /// Second normalized texture coordinate (y) of bottom-right corner
    float t1;
	
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
	
	/// return the details of a glyph
	GlyphInfo getGlyphInfo(CodePoint_t codePoint, uint16_t pixelSize);
	/// raster a glyph as 8bit alpha to a memory buffer
    void bakeGlyphAlpha(const GlyphInfo& glyphInfo, uint16_t pixelSize, uint8_t* outBuffer);
	/// raster a glyph as 32bit rgba to a memory buffer
    void bakeGlyphHinted(const GlyphInfo& glyphInfo, uint16_t pixelSize, uint32_t* outBuffer);
private:
    stbtt_fontinfo* m_stbFont;
	const char* m_fileBuffer;
	bool m_ownBuffer;
};

}
