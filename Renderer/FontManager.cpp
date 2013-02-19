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

	delete [] m_buffer;
	m_buffer = NULL;
}

FontHandle FontManager::loadTrueTypeFont(const char* filePath, uint16_t fontFlags, uint16_t pixelSize)
{
	TrueTypeFont* ttf = new TrueTypeFont();	
	bool initialized = ttf->initFromFile(filePath);
	
	if(initialized)
	{
		FontInfo fontInfo;
		if( ttf->getFontInfo(pixelSize, 0, fontInfo) )
		{
			m_cachedFonts.push_back(new CachedFont(fontInfo, ttf));	
			return (int32_t) m_cachedFonts.size()-1;
		}
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
			
			BakedGlyph bakedGlyph;			
			// load glyph info
			bool glyphFound = font.trueTypeFont->getGlyphInfo(font.fontInfo, codePoint, bakedGlyph.glyphInfo);
			assert(glyphFound);
			//assert font is not too big
			assert(bakedGlyph.glyphInfo.width*bakedGlyph.glyphInfo.height*m_depth < MAX_FONT_BUFFER_SIZE);
			//bake glyph to buffer
			font.trueTypeFont->bakeGlyphAlpha(font.fontInfo, bakedGlyph.glyphInfo, m_buffer);
			
			Rect16 rect;
			// We want each glyph to be separated by at least one black pixel
			if(!m_rectanglePacker.addRectangle(bakedGlyph.glyphInfo.width + 1, bakedGlyph.glyphInfo.height + 1, rect))
			{
				return false;
			}

			bakedGlyph.texture_x = rect.x;
			bakedGlyph.texture_y = rect.y;
			//but only update the bitmap region
			--rect.w;
			--rect.h;
			assert(rect.w == bakedGlyph.glyphInfo.width);
			assert(rect.h == bakedGlyph.glyphInfo.height);
			
			// update texture
			m_texture->update(rect, m_buffer);
			
			// store cached glyph
			font.cachedGlyphs[codePoint] = bakedGlyph;			
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