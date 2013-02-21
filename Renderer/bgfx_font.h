#pragma once
#include <bgfx.h> 

namespace bgfx_text
{
	struct TrueTypeHandle { uint16_t idx; };
	struct FontHandle { uint16_t idx; };
	struct TextBufferHandle{ uint16_t idx; };
	//const uint16_t INVALID_HANDLE = { UINT16_MAX };
	
	/// Type of rendering used for the font (determine texture format and shader type)
	enum FontType
	{
		FONT_TYPE_ALPHA    = 0, // L8
		FONT_TYPE_HINTED   = 1, // BGRA8
		FONT_TYPE_RGBA     = 2, // BGRA8
		FONT_TYPE_DISTANCE = 3  // L8
	};

	/// Type of texture supported for text
	enum TextureType
	{
		TEXTURE_TYPE_ALPHA = 0, // L8
		TEXTURE_TYPE_RGBA = 1   // BGRA8
	};

	/// type of vertex and index buffer to use with a TextBuffer
	enum BufferType
	{
		STATIC,
		DYNAMIQUE ,
		TRANSIENT
	};

	/// special style effect (can be combined)
	enum TextStyle
	{
		STYLE_NORMAL           = 0,
		STYLE_OVERLINE         = 1,
		STYLE_UNDERLINE        = 1<<1,
		STYLE_STRIKE_THROUGH   = 1<<2,
		STYLE_BACKGROUND       = 1<<3,
	};

	///  initialize bgfx_text library
	///  Create font rendering shader program, and vertex format.
	///  @remark assume bgfx is initialized
	void init();
	
	///  shutdown bgfx_text library
	///  @remark assume bgfx is (still) initialized
	void shutdown();

	///  allocate a texture of the given type and size
	bgfx::TextureHandle createTexture(TextureType _textureType, uint16_t _width, uint16_t _height);
	
	///  destroy a texture
	void destroyTexture(bgfx::TextureHandle _handle);
		
	/// Load a truetype ressource from a file, glyph can generated if the font is loaded
	TrueTypeHandle loadTrueTypeFont(const char * _fontPath);
	
	/// Load a truetype ressource from a buffer, glyph can generated if the font is loaded
	TrueTypeHandle loadTrueTypeFont(const bgfx::Memory* _mem);
	
	/// free the ressource allocated for the font (but keep loaded glyph)
	void unloadTrueTypeFont(TrueTypeHandle _handle);
		
	/// return a font descriptor to a truetype font whose height is a fixed pixel size	
	FontHandle getFontByPixelSize(TrueTypeHandle _handle, uint32_t _pixelSize, FontType _fontType = FONT_TYPE_ALPHA);
	
	/// return a font descriptor to a truetype font whose height is a fixed em size
	FontHandle getFontByEmSize(TrueTypeHandle _handle, uint32_t _emSize, FontType _fontType = FONT_TYPE_ALPHA);

	/// Load a baked font and return a font descriptor corresponding to a baked font
	FontHandle loadBakedFont(const char * _fontPath, const char * _fontName);
	//TODO load from a given buffer

	/// Preload a set of glyphs from a TrueType file
	/// @return true if every glyph could be preloaded, false otherwise
	/// if the Font is a baked font, this only do validation on the characters
	bool preloadGlyph(FontHandle _handle, const wchar_t* _string);

	/// bake a font and save it to disk
	void bakeAndSaveFont(FontHandle _handle, const char * _fontPath, const char * _fontName);
		
	/// Create a text buffer of the specified font type. It will only be compatible with fonts of the same type.
	TextBufferHandle createTextBuffer(FontType _type, BufferType bufferType, uint32_t _maxCharacterCount);
	
	/// Destroy a text buffer
	void destroyTextBuffer(TextBufferHandle _handle);

	/// Set the current style state of the text buffer
	void setTextStyle(TextBufferHandle _handle, uint32_t flags = STYLE_NORMAL);

	/// Set the current text color state of the text buffer
	void setTextColor(TextBufferHandle _handle, uint32_t _rgba = 0x000000ff);
	
	/// Set the current background color state of the text buffer
	void setTextBackgroundColor(TextBufferHandle _handle, uint32_t rgba = 0x000000FF);

	/// Set the overline color state of the text buffer
	void setOverlineColor(TextBufferHandle _handle,uint32_t rgba = 0x000000FF);

	/// Set the underline color state of the text buffer
	void setUnderlineColor(TextBufferHandle _handle,uint32_t rgba = 0x000000FF);
	
	/// Set the strike trough color state of the text buffer
	void setStrikeThroughColor(TextBufferHandle _handle,uint32_t rgba = 0x000000FF);
	
	/// Set the pen position state of the text buffer
	void setPenPosition(TextBufferHandle _handle, float x, float y);
	
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
	
	/// submit the text buffer for rendering
	void submitTextBuffer(TextBufferHandle _handle, uint8_t _id, int32_t _depth = 0);

	/// Submit the text buffer for rendering into multiple views.
	void submitTextBufferMask(TextBufferHandle _handle, uint32_t _viewMask, int32_t _depth = 0);
}

