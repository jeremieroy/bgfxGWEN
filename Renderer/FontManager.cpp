#include <assert.h>
#include "FontManager.h"

#define BGFX_FONT_ASSERT(cond, message) assert((cond) && (message));

namespace bgfx_font
{

const uint32_t MAX_CACHED_FONT = 64;
const uint32_t MAX_FONT_BUFFER_SIZE = 64*64*4;

FontManager::FontManager(ITextureProvider* texture, uint32_t maxGlyphBitmapSize)
{
	BGFX_FONT_ASSERT(texture != NULL, "A texture provider cannot be NULL");

    m_width = texture->getWidth();
	m_height = texture->getHeight();
	m_depth = texture->getDepth();
	assert(m_width >= 16 );
	assert(m_height >= 4 );
	BGFX_FONT_ASSERT( (m_depth == 1) || (m_depth == 4) , "Incompatible texture depth, must be 1 or 4");
	m_texture = texture;
	m_rectanglePacker.init(m_width, m_height);

	m_buffer = new uint8_t[maxGlyphBitmapSize*maxGlyphBitmapSize];

	// Create filler glyph
	uint16_t texture_x, texture_y;
	// We want each glyph to be separated by at least one black pixel
	assert( m_rectanglePacker.addRectangle(4 + 1, 4 + 1, texture_x, texture_y) );

	memset( m_buffer, 255, m_depth * 4 * 4);

	// update texture
	m_texture->update(texture_x, texture_y, 4, 4, m_buffer);
	m_fillerGlyph.texture_x = texture_x;
	m_fillerGlyph.texture_y = texture_y;
	m_fillerGlyph.width = 4;
	m_fillerGlyph.height = 4;
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

FontHandle FontManager::loadTrueTypeFont(const char* filePath, uint16_t pixelSize)
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

			GlyphInfo bakedGlyph;
			// load glyph info
			bool glyphFound = font.trueTypeFont->getGlyphInfo(font.fontInfo, codePoint, bakedGlyph);
			assert(glyphFound);
			//assert font is not too big
			assert(bakedGlyph.width*bakedGlyph.height*m_depth < MAX_FONT_BUFFER_SIZE);
			//bake glyph to buffer
			font.trueTypeFont->bakeGlyphAlpha(font.fontInfo, bakedGlyph, m_buffer);
			
			uint16_t texture_x, texture_y;
			// We want each glyph to be separated by at least one black pixel
			if(!m_rectanglePacker.addRectangle(bakedGlyph.width + 1, bakedGlyph.height + 1,  texture_x, texture_y))
			{
				return false;
			}

			bakedGlyph.texture_x = texture_x;
			bakedGlyph.texture_y = texture_y;
			//but only update the bitmap region	(not 1 pixel separator)
			m_texture->update(texture_x, texture_y, bakedGlyph.width, bakedGlyph.height, m_buffer);
			
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

FontInfo& getFontInfo(FontHandle handle);

}
