#pragma once

#include "FontManager.h"

namespace bgfx_font
{

namespace TextStyleFlags
{
	enum
	{
		NORMAL           = 0,
		OVERLINE         = 1,
		UNDERLINE        = 1<<1,
		STRIKE_THROUGH   = 1<<2,
		COLOR_BACKGROUND = 1<<3,

		//STRIKE_THROUGH = 1<<2,
	};
};

struct VertexBufferProvider
{
	virtual void clear() = 0;
	virtual void addQuad(float startX, float startY, float endX, float endY, uint16_t s0, uint16_t t0, uint16_t s1, uint16_t t1, uint32_t rgba = 0x000000FF) = 0;
};

class TextBuffer
{
	/// TextBuffer is bound to a fontManager for glyph retrieval
	/// @remark the ownership of the manager is not taken
	TextBuffer(FontManager* fontManager, VertexBufferProvider* vertexBuffer, IndexBufferProvider* indexBuffer);
	~TextBuffer();

	void setStyle(uint32_t flags = TextStyleFlags::NORMAL);
	void setTextColor(uint32_t rgba = 0x000000FF);
	void setBackgroundColor(uint32_t rgba = 0x000000FF);

	void setOverlineColor(uint32_t rgba = 0x000000FF);
	void setUnderlineColor(uint32_t rgba = 0x000000FF);
	void setStrikeThroughColor(uint32_t rgba = 0x000000FF);	

	/// append an ASCII/utf-8 string to the buffer using current pen position and color
	void appendText(FontHandle font, const char * _string);

	/// append a wide char unicode string to the buffer using current pen position and color
	void appendText(FontHandle font, const wchar_t * _string);

	/// append an ASCII/utf-8 string to the buffer using printf formatting and current pen position and color
	void appendTextPrintf(FontHandle font, const char * format, ...);

	/// append wide char unicode string to the buffer using printf formatting and current pen position and color
	void appendTextPrintf(FontHandle font, const wchar_t * format, ...);

	/// Clear the text buffer and reset its state (pen/color)
	void clearTextBuffer();

private:
	void appendGlyph(CodePoint_t codePoint, const FontInfo& font, const BakedGlyph& glyphInfo);

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
	//VertexBufferProvider* m_vertexBuffer;
	struct Vertex
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
	
	Vertex* m_vertexBuffer;
	uint16_t* m_indexBuffer;
	
	size_t m_vertexCount;
	size_t m_indexCount;

	size_t m_lineStartIndex;

};

}
