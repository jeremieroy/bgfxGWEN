//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <assert.h>
//#include <limits.h>
#include "FontManager.h"

#define BGFX_FONT_ASSERT(cond, message) assert((cond) && (message));

namespace bgfx_font
{

const uint32_t MAX_CACHED_FONT = 64;
const uint32_t MAX_FONT_BUFFER_SIZE = 64*64*4;

FontManager::FontManager(ITextureProvider* texture)
{
	BGFX_FONT_ASSERT(texture != NULL, "A texture provider cannot be NULL");
	    
    m_width = texture->getWidth();
	m_height = texture->getHeight();
	m_depth = texture->getDepth();

	BGFX_FONT_ASSERT( (m_depth == 1) || (m_depth == 4) , "Incompatible texture depth, must be 1 or 4");
	m_texture = texture;
	m_rectanglePacker.init(m_width, m_height);

	m_buffer = new uint8_t[MAX_FONT_BUFFER_SIZE];
}

FontManager::~FontManager()
{	
	for(size_t i = 0; i<m_cachedFonts.size(); ++i)
	{
		delete m_cachedFonts[i];
	}

	//for(size_t i = 0; i<m_cachedBuffer.size(); ++i)
	//{
	//	delete m_cachedBuffer[i];
	//}

	delete [] m_buffer;
	m_buffer = NULL;
}

FontHandle FontManager::loadTrueTypeFont(const char* filePath, uint16_t fontFlags, uint16_t pixelSize)
{
	//seek if buffer already loaded
	/*
	char* buffer = NULL;
	for(size_t i = 0; i<m_cachedBuffer.size(); ++ i)
	{
		if(0 == strcmp(m_cachedBuffer[i]->filePath, filePath))
		{
			buffer = m_cachedBuffer[i]->buffer;
			break;
		}
	}*/

	TrueTypeFont* ttf = new TrueTypeFont();
	//bool initialized = (buffer != NULL) ? ttf->initFromBuffer(buffer): ttf->initFromFile(filePath);
	bool initialized = ttf->initFromFile(filePath);
	
	if(initialized)
	{
		m_cachedFonts.push_back(new CachedFont(pixelSize, fontFlags, ttf));	
		return (int32_t) m_cachedFonts.size()-1;
	}

	delete ttf;
	return INVALID_FONT_HANDLE;
}

void FontManager::unLoadTrueTypeFont(FontHandle handle)
{
	assert(handle != INVALID_FONT_HANDLE);
	assert(handle < (int32_t)m_cachedFonts.size());
	delete m_cachedFonts[handle]->trueTypeFont;
	m_cachedFonts[handle]->trueTypeFont = NULL;
}

FontHandle FontManager::loadBakedFont(const char* fontPath)
{	
	assert(false); //TODO implement
	return INVALID_FONT_HANDLE;
}

bool FontManager::preloadGlyph(FontHandle handle, const wchar_t* _string)
{
	assert(handle != INVALID_FONT_HANDLE);
	assert(handle < (int32_t)m_cachedFonts.size());
	assert(m_cachedFonts[handle] != NULL);

	CachedFont& font = *m_cachedFonts[handle];
	//if truetype present
	if(font.trueTypeFont != NULL)
	{		 
		//parse string		
		for( size_t i=0, end = wcslen(_string) ; i < end; ++i )
		{
			//if glyph cached, continue
			uint32_t codePoint = _string[i];
			GlyphHash_t::iterator iter = font.cachedGlyphs.find(codePoint);			
			if(iter != font.cachedGlyphs.end())
			{
				continue;
			}
			
			// load glyph info
			GlyphInfo glyphInfo = font.trueTypeFont->getGlyphInfo(codePoint, font.pixelSize);
			//assert font is not too big
			assert(glyphInfo.width*glyphInfo.height*m_depth < MAX_FONT_BUFFER_SIZE);
			//bake glyph to buffer
			font.trueTypeFont->bakeGlyphAlpha(glyphInfo, font.pixelSize, m_buffer);
			
			Rect16 rect;
			// We want each glyph to be separated by at least one black pixel
			if(!m_rectanglePacker.addRectangle(glyphInfo.width + 1, glyphInfo.height + 1, rect))
			{
				return false;
			}

			glyphInfo.texture_x = rect.x;
			glyphInfo.texture_y = rect.y;
			//but only update the bitmap region
			--rect.w;
			--rect.h;
			
			// update texture
			m_texture->update(rect, m_buffer);
			
			font.cachedGlyphs[codePoint] = glyphInfo;
			//compute region
			//GlyphRegion region;
			/* 
			region.advance_x = 
			region.advance_x = 
			region.x0 = 
			region.y0 = 
			region.x1 = 
			region.y1 = 
			
			glyph->offset_x = ft_glyph_left;
			glyph->offset_y = ft_glyph_top;
			glyph->s0       = x/(float)width;
			glyph->t0       = y/(float)height;
			glyph->s1       = (x + glyph->width)/(float)width;
			glyph->t1       = (y + glyph->height)/(float)height;
			*/
			// store cached glyph
			//font.cachedGlyphs.insert( std::pair<uint32_t, GlyphRegion>(codePoint, region) );
		}
		return true;		
	}

	return false;
}
	
bool FontManager::getGlyphInfo(FontHandle fontHandle, uint32_t codePoint, GlyphInfo& outInfo)
{
	return false;
}


}