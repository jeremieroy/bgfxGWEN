#include "TextBuffer.h"
#include "FontManager.h"
#include "utf8_helper.h"
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
	m_lineAscender = 0;
	m_lineDescender = 0;
	m_lineGap = 0;
	m_fontManager = NULL;
	m_textureHandle.idx = bgfx::invalidHandle;

	m_textureWidth = 0;
	m_textureHeight = 0;
	m_black_x0 = 0;
	m_black_y0 = 0;
	m_black_x1 = 0;
	m_black_y1 = 0;
	
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
	m_textureHandle.idx = bgfx::invalidHandle;
}


void TextBuffer::appendText(FontHandle fontHandle, const char * _string)
{	
	GlyphInfo glyph;
	const FontInfo& font = m_fontManager->getFontInfo(fontHandle);

	if( m_textureHandle.idx == bgfx::invalidHandle)
	{
		m_textureHandle = m_fontManager->getTextureHandle(font.textureAtlas);
		m_fontManager->getTextureSize(font.textureAtlas, m_textureWidth, m_textureHeight);
		m_fontManager->getBlackGlyphUV(font.textureAtlas, m_black_x0, m_black_y0, m_black_x1, m_black_y1);	
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


	uint32_t codepoint;
	uint32_t state = 0;

	for (; *_string; ++_string)
		if (!utf8_decode(&state, &codepoint, *_string))
		{
			if(m_fontManager->getGlyphInfo(fontHandle, (CodePoint_t)codepoint, glyph))
			{
				appendGlyph((CodePoint_t)codepoint, font, glyph);
			}else
			{
				assert(false && "Glyph not found");
			}
		}
	  //printf("U+%04X\n", codepoint);

	if (state != UTF8_ACCEPT)
	{
	//	assert(false && "The string is not well-formed");
		return; //"The string is not well-formed\n"
	}
}

void TextBuffer::appendText(FontHandle fontHandle, const wchar_t * _string)
{	
	GlyphInfo glyph;
	const FontInfo& font = m_fontManager->getFontInfo(fontHandle);

	if( m_textureHandle.idx == bgfx::invalidHandle)
	{
		m_textureHandle = m_fontManager->getTextureHandle(font.textureAtlas);
		m_fontManager->getTextureSize(font.textureAtlas, m_textureWidth, m_textureHeight);		
		m_fontManager->getBlackGlyphUV(font.textureAtlas, m_black_x0, m_black_y0, m_black_x1, m_black_y1);	
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
		m_lineGap = 0;
	}

	//parse string
	for( size_t i=0, end = wcslen(_string) ; i < end; ++i )
	{
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
	/*
	static CodePoint_t buffer[MAX_BUFFERED_CHARACTERS+1];
	size_t count;
	utf8_countCodePoints(
	*/
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
        m_penY -= m_lineAscender + m_lineDescender + m_lineGap;
		m_lineDescender = 0;// font.descender * font.scale;
		m_lineAscender = 0;//font.ascender* font.scale;
        m_lineStartIndex = m_vertexCount;
		return;
    }

	if( font.ascender * font.scale > m_lineAscender )
    {
        float y = m_penY;
        m_penY -= ((font.ascender  * font.scale) - m_lineAscender);
		verticalCenterLastLine((int16_t) ceil((y-m_penY)));		
        m_lineAscender = (font.ascender * font.scale);
		m_lineGap = font.lineGap * font.scale;
    }

    if( font.descender * font.scale < m_lineDescender )
    {
        m_lineDescender = font.descender * font.scale;
		m_lineGap = font.lineGap * font.scale;
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
	// if background not invisible
	if(false && m_styleFlags & STYLE_BACKGROUND &&  m_backgroundColor & 0x000000FF)
	{
		int16_t x0 = ceil( m_penX - kerning );
		int16_t y0 = ceil( m_penY  + m_lineAscender);// m_lineDescender);//+ font.descender* font.scale);

		int16_t x1 = (int16_t)ceil( (float)x0 + (glyphInfo.advance_x) * font.scale);
		int16_t y1 = (int16_t)ceil( m_penY +m_lineDescender );// (font.ascender - font.descender  - font.lineGap ) * font.scale );
		
		/*
		float x0_precise = m_penX + (glyphInfo.offset_x);// * font.scale);
		int16_t x0 = (int16_t)( x0_precise);
		int16_t y0 = (int16_t)( m_penY - (glyphInfo.offset_y));// * font.scale ));
		int16_t x1 = (int16_t)( x0 + glyphInfo.width );
		int16_t y1 = (int16_t)( y0 - glyphInfo.height );

		float shift = x0_precise - x0;
		*/
		float xMult = 32767.0f / (float) (m_textureWidth);
		float yMult = 32767.0f / (float) (m_textureHeight);

		//TODO: Understand why I have to add the +1 there
		int16_t s0 = (int16_t)ceil(m_black_x0 * xMult);
		int16_t t0 = (int16_t)ceil(m_black_y0 * yMult);
		int16_t s1 = (int16_t)ceil(m_black_x1 * xMult);
		int16_t t1 = (int16_t)ceil(m_black_y1 * yMult);

		m_vertexBuffer[m_vertexCount+0].set( x0, y0, s0, t0, m_backgroundColor);
		m_vertexBuffer[m_vertexCount+1].set( x0, y1, s0, t1, m_backgroundColor);
		m_vertexBuffer[m_vertexCount+2].set( x1, y1, s1, t1, m_backgroundColor);
		m_vertexBuffer[m_vertexCount+3].set( x1, y0, s1, t0, m_backgroundColor);

		m_indexBuffer[m_indexCount + 0] = m_vertexCount+0;
		m_indexBuffer[m_indexCount + 1] = m_vertexCount+1;
		m_indexBuffer[m_indexCount + 2] = m_vertexCount+2;
		m_indexBuffer[m_indexCount + 3] = m_vertexCount+0;
		m_indexBuffer[m_indexCount + 4] = m_vertexCount+2;
		m_indexBuffer[m_indexCount + 5] = m_vertexCount+3;
		m_vertexCount += 4;
		m_indexCount += 6;

	}
	


	// TODO handle underline

	// TODO handle overline

	// TODO handle strikethrough


	//handle glyph	

	float x0_precise = m_penX + (glyphInfo.offset_x);// * font.scale);
	int16_t x0 = (int16_t)( x0_precise);
	int16_t y0 = (int16_t)( m_penY - (glyphInfo.offset_y));// * font.scale ));
	int16_t x1 = (int16_t)( x0 + glyphInfo.width );
	int16_t y1 = (int16_t)( y0 - glyphInfo.height );

	float shift = x0_precise - x0;
	float xMult = 32767.0f / (float) (m_textureWidth);
	float yMult = 32767.0f / (float) (m_textureHeight);

	//TODO: Understand why I have to add the +1 there
	int16_t s0 = (int16_t)ceil((glyphInfo.texture_x) * xMult);
	int16_t t0 = (int16_t)ceil((glyphInfo.texture_y) * yMult);
	int16_t s1 = (int16_t)ceil((glyphInfo.texture_x + glyphInfo.width) * xMult);
	int16_t t1 = (int16_t)ceil((glyphInfo.texture_y + glyphInfo.height) * yMult);
	
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
		
	m_penX += (glyphInfo.advance_x) * font.scale;	
}

void TextBuffer::verticalCenterLastLine(int16_t dy)
{
	size_t i, j;
	for( i=m_lineStartIndex; i < m_vertexCount; ++i )
    {
		TextVertex& v = m_vertexBuffer[i];
		v.y -= dy; 
    }
}

}
