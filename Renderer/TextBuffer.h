#pragma once

#include "FontManager.h"

namespace bgfx_font
{

struct TextVertex
{
	void set(int16_t _x, int16_t _y, uint16_t _u, uint16_t _v, uint32_t _rgba)
	{
		x = _x; y = _y; 
		u = _u; v = _v;
		rgba = _rgba;
	}
	int16_t x,y;
	uint16_t u,v;
	uint32_t rgba;
};

class TextBuffer
{
public:
	
	/// TextBuffer is bound to a fontManager for glyph retrieval
	/// @remark the ownership of the manager is not taken
	TextBuffer();
	~TextBuffer();
	
	void setFontManager(FontManager* fontManager);

	void setStyle(uint32_t flags = STYLE_NORMAL) { m_styleFlags = flags; }
	void setTextColor(uint32_t rgba = 0x000000FF) { m_textColor = rgba; }
	void setBackgroundColor(uint32_t rgba = 0x000000FF) { m_backgroundColor = rgba; }

	void setOverlineColor(uint32_t rgba = 0x000000FF) { m_overlineColor = rgba; }
	void setUnderlineColor(uint32_t rgba = 0x000000FF) { m_underlineColor = rgba; }
	void setStrikeThroughColor(uint32_t rgba = 0x000000FF) { m_strikeThroughColor = rgba; }
	
	void setPenPosition(float x, float y) { m_penX = x; m_penY = y; }

	/// append an ASCII/utf-8 string to the buffer using current pen position and color
	void appendText(FontHandle fontHandle, const char * _string);

	/// append a wide char unicode string to the buffer using current pen position and color
	void appendText(FontHandle fontHandle, const wchar_t * _string);

	/// append an ASCII/utf-8 string to the buffer using printf formatting and current pen position and color
	void appendTextPrintf(FontHandle fontHandle, const char * format, ...);

	/// append wide char unicode string to the buffer using printf formatting and current pen position and color
	void appendTextPrintf(FontHandle fontHandle, const wchar_t * format, ...);

	/// Clear the text buffer and reset its state (pen/color)
	void clearTextBuffer();
	
	/// get pointer to the vertex buffer to submit it to the graphic card
	const TextVertex* getVertexBuffer(){ return m_vertexBuffer; }
	/// number of vertex in the vertex buffer
	uint32_t getVertexCount(){ return m_vertexCount; }
	/// size in bytes of a vertex
	uint32_t getVertexSize(){ return sizeof(TextVertex); }
		
	/// get a pointer to the index buffer to submit it to the graphic
	const uint16_t* getIndexBuffer(){ return m_indexBuffer; }
	/// number of index in the index buffer
	uint32_t getIndexCount(){ return m_vertexCount; }
	/// size in bytes of an index
	uint32_t getIndexSize(){ return sizeof(uint16_t); }

private:
	void appendGlyph(CodePoint_t codePoint, const FontInfo& font, const GlyphInfo& glyphInfo);

	uint32_t m_styleFlags;

	// color states
	uint32_t m_textColor;
	uint32_t m_backgroundColor;
	uint32_t m_overlineColor;
	uint32_t m_underlineColor;
	uint32_t m_strikeThroughColor;

	//position states	
	float m_penX;
	float m_penY;

	float m_originX;
	float m_originY;	

	int16_t m_lineAscender;
	int16_t m_lineDescender;	

	///
	FontManager* m_fontManager;

	
	TextVertex* m_vertexBuffer;
	uint16_t* m_indexBuffer;
	
	size_t m_vertexCount;
	size_t m_indexCount;

	size_t m_lineStartIndex;

};

}
