#include <assert.h>
#include "FontManager.h"

#define BGFX_FONT_ASSERT(cond, message) assert((cond) && (message));

namespace bgfx_font
{

const uint32_t MAX_CACHED_FONT = 64;
const uint32_t MAX_FONT_BUFFER_SIZE = 64*64*4;

FontManager::FontManager(uint32_t maxGlyphBitmapSize)
{
	m_buffer = new uint8_t[maxGlyphBitmapSize*maxGlyphBitmapSize];
	m_textureCount = 0;
}

FontManager::~FontManager()
{
	for(size_t i = 0; i<m_cachedFonts.size(); ++i)
	{
		delete m_cachedFonts[i];
	}

	for(size_t i = 0; i<m_trueType.size(); ++i)
	{		
		delete m_trueType[i].trueType;
		delete m_trueType[i].buffer;
	}

	delete [] m_buffer;
	m_buffer = NULL;
}

TextureAtlasHandle FontManager::addTextureAtlas(TextureAtlas* atlas)
{
	assert(atlas != NULL && "Texture atlas cannot be NULL");
	assert(m_textureCount < 4 && "You cannot add more than 4 texture atlas");
	m_textures[m_textureCount] = atlas;	
	return m_textureCount++;
}

TextureAtlas* FontManager::getTextureAtlas(TextureAtlasHandle handle)
{
	assert(handle != INVALID_HANDLE && "getTextureAtlas with an invalid TextureAtlasHandle");
	return m_textures[handle];
}

TrueTypeHandle FontManager::loadTrueTypeFromFile(const char* fontPath, int32_t fontIndex)
{
	FILE * pFile;
	pFile = fopen (fontPath, "rb");
	if (pFile==NULL)
	{
		return INVALID_HANDLE;
	}
	
	// Go to the end of the file.
	if (fseek(pFile, 0L, SEEK_END) == 0)
	{
		// Get the size of the file.
		long bufsize = ftell(pFile);
		if (bufsize == -1) 
		{
			fclose(pFile);
			return INVALID_HANDLE;
		}
			
		CachedFile cachedFile;
		cachedFile.buffer = new uint8_t[bufsize];

		// Go back to the start of the file.
		fseek(pFile, 0L, SEEK_SET);

		// Read the entire file into memory.
		size_t newLen = fread((void*)cachedFile.buffer, sizeof(char), bufsize, pFile);						
		if (newLen == 0) 
		{
			fclose(pFile);
			delete [] cachedFile.buffer;
			return INVALID_HANDLE;
		}
		fclose(pFile);


		cachedFile.trueType = new TrueTypeFont();	
		if(cachedFile.trueType->init(cachedFile.buffer, bufsize, fontIndex))
		{		
			m_trueType.push_back(cachedFile);
			return (TrueTypeHandle) (m_trueType.size()-1);
		}
		delete [] cachedFile.buffer;
		delete cachedFile.trueType;	
		return INVALID_HANDLE;
	}	

	return INVALID_HANDLE;
}

TrueTypeHandle FontManager::loadTrueTypeFromMemory(const uint8_t* buffer, uint32_t size, int32_t fontIndex)
{	
	TrueTypeFont* ttf = new TrueTypeFont();	
	if(ttf->init(buffer, size,fontIndex))
	{	

		CachedFile cachedFile;
		cachedFile.buffer = NULL;
		cachedFile.trueType = ttf;
		m_trueType.push_back(cachedFile);
		return (TrueTypeHandle) (m_trueType.size()-1);
	}
	delete ttf;
	
	return INVALID_HANDLE;
}

void FontManager::unLoadTrueType(TrueTypeHandle handle)
{
	assert(handle != INVALID_HANDLE);
	assert(handle < (TrueTypeHandle)m_trueType.size());
	delete m_trueType[handle].trueType;
	delete m_trueType[handle].buffer;
	m_trueType[handle].trueType = NULL;
	m_trueType[handle].buffer = NULL;
}

FontHandle FontManager::getFontByPixelSize(TrueTypeHandle handle, uint32_t pixelSize, FontType fontType)
{
	assert(handle != INVALID_HANDLE);
	assert(handle < (TrueTypeHandle)m_trueType.size());

	//search first compatible texture
	size_t texIdx = 0;
	for(; texIdx < m_textureCount; ++texIdx)
	{
		TextureType texType = m_textures[texIdx]->getTextureType();
		if(texType == TEXTURE_TYPE_ALPHA && (fontType == FONT_TYPE_ALPHA || fontType == FONT_TYPE_DISTANCE))
		{
			break;
		}
		if(texType == TEXTURE_TYPE_RGBA && (fontType == FONT_TYPE_RGBA || fontType == FONT_TYPE_HINTED))
		{
			break;
		}
	}

	if(texIdx == m_textureCount)
	{ 
		return false;
	}

	CachedFont* fnt = new CachedFont;
	fnt->trueTypeFont = m_trueType[handle].trueType;
	fnt->fontInfo = fnt->trueTypeFont->getFontInfoByPixelSize((float) pixelSize);
	fnt->fontInfo.fontType = fontType;

	

	fnt->fontInfo.textureIndex = texIdx;

	m_cachedFonts.push_back(fnt);
	return 	m_cachedFonts.size()-1;
}
	
FontHandle FontManager::getFontByEmSize(TrueTypeHandle handle, uint32_t emSize, FontType fontType)
{
	assert(handle != INVALID_HANDLE);
	assert(handle < (TrueTypeHandle)m_trueType.size());

	CachedFont* fnt = new CachedFont;
	fnt->trueTypeFont = m_trueType[handle].trueType;
	fnt->fontInfo = fnt->trueTypeFont->getFontInfoByPixelSize((float) emSize);
	fnt->fontInfo.fontType = fontType;

	//search first compatible texture
	size_t texIdx = 0;
	for(; texIdx < m_textureCount; ++texIdx)
	{
		
		TextureType texType = m_textures[texIdx]->getTextureType();
		if(texType == TEXTURE_TYPE_ALPHA && (fontType == FONT_TYPE_ALPHA || fontType == FONT_TYPE_DISTANCE))
		{
			break;
		}
		if(texType == TEXTURE_TYPE_RGBA && (fontType == FONT_TYPE_RGBA || fontType == FONT_TYPE_HINTED))
		{
			break;
		}
	}

	if(texIdx == m_textureCount)
	{ 
		return false;
	}

	fnt->fontInfo.textureIndex = texIdx;
	m_cachedFonts.push_back(fnt);
	return 	m_cachedFonts.size()-1;
}

bool FontManager::preloadGlyph(FontHandle handle, const wchar_t* _string)
{
	assert(handle != INVALID_HANDLE);
	assert(handle < (int32_t)m_cachedFonts.size());
	assert(m_cachedFonts[handle] != NULL);	

	CachedFont& font = *m_cachedFonts[handle];
	FontInfo& fontInfo = font.fontInfo;
	TextureAtlas* textureAtlas = m_textures[fontInfo.textureIndex];
	

	//if truetype present
	if(font.trueTypeFont != NULL)
	{
		GlyphInfo glyphInfo;
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
			if( !font.trueTypeFont->getGlyphInfo(fontInfo, codePoint, glyphInfo) )
			{
				return false;
			}

			//assert font is not too big
			assert(glyphInfo.width*glyphInfo.height*textureAtlas->getDepth() < MAX_FONT_BUFFER_SIZE);

			//bake glyph as bitmap to buffer
			switch(font.fontInfo.fontType)
			{
			case FONT_TYPE_ALPHA:
					font.trueTypeFont->bakeGlyphAlpha(fontInfo, glyphInfo, m_buffer);
				break;
				default:
					assert(false && "TextureType not supported yet");
			};		
			
			//copy bitmap to texture
			TextureAtlas::Rectangle rect;
			textureAtlas->addBitmap(glyphInfo.width, glyphInfo.height,m_buffer, rect);
			
			glyphInfo.texture_x = rect.x;
			glyphInfo.texture_x = rect.y;
			
						
			// store cached glyph
			font.cachedGlyphs[codePoint] = glyphInfo;
		}
		return true;
	}

	return false;
}

FontHandle FontManager::loadBakedFontFromFile(const char* fontPath,  const char* descriptorPath)
{
	assert(false); //TODO implement
	return INVALID_HANDLE;
}

FontHandle FontManager::loadBakedFontFromMemory(const uint8_t* imageBuffer, uint32_t imageSize, const uint8_t* descriptorBuffer, uint32_t descriptorSize)
{
	assert(false); //TODO implement
	return INVALID_HANDLE;
}

const FontInfo& FontManager::getFontInfo(FontHandle handle)
{ 
	assert(handle != INVALID_HANDLE);
	return m_cachedFonts[handle]->fontInfo; 
}
bool FontManager::getGlyphInfo(FontHandle fontHandle, uint32_t codePoint, GlyphInfo& outInfo)
{
	return false;
}

}
