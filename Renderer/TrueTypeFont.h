#pragma once
/// true type font loader and glyph baker
/// use stb_truetype

#include <stdint.h> // uint32_t

struct stbtt_fontinfo;

namespace bgfx_font
{
    struct GlyphSize
    {
        uint16_t width;
        uint16_t height;
    };

	typedef int16_t FontHandle;

	/*
 * Glyph metrics:
 * --------------
 *
 *                       xmin                     xmax
 *                        |                         |
 *                        |<-------- width -------->|
 *                        |                         |    
 *              |         +-------------------------+----------------- ymax
 *              |         |    ggggggggg   ggggg    |     ^        ^
 *              |         |   g:::::::::ggg::::g    |     |        | 
 *              |         |  g:::::::::::::::::g    |     |        | 
 *              |         | g::::::ggggg::::::gg    |     |        | 
 *              |         | g:::::g     g:::::g     |     |        | 
 *    offset_x -|-------->| g:::::g     g:::::g     |  offset_y    | 
 *              |         | g:::::g     g:::::g     |     |        | 
 *              |         | g::::::g    g:::::g     |     |        | 
 *              |         | g:::::::ggggg:::::g     |     |        |  
 *              |         |  g::::::::::::::::g     |     |      height
 *              |         |   gg::::::::::::::g     |     |        | 
 *  baseline ---*---------|---- gggggggg::::::g-----*--------      |
 *            / |         |             g:::::g     |              | 
 *     origin   |         | gggggg      g:::::g     |              | 
 *              |         | g:::::gg   gg:::::g     |              | 
 *              |         |  g::::::ggg:::::::g     |              | 
 *              |         |   gg:::::::::::::g      |              | 
 *              |         |     ggg::::::ggg        |              | 
 *              |         |         gggggg          |              v
 *              |         +-------------------------+----------------- ymin
 *              |                                   |
 *              |------------- advance_x ---------->|
 */

/**
 * A structure that describe a glyph.
 */
struct GlyphInfo
{
    /// Wide character this glyph represents
    //uint32_t charcode;
	/// Glyph's width in pixels.
    uint16_t width;
	
	/// Glyph's height in pixels.
	uint16_t height;
	
	/// Glyph's left offset in pixels.
    int16_t offset_x;
	    
    /// Glyphs's top offset in pixels.
    /// Remember that this is the distance from the baseline to the top-most
    /// glyph scanline, upwards y coordinates being positive.
    int16_t offset_y;

	int16_t advance_x;

	int16_t advance_y;
	
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


// ascent is the coordinate above the baseline the font extends; descent
	// is the coordinate below the baseline the font extends (i.e. it is typically negative)
	// lineGap is the spacing between one row's descent and the next row's ascent...
	// so you should advance the vertical position by "*ascent - *descent + *lineGap"
	//   these are expressed in unscaled coordinates, so you must multiply by
	//   the scale factor for a given size
	//int ascent, descent, lineGap;
	//stbtt_GetFontVMetrics(m_stbFont, &ascent, &descent, &lineGap);



	
    class TrueTypeFont
    {
    public:
        TrueTypeFont();
        ~TrueTypeFont();
        bool loadFont(const char * _fontPath);
		
		GlyphInfo getGlyphInfo(uint32_t codePoint, float size, float shift_x = 0.0f, float shift_y = 0.0f);
		GlyphSize getGlyphSize(uint32_t codePoint, float size);

        void bakeGlyphAlpha(uint32_t codePoint, float size, uint8_t* outBuffer);
        void bakeGlyphHinted(uint32_t codePoint, float size, uint32_t* outBuffer);

        stbtt_fontinfo* m_stbFont;
		void* m_buffer;
	};	

}
