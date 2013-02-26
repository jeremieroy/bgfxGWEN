#pragma once

#include "FontManager.h"

namespace bgfx_font
{



class TextBuffer
{
public:
	
	/// TextBuffer is bound to a fontManager for glyph retrieval
	/// @remark the ownership of the manager is not taken
	TextBuffer();
	~TextBuffer();
	
	void setFontManager(FontManager* fontManager);

	void setStyle(uint32_t flags = STYLE_NORMAL) { m_styleFlags = flags; }
	void setTextColor(uint32_t rgba = 0x000000FF) { m_textColor = toABGR(rgba); }
	void setBackgroundColor(uint32_t rgba = 0x000000FF) { m_backgroundColor = toABGR(rgba); }

	void setOverlineColor(uint32_t rgba = 0x000000FF) { m_overlineColor = toABGR(rgba); }
	void setUnderlineColor(uint32_t rgba = 0x000000FF) { m_underlineColor = toABGR(rgba); }
	void setStrikeThroughColor(uint32_t rgba = 0x000000FF) { m_strikeThroughColor = toABGR(rgba); }
	
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
	const uint8_t* getVertexBuffer(){ return (uint8_t*) m_vertexBuffer; }
	/// number of vertex in the vertex buffer
	uint32_t getVertexCount(){ return m_vertexCount; }
	/// size in bytes of a vertex
	uint32_t getVertexSize(){ return sizeof(TextVertex); }
		
	/// get a pointer to the index buffer to submit it to the graphic
	const uint16_t* getIndexBuffer(){ return m_indexBuffer; }
	/// number of index in the index buffer
	uint32_t getIndexCount(){ return m_indexCount; }
	/// size in bytes of an index
	uint32_t getIndexSize(){ return sizeof(uint16_t); }

	bgfx::TextureHandle getTextureHandle() { return m_textureHandle; }
	void getTextureSize(uint16_t& width, uint16_t& height) { width = m_textureWidth; height = m_textureHeight; }
	void getBlackGlyph(int16_t& x0, int16_t& y0, int16_t& x1, int16_t& y1) { x0 = m_black_x0; y0 = m_black_y0; x1 = m_black_x1; y1 = m_black_y1; }

private:
	void appendGlyph(CodePoint_t codePoint, const FontInfo& font, const GlyphInfo& glyphInfo);
	void verticalCenterLastLine(int16_t txtDecalY, uint16_t top, uint16_t bottom);
	uint32_t toABGR(uint32_t rgba) 
	{ 
		return (((rgba >> 0) & 0xff) << 24) |  
			(((rgba >> 8) & 0xff) << 16) |    
			(((rgba >> 16) & 0xff) << 8) |    
			(((rgba >> 24) & 0xff) << 0);   
	}
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
	int16_t m_lineGap;
	
	///
	FontManager* m_fontManager;
	bgfx::TextureHandle m_textureHandle;
	uint16_t padding__;
	uint16_t m_textureWidth;
	uint16_t m_textureHeight;
	int16_t m_black_x0;
	int16_t m_black_y0;
	int16_t m_black_x1;
	int16_t m_black_y1;


	void setVertex(size_t i, int16_t _x, int16_t _y, int16_t _u, int16_t _v, uint32_t _rgba, uint8_t style = STYLE_NORMAL)
	{
		m_vertexBuffer[i].x=_x;
		m_vertexBuffer[i].y=_y;
		m_vertexBuffer[i].u=_u;
		m_vertexBuffer[i].v=_v;
		m_vertexBuffer[i].rgba=_rgba;
		m_styleBuffer[i] = style;
	}

	struct TextVertex
	{		
		int16_t x,y;
		int16_t u,v;
		uint32_t rgba;
	};

	TextVertex* m_vertexBuffer;
	uint16_t* m_indexBuffer;
	uint8_t* m_styleBuffer;
	
	size_t m_vertexCount;
	size_t m_indexCount;
	size_t m_lineStartIndex;
	

};

}
