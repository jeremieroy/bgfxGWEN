#include "TrueTypeFont.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <stdio.h>
#include <assert.h>
//#include <stdlib.h>

namespace bgfx_font
{
TrueTypeFont::TrueTypeFont(): m_fileBuffer(NULL), m_ownBuffer(false)
{
	m_stbFont = new stbtt_fontinfo;
}

TrueTypeFont::~TrueTypeFont()
{
	if(m_ownBuffer) delete [] m_fileBuffer;
	m_fileBuffer = NULL;
	delete m_stbFont;
}

bool TrueTypeFont::initFromBuffer(const char* extBuffer)
{
	if(m_ownBuffer) delete [] m_fileBuffer;
	m_fileBuffer = extBuffer;
	m_ownBuffer = false;

	if( 0 != stbtt_InitFont(m_stbFont, (const unsigned char*) m_fileBuffer, stbtt_GetFontOffsetForIndex((const unsigned char*)m_fileBuffer,0)))
	{		
		m_fileBuffer = NULL;
		return false;
	}
	return true;
}

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

	if( 0 != stbtt_InitFont(m_stbFont, (const unsigned char*) m_fileBuffer, stbtt_GetFontOffsetForIndex((const unsigned char*)m_fileBuffer,0)))
	{
		delete [] m_fileBuffer;
		m_fileBuffer = NULL;
		m_ownBuffer = false;
		return false;
	}

	return true;
}

GlyphInfo TrueTypeFont::getGlyphInfo(CodePoint_t codePoint, uint16_t pixelSize)
{
	assert(m_fileBuffer != NULL && "TrueTypeFont not initialized" );
	int glyphIndex = stbtt_FindGlyphIndex(m_stbFont,codePoint);

	float scale = stbtt_ScaleForPixelHeight(m_stbFont, (float) pixelSize);

	int x0, y0, x1, y1;
	const float shift_x = 0;
	const float shift_y = 0;	
	stbtt_GetGlyphBitmapBoxSubpixel(m_stbFont, glyphIndex, scale, scale, shift_x, shift_y, &x0,&y0,&x1,&y1);
	// get the bbox of the bitmap centered around the glyph origin; so the
	// bitmap width is x1-x0, height is y1-y0, and location to place
	// the bitmap top left is (leftSideBearing*scale, y0).
	// (Note that the bitmap uses y-increases-down, but the shape uses
	// y-increases-up, so CodepointBitmapBox and CodepointBox are inverted.)

	//I'm assuming this is true:
	assert(x0 == 0);

	int ascent, descent, lineGap;
	// ascent is the coordinate above the baseline the font extends; descent
	// is the coordinate below the baseline the font extends (i.e. it is typically negative)
	// lineGap is the spacing between one row's descent and the next row's ascent...
	// so you should advance the vertical position by "*ascent - *descent + *lineGap"
	//   these are expressed in unscaled coordinates, so you must multiply by
	//   the scale factor for a given size	
	stbtt_GetFontVMetrics(m_stbFont, &ascent, &descent, &lineGap);
	
	int advanceWidth, leftSideBearing;
	// leftSideBearing is the offset from the current horizontal position to the left edge of the character
	// advanceWidth is the offset from the current horizontal position to the next horizontal position
	// these are expressed in unscaled coordinates
	stbtt_GetGlyphHMetrics(m_stbFont, glyphIndex, &advanceWidth, &leftSideBearing);
	return GlyphInfo(x1-x0, y1-y0, leftSideBearing*scale, (float)y0, advanceWidth*scale, (ascent - descent + lineGap) *scale );
}

void TrueTypeFont::bakeGlyphAlpha(const GlyphInfo& glyphInfo, uint16_t pixelSize, uint8_t* outBuffer)
{
	assert(m_fileBuffer != NULL && "TrueTypeFont not initialized" );
    float scale = stbtt_ScaleForPixelHeight(m_stbFont, (float) pixelSize);	
    const float shift_x = 0;
	const float shift_y = 0;	
	stbtt_MakeGlyphBitmapSubpixel(m_stbFont, outBuffer, glyphInfo.width, glyphInfo.height, glyphInfo.width, scale, scale, shift_x, shift_y, glyphInfo.glyphIndex);      
}

void TrueTypeFont::bakeGlyphHinted(const GlyphInfo& glyphInfo, uint16_t pixelSize, uint32_t* outBuffer)
{
	assert(m_fileBuffer != NULL && "TrueTypeFont not initialized" );
}

}
