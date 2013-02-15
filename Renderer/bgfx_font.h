#pragma once

namespace bgfx_font
{
	enum FontType
	{
		FONT_TYPE_ALPHA, // uint8
		FONT_TYPE_DISTANCE_FIELD, //uint8
		FONT_TYPE_RGBA, //0xRRGGBBAA
	};

	/// create a Glyph stash (a.k.a. Font fexture Atlas)
	/// @param _width width of the texture to be used
	/// @param _height height of the texture to be used
	/// @param _depth number of bytes of a pixel. 1 for FONT_TYPE_ALPHA and FONT_TYPE_DISTANCE_FIELD, 4 for FONT_TYPE_RGBA
	/// @remark the texture is owned by the glyph stash and should not be destroyed
	GlyphStashHandle createGlyphStash( uint16_t _width, uint16_t _height, uint32_t _depth );
	
	/// return the portion of the texture that is used (0 == empty 1.0 == full)
	float getGlyphStashUsageRatio(GlyphStashHandle _handle);
	
	/// return a handle to the texture used by the glyph stash (e.g. for rendering)
	bgfx::TextureHandle getGlyphStashTexture(GlyphStashHandle _handle);
	
	/// destroy the glyph stash and the associated texture
	void destroyGlyphStash(GlyphStashHandle _handle);

	/// Create a dynamic font from a truetype file, glyph will generated and added to the stash as needed. (most versatile but slowest)
	/// @param _handle    A handle to the glyph stash to be used for this font
	/// @param _type,     Type of the font 
	/// @param _fontPath  A font filename
	/// @param _size      Size of the font to be created (in points)
	FontHandle createTrueTypeFont( GlyphStashHandle _handle, FontType _type, const char * _fontPath, const float _size );

	/// preload a set of glyph into the stash
	void preloadGlyph(FontHandle _handle, const wchar_t* _string);
	
	/// bake a font and save it to disk
	void bakeAndSaveFont(FontHandle _handle, const char* _outputDir, const char* _fontName);

	/// create a static font with a fixed set of available glyph from a previously baked font. (fastest but limited)
	FontHandle createBakedFont(GlyphStashHandle _handle, const char* _inputDir, const char* _fontName);

	/// create a text buffer with the specified font type. It will only be compatible with font of the same type.
	TextBufferHandle createTextBuffer(FontType _type, uint32_t _maxCharacterCount);
	void destroyTextBuffer(TextBufferHandle _handle);

	/// because getTextBufferBuffer would be silly...
	const Memory* getTextBufferContent(TextBufferHandle _handle);

	/// Set the color state of the text to be added to the buffer
	void setTextColor(TextBufferHandle _handle, uint32_t _rgba = 0x000000ff);

	/// Set the position of the pen
	void setTextPen(TextBufferHandle _handle, float x, float y);

	/// append an ASCII/utf-8 string to the buffer using current pen position and color
	void appendText(TextBufferHandle _handle, const char * _string);

	/// append a wide char unicode string to the buffer using current pen position and color
	void appendText_wchar(TextBufferHandle _handle, const wchar_t * _string);

	/// append an ASCII/utf-8 string to the buffer using printf formatting and current pen position and color
	void appendTextPrintf(TextBufferHandle _handle, const char * format, ...);

	/// appenda wide char unicode string to the buffer using printf formatting and current pen position and color
	void appendTextPrintf_wchar(TextBufferHandle _handle, const char * format, ...);

	/// Clear the text buffer and reset its state (pen/color)
	void clearTextBuffer(TextBufferHandle _handle);

	///  initialize bgfx_font library
	///  Create font rendering shader program, and vertex format.
	///  @remark assume bgfx is initialized
	void init();

	///  shutdown bgfx_font library
	///  @remark assume bgfx is still initialized
	void shutdown();

	/// submit the text buffer for rendering
	void submitTextBuffer(uint8_t _id, int32_t _depth = 0);

	/// Submit the text buffer for rendering into multiple views.
	void submitTextBufferMask(uint32_t _viewMask, int32_t _depth = 0);

	/// TODO think about transient vs static text buffer
}
