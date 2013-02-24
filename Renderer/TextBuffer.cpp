#include "TextBuffer.h"
#include "FontManager.h"

#include <assert.h>

namespace bgfx_font
{

const size_t MAX_BUFFERED_CHARACTERS = 8192;

TextBuffer::TextBuffer()
{	
	m_styleFlags = STYLE_NORMAL;
	//0xAABBGGRR
	m_textColor = 0xFFFFFFFF;
	m_backgroundColor = 0xFFFFFFFF;
	m_backgroundColor = 0xFFFFFFFF;
	m_overlineColor = 0xFFFFFFFF;
	m_underlineColor = 0xFFFFFFFF;
	m_strikeThroughColor = 0xFFFFFFFF;
	m_penX = 0;
	m_penY = 0;
	m_originX = 0;
	m_originY = 0;
	m_fontManager = NULL;
	m_vertexBuffer = new TextVertex[MAX_BUFFERED_CHARACTERS * 4];
	m_indexBuffer = new uint16_t[MAX_BUFFERED_CHARACTERS * 6];
	m_vertexCount = 0;
	m_indexCount = 0;
	m_lineStartIndex = 0;
	m_lineAscender = 0;
	m_lineDescender = 0;
}

TextBuffer::~TextBuffer()
{
	delete[] m_vertexBuffer;
	delete[] m_indexBuffer;
}

void TextBuffer::setFontManager(FontManager* fontManager)
{
	m_fontManager = fontManager;
	m_textureHandle.idx = bgfx::invalidHandle;
}


void TextBuffer::appendText(FontHandle fontHandle, const char * _string)
{

}

void TextBuffer::appendText(FontHandle fontHandle, const wchar_t * _string)
{	
	GlyphInfo glyph;
	const FontInfo& font = m_fontManager->getFontInfo(fontHandle);

	if( m_textureHandle.idx == bgfx::invalidHandle)
	{
		m_textureHandle = m_fontManager->getTextureHandle(font.textureAtlas);
		m_fontManager->getTextureSize(font.textureAtlas, m_textureWidth, m_textureHeight);
	}else
	{
		assert((m_textureHandle.idx ==  m_fontManager->getTextureHandle(font.textureAtlas).idx) && "You cannot mix font from different atlas in the same textbuffer");
	}
	
	if(m_vertexCount == 0)
	{
		m_originX = m_penX;
		m_originY = m_penY;
		m_lineDescender = 0;// font.descender;
		m_lineAscender = 0;//font.ascender;
	}

	//parse string
	for( size_t i=0, end = wcslen(_string) ; i < end; ++i )
	{
		if(i==4)
		{
			int yo=9;
		}
		//if glyph cached, continue
		uint32_t codePoint = _string[i];
		if(m_fontManager->getGlyphInfo(fontHandle, codePoint, glyph))
		{
			appendGlyph(codePoint, font, glyph);
		}else
		{
			assert(false && "Glyph not found");
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
	m_vertexCount = 0;
	m_indexCount = 0;
	m_lineStartIndex = 0;
	m_lineAscender = 0;
	m_lineDescender = 0;
}

void TextBuffer::appendGlyph(CodePoint_t codePoint, const FontInfo& font, const GlyphInfo& glyphInfo)
{
	//float gamma = glyphInfo.gamma;

	//handle newlines
	if(codePoint == L'\n' )
    {
        m_penX = m_originX;		
        m_penY += m_lineDescender;
		m_lineDescender = 0;// font.descender * font.scale;
		m_lineAscender = 0;//font.ascender* font.scale;
        m_lineStartIndex = m_vertexCount;
		return;
    }

	if( font.ascender * font.scale > m_lineAscender )
    {
        float y = m_penY;
        m_penY -= ((font.ascender  * font.scale) - m_lineAscender);
		//text_buffer_move_last_line( self, (int)(y-pen->y) );
        m_lineAscender = (font.ascender * font.scale);
    }

    if( font.descender * font.scale < m_lineDescender )
    {
        m_lineDescender = font.descender * font.scale;
    }
			
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

	float x0_precise = m_penX + (glyphInfo.offset_x);// * font.scale);
	int16_t x0 = (int16_t)( x0_precise);
	int16_t y0 = (int16_t)( m_penY - (glyphInfo.offset_y));// * font.scale ));
	int16_t x1 = (int16_t)( x0 + glyphInfo.width );
	int16_t y1 = (int16_t)( y0 - glyphInfo.height );
		
	/*
	q->x0 = rx;
	q->y0 = ry;
	q->x1 = rx + scale * (glyph->x1 - glyph->x0);
	q->y1 = ry - scale * (glyph->y1 - glyph->y0);
	*/


	float shift = x0_precise - x0;
	float xMult = 32767.0f / (float) (m_textureWidth);
	float yMult = 32767.0f / (float) (m_textureHeight);

	//TODO: Understand why I have to add the +1 there

	uint16_t s0 = (float) (glyphInfo.texture_x+1) * xMult;
	uint16_t t0 = (float) (glyphInfo.texture_y+1) * yMult;
	uint16_t s1 = (float) (glyphInfo.texture_x+1 + glyphInfo.width) * xMult;
	uint16_t t1 = (float) (glyphInfo.texture_y+1 + glyphInfo.height) * yMult;
	/*
	uint16_t s0 = (32767 * glyphInfo.texture_x) / m_textureWidth;
	uint16_t t0 = (32767 * glyphInfo.texture_y) / m_textureHeight;
	uint16_t s1 = (32767 * (glyphInfo.texture_x + glyphInfo.width+1)) / m_textureWidth;
	uint16_t t1 = (32767 * (glyphInfo.texture_y + glyphInfo.height+1)) / m_textureHeight;
	*/
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
