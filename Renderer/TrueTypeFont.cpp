#include "TrueTypeFont.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <stdio.h>
//#include <stdlib.h>

namespace bgfx_font
{

TrueTypeFont::TrueTypeFont()
{
	m_buffer = NULL;
	m_stbFont = new stbtt_fontinfo;
}

TrueTypeFont::~TrueTypeFont()
{
	if(m_buffer = NULL)
	{
		free(m_buffer);
		m_buffer = NULL;
	}
	delete m_stbFont;
}

bool TrueTypeFont::loadFont(const char * _fontPath)
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
			// Allocate buffer
			m_buffer = malloc(sizeof(char) * (bufsize));

			// Go back to the start of the file.
			fseek(pFile, 0L, SEEK_SET);

			// Read the entire file into memory.
			fread(m_buffer, sizeof(char), bufsize, pFile);
			/*
			size_t newLen = 
			if (newLen == 0) 
			{
				free(m_buffer);
				m_buffer = NULL;
				fclose(pFile);
				return false;
			}*/
			fclose(pFile);	
		}	
	}
	else	 
	{
		return false;
	}

	stbtt_InitFont(m_stbFont, (const unsigned char*) m_buffer, stbtt_GetFontOffsetForIndex((const unsigned char*)m_buffer,0));
	return true;
}

void TrueTypeFont::bakeGlyphAlpha(uint32_t codePoint, float size, uint8_t* outBuffer)
{
    float scale = stbtt_ScaleForMappingEmToPixels(m_stbFont, size);
    int ascent, descent, lineGap = 0;
    stbtt_GetFontVMetrics(m_stbFont, &ascent, &descent, &lineGap);
    int baseline = (int) (ascent*scale);
    
    int advance =0;
    int lsb = 0;
    stbtt_GetCodepointHMetrics(m_stbFont, codePoint, &advance, &lsb);
    
    int x0, y0, x1, y1;
    float shift_x = 0;
    float shift_y = 0;
    stbtt_GetCodepointBitmapBoxSubpixel(m_stbFont, codePoint, scale,scale,shift_x, shift_y, &x0,&y0,&x1,&y1);
	int w = x1-x0;
	int h = y1-y0;
    stbtt_MakeCodepointBitmapSubpixel(m_stbFont, outBuffer, w, h, w, scale, scale, shift_x, shift_y, codePoint);      
}

void TrueTypeFont::bakeGlyphHinted(uint32_t codePoint, float size, uint32_t* outBuffer)
{
}

GlyphSize TrueTypeFont::getGlyphSize(uint32_t codePoint, float size)
{
    int x0, y0, x1, y1;
    float shift_x = 0;
    float shift_y = 0;
    float scale = stbtt_ScaleForMappingEmToPixels(m_stbFont, size);
    stbtt_GetCodepointBitmapBoxSubpixel(m_stbFont, codePoint, scale, scale, shift_x, shift_y, &x0,&y0,&x1,&y1);
    GlyphSize glyphSize;
    glyphSize.width = x1-x0;
    glyphSize.height = y1-y0;
    return glyphSize;    
}

GlyphInfo TrueTypeFont::getGlyphInfo(uint32_t codePoint, float size, float shift_x, float shift_y)
{
	
	int x0, y0, x1, y1;
	float scale = stbtt_ScaleForMappingEmToPixels(m_stbFont, size);
	stbtt_GetCodepointBitmapBoxSubpixel(m_stbFont, codePoint, scale, scale, shift_x, shift_y, &x0,&y0,&x1,&y1);
	GlyphInfo glyphInfo;
	glyphInfo.width = x1-x0;
    glyphInfo.height = y1-y0;

	

	// ascent is the coordinate above the baseline the font extends; descent
	// is the coordinate below the baseline the font extends (i.e. it is typically negative)
	// lineGap is the spacing between one row's descent and the next row's ascent...
	// so you should advance the vertical position by "*ascent - *descent + *lineGap"
	//   these are expressed in unscaled coordinates, so you must multiply by
	//   the scale factor for a given size
	int ascent, descent, lineGap;
	stbtt_GetFontVMetrics(m_stbFont, &ascent, &descent, &lineGap);
	glyphInfo.advance_y = ascent - descent + lineGap;

	
	int advanceWidth, leftSideBearing;
	stbtt_GetCodepointHMetrics(m_stbFont, codePoint, &advanceWidth, &leftSideBearing);
	glyphInfo.offset_x = leftSideBearing;
	// leftSideBearing is the offset from the current horizontal position to the left edge of the character
	// advanceWidth is the offset from the current horizontal position to the next horizontal position
	// these are expressed in unscaled coordinates
	return glyphInfo;
}


}
