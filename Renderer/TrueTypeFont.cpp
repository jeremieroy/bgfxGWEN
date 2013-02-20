#include "TrueTypeFont.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <assert.h>

namespace bgfx_font
{

TrueTypeFont::TrueTypeFont(): m_font(NULL)
{	
}

TrueTypeFont::~TrueTypeFont()
{
	delete m_font;
	m_font = NULL;
}

bool TrueTypeFont::init(const uint8_t* buffer, uint32_t size, int32_t fontIndex)
{
	assert((size > 256 && size < 100000000) && "TrueType buffer size is suspicious");
	assert(m_font == NULL && "TrueTypeFont already initialized" );
	
	stbtt_fontinfo fnt;
	//check if valid
	if( 0 == stbtt_InitFont( &fnt, (const unsigned char*) buffer, stbtt_GetFontOffsetForIndex((const unsigned char*)buffer, fontIndex)))
	{
		return false;
	}

	m_font = new stbtt_fontinfo(fnt);
	return true;
}

/*
bool TrueTypeFont::initFromFile(const char * _fontPath)
{  
	FILE * pFile;
	pFile = fopen (_fontPath, "rb");
	if (pFile!=NULL)
	{
		// Go to the end of the file.
		if (fseek(pFile, 0L, SEEK_END) == 0)
		{
			// Get the size of the file.
			long bufsize = ftell(pFile);
			if (bufsize == -1) 
			{
				fclose(pFile);
				return false;
			}
			
			if(m_ownBuffer) delete [] m_fileBuffer;
			// Allocate buffer
			m_fileBuffer = new char[bufsize];
			m_ownBuffer = true;

			// Go back to the start of the file.
			fseek(pFile, 0L, SEEK_SET);

			// Read the entire file into memory.
			size_t newLen = fread((void*)m_fileBuffer, sizeof(char), bufsize, pFile);						
			if (newLen == 0) 
			{
				fclose(pFile);
				delete [] m_fileBuffer;
				m_fileBuffer = NULL;
				m_ownBuffer = false;
				return false;
			}
			fclose(pFile);
		}	
	}
	else
	{
		return false;
	}

	//TODO check if file is valid
	//if( 0 == stbtt_InitFont(m_stbFont, (const unsigned char*) m_fileBuffer, stbtt_GetFontOffsetForIndex((const unsigned char*)m_fileBuffer,0)))
	//{
	//	delete [] m_fileBuffer;
	//	m_fileBuffer = NULL;
	//	m_ownBuffer = false;
	//	return false;
	//}

	return true;
}
*/

FontInfo TrueTypeFont::getFontInfoByEmSize(float emSize)
{
	assert(m_font != NULL && "TrueTypeFont not initialized" );
	stbtt_fontinfo* fnt = (stbtt_fontinfo*)m_font;
	
	int ascent, descent, lineGap;	
	stbtt_GetFontVMetrics(fnt, &ascent, &descent, &lineGap);

	float scale = stbtt_ScaleForMappingEmToPixels(fnt, emSize);	
	
	FontInfo outFontInfo;
	outFontInfo.scale = scale;
	outFontInfo.ascender = ascent;
	outFontInfo.descender = descent;
	outFontInfo.lineGap = lineGap;
	return outFontInfo;
}

FontInfo TrueTypeFont::getFontInfoByPixelSize(float pixelSize )
{
	assert(m_font != NULL && "TrueTypeFont not initialized" );
	stbtt_fontinfo* fnt = (stbtt_fontinfo*)m_font;
	
	int ascent, descent, lineGap;	
	stbtt_GetFontVMetrics(fnt, &ascent, &descent, &lineGap);

	float scale = stbtt_ScaleForPixelHeight(fnt, pixelSize);	
	
	FontInfo outFontInfo;
	outFontInfo.scale = scale;
	outFontInfo.ascender = ascent;
	outFontInfo.descender = descent;
	outFontInfo.lineGap = lineGap;
	return outFontInfo;
}

bool TrueTypeFont::getGlyphInfo(const FontInfo& fontInfo, CodePoint_t codePoint, GlyphInfo& outGlyphInfo)
{
	assert(m_font != NULL && "TrueTypeFont not initialized" );
	stbtt_fontinfo* fnt = (stbtt_fontinfo*)m_font;

	int glyphIndex = stbtt_FindGlyphIndex(fnt, codePoint);
	//TODO check glyph validity ?
	
	int x0, y0, x1, y1;
	const float shift_x = 0;
	const float shift_y = 0;
	stbtt_GetGlyphBitmapBoxSubpixel(fnt, glyphIndex, fontInfo.scale, fontInfo.scale, shift_x, shift_y, &x0,&y0,&x1,&y1);
	// get the bbox of the bitmap centered around the glyph origin; so the
	// bitmap width is x1-x0, height is y1-y0, and location to place
	// the bitmap top left is (leftSideBearing*scale, y0).
	// (Note that the bitmap uses y-increases-down, but the shape uses
	// y-increases-up, so CodepointBitmapBox and CodepointBox are inverted.)	
		
	int advanceWidth, leftSideBearing;
	stbtt_GetGlyphHMetrics(fnt, glyphIndex, &advanceWidth, &leftSideBearing);

	int16_t offset_x = leftSideBearing;
	int16_t offset_y = y0; // ????

	outGlyphInfo.glyphIndex = glyphIndex;
	outGlyphInfo.width = x1-x0;
	outGlyphInfo.height = y1-y0;
	outGlyphInfo.offset_x = offset_x;
	outGlyphInfo.offset_y = offset_y;
	outGlyphInfo.advance_x = advanceWidth;
	outGlyphInfo.advance_y = (fontInfo.ascender - fontInfo.descender + fontInfo.lineGap);

	return true;
}

void TrueTypeFont::bakeGlyphAlpha(const FontInfo& fontInfo, const GlyphInfo& glyphInfo, uint8_t* outBuffer)
{
	assert(m_font != NULL && "TrueTypeFont not initialized" );
	stbtt_fontinfo* fnt = (stbtt_fontinfo*)m_font;
    
    const float shift_x = 0;
	const float shift_y = 0;	
	stbtt_MakeGlyphBitmapSubpixel(fnt, outBuffer, glyphInfo.width, glyphInfo.height, glyphInfo.width, fontInfo.scale, fontInfo.scale, shift_x, shift_y, glyphInfo.glyphIndex);
}

void TrueTypeFont::bakeGlyphHinted(const FontInfo& fontInfo, const GlyphInfo& glyphInfo, uint32_t* outBuffer)
{
	assert(m_font != NULL && "TrueTypeFont not initialized" );
	stbtt_fontinfo* fnt = (stbtt_fontinfo*)m_font;
}

}
