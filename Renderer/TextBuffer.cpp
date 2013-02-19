#include "TextBuffer.h"
#include "FontManager.h"

#include <assert.h>

namespace bgfx_font
{

TextBuffer::TextBuffer(FontManager* fontManager, VertexBufferProvider* vertexBuffer, IndexBufferProvider* indexBuffer)
	: m_fontManager(fontManager), m_vertexBuffer(vertexBuffer), m_indexBuffer(indexBuffer)
{
	assert(m_fontManager != NULL);
	assert(m_vertexBuffer != NULL);
	assert(m_indexBuffer != NULL);

	m_styleFlags = TextStyleFlags::NORMAL;
	
	m_textColor = 0x000000FF;
	m_backgroundColor = 0xFFFFFFFF;
	m_backgroundColor = 0x000000FF;
	m_overlineColor = 0x000000FF;
	m_underlineColor = 0x000000FF;
	m_strikeThroughColor = 0x000000FF;
}

TextBuffer::~TextBuffer()
{
}

void TextBuffer::appendText(FontHandle font, const char * _string)
{

}

void TextBuffer::appendText(FontHandle font, const wchar_t * _string)
{
	GlyphInfo glyphInfo;
	
	//parse string		
	for( size_t i=0, end = wcslen(_string) ; i < end; ++i )
	{
		//if glyph cached, continue
		uint32_t codePoint = _string[i];
		if(m_fontManager->getGlyphInfo(font, codePoint, glyphInfo))
		{
			float x0 = glyphInfo.texture_x;
			float x0 = glyphInfo.texture_x;
		}

	}
}

void TextBuffer::appendTextPrintf(FontHandle font, const char * format, ...)
{

}

void TextBuffer::appendTextPrintf(FontHandle font, const wchar_t * format, ...)
{

}
	
void TextBuffer::clearTextBuffer()
{

}

void TextBuffer::appendGlyph(const GlyphInfo& glyphInfo)
{
	 // Maximum number of vertices is 20 (= 5x2 triangles) per glyph:
    //  - 2 triangles for background
    //  - 2 triangles for overline
    //  - 2 triangles for underline
    //  - 2 triangles for strikethrough
    //  - 2 triangles for glyph

	//float gamma = glyphInfo.gamma;

	//handle newlines
	if( glyphInfo.codePoint == L'\n' )
    {
        m_penX = m_originX;
        m_penY += m_lineDescender;
		m_lineDescender = 0.0f;
		m_lineAscender = 0.0f;
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
	

	struct UVCO
	{
		uint16_t s0, t0, s1,t1;
	} black;
#pragma message("TODO implement")

	int16_t fontDescender = 5;
	int16_t fontHeight = 12;
	int16_t fontLineGap = 2;

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

}

}
