#include "TextBuffer.h"
#include "FontManager.h"

#include <assert.h>

namespace bgfx_font
{

const size_t MAX_BUFFERED_CHARACTERS = 8192;

TextBuffer::TextBuffer()
{	
	m_styleFlags = STYLE_NORMAL;

	m_textColor = 0x000000FF;
	m_backgroundColor = 0xFFFFFFFF;
	m_backgroundColor = 0x000000FF;
	m_overlineColor = 0x000000FF;
	m_underlineColor = 0x000000FF;
	m_strikeThroughColor = 0x000000FF;
	m_penX = 0;
	m_penY = 0;
	m_originX = 0;
	m_originY = 0;
	m_lineAscender = 0;
	m_lineDescender = 0;
	m_fontManager = NULL;
	m_vertexBuffer = new TextVertex[MAX_BUFFERED_CHARACTERS * 4];
	m_indexBuffer = new uint16_t[MAX_BUFFERED_CHARACTERS * 6];
	m_vertexCount = 0;
	m_indexCount = 0;
	m_lineStartIndex = 0;
}

TextBuffer::~TextBuffer()
{
	delete[] m_vertexBuffer;
	delete[] m_indexBuffer;
}

void TextBuffer::setFontManager(FontManager* fontManager)
{
	m_fontManager = fontManager;
}

void TextBuffer::appendText(FontHandle fontHandle, const char * _string)
{

}

void TextBuffer::appendText(FontHandle fontHandle, const wchar_t * _string)
{
	GlyphInfo glyph;
	const FontInfo& font = m_fontManager->getFontInfo(fontHandle);
	//parse string
	for( size_t i=0, end = wcslen(_string) ; i < end; ++i )
	{
		
		//if glyph cached, continue
		uint32_t codePoint = _string[i];
		if(m_fontManager->getGlyphInfo(fontHandle, codePoint, glyph))
		{
			appendGlyph(codePoint, font, glyph);
		}
	}
}

void TextBuffer::appendTextPrintf(FontHandle fontHandle, const char * format, ...)
{

}

void TextBuffer::appendTextPrintf(FontHandle fontHandle, const wchar_t * format, ...)
{

}
	
void TextBuffer::clearTextBuffer()
{

}

void TextBuffer::appendGlyph(CodePoint_t codePoint, const FontInfo& font, const GlyphInfo& glyphInfo)
{
	//float gamma = glyphInfo.gamma;

	//handle newlines
	if(codePoint == L'\n' )
    {
        m_penX = m_originX;
        m_penY += m_lineDescender;
		m_lineDescender = 0;
		m_lineAscender = 0;
        m_lineStartIndex = m_vertexCount;
		return;
    }

	/*
	if( markup->font->ascender > self->line_ascender )
    {
        float y = pen->y;
        pen->y -= (markup->font->ascender - self->line_ascender);
        text_buffer_move_last_line( self, (int)(y-pen->y) );
        self->line_ascender = markup->font->ascender;
    }
    if( markup->font->descender < self->line_descender )
    {
        self->line_descender = markup->font->descender;
    }
	*/

	//handle kerning
	float kerning = 0;
	/*	
    if( previous && markup->font->kerning )
    {
        kerning = texture_glyph_get_kerning( glyph, previous );
    }
	*/
    m_penX += kerning;




	// TODO handle background
/*
	// if background not invisible
	if(m_backgroundColor & 0x000000FF)
	{
		int16_t x0 = ( m_penX - kerning );
		int16_t y0 = ( m_penY + fontDescender );

		int16_t x1 = (int16_t)( (float)x0 + glyphInfo.advance_x );
		int16_t y1 = (int)( y0 + fontHeight + fontLineGap );
		float s0 = black.s0;
		float t0 = black.t0;
		float s1 = black.s1;
		float t1 = black.t1;
		uint32_t color = m_backgroundColor;

		m_vertexBuffer[m_vertexCount] .set(x0, y0, s0, t0, color
			SET_GLYPH_VERTEX(vertices[vcount+0],
			(int)x0,y0,0,  s0,t0,  r,g,b,a,  x0-((int)x0), gamma );
		SET_GLYPH_VERTEX(vertices[vcount+1],
			(int)x0,y1,0,  s0,t1,  r,g,b,a,  x0-((int)x0), gamma );
		SET_GLYPH_VERTEX(vertices[vcount+2],
			(int)x1,y1,0,  s1,t1,  r,g,b,a,  x1-((int)x1), gamma );
		SET_GLYPH_VERTEX(vertices[vcount+3],
			(int)x1,y0,0,  s1,t0,  r,g,b,a,  x1-((int)x1), gamma );
		indices[icount + 0] = vcount+0;
		indices[icount + 1] = vcount+1;
		indices[icount + 2] = vcount+2;
		indices[icount + 3] = vcount+0;
		indices[icount + 4] = vcount+2;
		indices[icount + 5] = vcount+3;
		vcount += 4;
		icount += 6;
	}
	*/


	// TODO handle underline

	// TODO handle overline

	// TODO handle strikethrough


	//handle glyph	

	float x0_precise = m_penX + glyphInfo.offset_x * font.scale;
	int16_t x0 = (int16_t)( x0_precise);
	int16_t y0 = (int16_t)( m_penY + glyphInfo.offset_y * font.scale );
	int16_t x1 = (int16_t)( x0 + glyphInfo.width );
	int16_t y1 = (int16_t)( y0 - glyphInfo.height );

	float shift = x0_precise - x0;
	
	uint16_t s0 = glyphInfo.texture_x;
	uint16_t t0 = glyphInfo.texture_y;
	uint16_t s1 = s0 + glyphInfo.width;
	uint16_t t1 = t0 + glyphInfo.height;

	m_vertexBuffer[m_vertexCount+0].set( x0, y0, s0, t0, m_textColor);
	m_vertexBuffer[m_vertexCount+1].set( x0, y1, s0, t1, m_textColor);
	m_vertexBuffer[m_vertexCount+2].set( x1, y1, s1, t1, m_textColor);
	m_vertexBuffer[m_vertexCount+3].set( x1, y0, s1, t0, m_textColor);

	m_indexBuffer[m_indexCount + 0] = m_vertexCount+0;
	m_indexBuffer[m_indexCount + 1] = m_vertexCount+1;
	m_indexBuffer[m_indexCount + 2] = m_vertexCount+2;
	m_indexBuffer[m_indexCount + 3] = m_vertexCount+0;
	m_indexBuffer[m_indexCount + 4] = m_vertexCount+2;
	m_indexBuffer[m_indexCount + 5] = m_vertexCount+3;
	m_vertexCount += 4;
	m_indexCount += 6;

	//vertex_buffer_push_back( buffer, vertices, vcount, indices, icount );
	m_penX += (glyphInfo.advance_x) * font.scale;
	/*
	texture_glyph_t *black = texture_font_get_glyph( font, -1 );


	float m_penX;
	float m_penY;

	float endX = 
	float endY =
	uint16_t s0, uint16_t t0, uint16_t s1, uint16_t t1
	uint32_t rgba = 0x000000FF;
	float startX = m_penX + glyphInfo.advance_x
	float startY = m_penY +

	m_vertexBuffer->addQuad(

	m_penX + glyphInfo.advance_x;
	m_penX + glyphInfo.advance_y;

	Vertex* m_vertexBuffer;
	uint16_t m_indexBuffer;

        return;

	//compute position
	//compute u/v coordinates
	//append textcolor	
*/
}

}
