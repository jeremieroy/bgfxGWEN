#pragma once
#include <stdint.h> // uint32_t

namespace bgfx_font
{	
	const uint16_t INVALID_HANDLE_ID = UINT16_MAX;
	#define BGFX_FONT_HANDLE(_name) struct _name { uint16_t idx; explicit _name(uint16_t _idx = INVALID_HANDLE_ID):idx(_idx){} bool isValid(){return idx != INVALID_HANDLE_ID;} }
	// @ note to myself
	// would be better if I didn't got into troubles with Initializer lists :  if (handle != { UINT16_MAX }) ...

	BGFX_FONT_HANDLE(TrueTypeHandle);
	BGFX_FONT_HANDLE(FontHandle);
	BGFX_FONT_HANDLE(TextureAtlasHandle);
	BGFX_FONT_HANDLE(TextBufferHandle);	
		
	/// Type of texture supported for text
	enum TextureType
	{
		TEXTURE_TYPE_ALPHA = 1, // L8
		TEXTURE_TYPE_RGBA = 1<<1   // BGRA8
	};

	/// Type of rendering used for the font (determine texture format and shader type)
	/// @remark Encode texture compatibility on the low bits
	enum FontType
	{
		FONT_TYPE_ALPHA    = 0x00000100 | TEXTURE_TYPE_ALPHA, // L8
		FONT_TYPE_HINTED   = 0x00000200 | TEXTURE_TYPE_RGBA,  // BGRA8
		FONT_TYPE_RGBA     = 0x00000300 | TEXTURE_TYPE_RGBA,  // BGRA8
		FONT_TYPE_DISTANCE = 0x00000400 | TEXTURE_TYPE_ALPHA  // L8
	};
	
	/// type of vertex and index buffer to use with a TextBuffer
	enum BufferType
	{
		STATIC,
		DYNAMIC ,
		TRANSIENT
	};

	/// special style effect (can be combined)
	enum TextStyleFlags
	{
		STYLE_NORMAL           = 0,
		STYLE_OVERLINE         = 1,
		STYLE_UNDERLINE        = 1<<1,
		STYLE_STRIKE_THROUGH   = 1<<2,
		STYLE_BACKGROUND       = 1<<3,
	};
	
	struct FontInfo
	{		
		/// The pixel extents above the baseline in pixels (typically positive)
		int16_t ascender;
		/// The extents below the baseline in pixels (typically negative)
		int16_t descender;
		/// The spacing in pixels between one row's descent and the next row's ascent
		int16_t lineGap;
		/// The thickness of the under/hover/striketrough line in pixels
		int16_t underline_thickness;
		/// Rendering type used for the font
		int16_t fontType;
		/// Id of the texture atlas in which the glyph's bitmap are stored
		TextureAtlasHandle textureAtlas;		

		/// scale to apply to unscaled coordinates
		float scale;	
	};

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

	/// Unicode value of a character
	typedef int32_t CodePoint_t;

	/// A structure that describe a glyph.	
	struct GlyphInfo
	{			
		/// Index for faster retrieval
		int32_t glyphIndex;
	
		/// Glyph's width in pixels.
		uint16_t width;

		/// Glyph's height in pixels.
		uint16_t height;
	
		/// Glyph's left offset in pixels
		int16_t offset_x;

		/// Glyph's top offset in pixels
		/// Remember that this is the distance from the baseline to the top-most
		/// glyph scan line, upwards y coordinates being positive.
		int16_t offset_y;

		/// For horizontal text layouts, this is the unscaled horizontal distance 
		/// used to increment the pen position when the glyph is drawn as part of a string of text.
		int16_t advance_x;
	
		/// For vertical text layouts, this is the unscaled vertical distance in unscaled coordinates 
		/// used to increment the pen position when the glyph is drawn as part of a string of text.
		int16_t advance_y;

		/// texture coordinates are expressed in a normalized ratio of the texture size 
		/// that map [0,width] and  [0,height]  to [0;32767]

		/// left texture coordinates 
		int16_t texture_x0;

		/// top texture coordinates 
		int16_t texture_y0;

		/// right texture coordinates 
		int16_t texture_x1;

		/// bottom texture coordinates 
		int16_t texture_y1;
		
		/// TODO handle kerning		
	};
	
	
}
