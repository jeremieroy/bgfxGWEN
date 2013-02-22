#include <assert.h>
#include "FontManager.h"

#define BGFX_FONT_ASSERT(cond, message) assert((cond) && (message));

namespace bgfx_font
{
const uint16_t MAX_OPENED_FILES = 64;
const uint16_t MAX_OPENED_FONT = 64;
const uint16_t MAX_TEXTURE_ATLAS = 4;

const uint32_t MAX_FONT_BUFFER_SIZE = 128*128*4;

FontManager::FontManager(ITextureFactory* textureFactory, uint32_t maxGlyphBitmapSize):
m_textureFactory(textureFactory),m_filesHandles(MAX_OPENED_FILES), m_fontHandles(MAX_OPENED_FONT), m_atlasHandles(MAX_TEXTURE_ATLAS)
{
	m_cachedFiles = new CachedFile[MAX_OPENED_FILES];
	m_cachedFonts = new CachedFont[MAX_OPENED_FONT];
	m_atlas = new TextureAtlas[MAX_TEXTURE_ATLAS];
	m_buffer = new uint8_t[maxGlyphBitmapSize*maxGlyphBitmapSize];
}

FontManager::~FontManager()
{	
	assert(m_fontHandles.getNumHandles() == 0 && "All the fonts must be destroyed before destroying the manager");
	delete [] m_cachedFonts;

	assert(m_filesHandles.getNumHandles() == 0 && "All the files must be destroyed before destroying the manager");
	delete [] m_cachedFiles;

	assert(m_atlasHandles.getNumHandles() == 0 && "All the texture must be destroyed before destroying the manager");
	delete [] m_cachedFiles;

	delete [] m_buffer;
	m_buffer = NULL;
}

TextureAtlasHandle FontManager::createTextureAtlas(TextureType type, uint16_t width, uint16_t height)
{
	uint16_t textureHandle = m_textureFactory->createTexture(type, width, height);
	assert(textureHandle != INT16_MAX);

	assert(width >= 16 );
	assert(height >= 4 );

	uint16_t atlasIdx = m_filesHandles.alloc();
	assert(atlasIdx != bx::HandleAlloc::invalid);
	m_atlas[atlasIdx].textureHandle = textureHandle;
	m_atlas[atlasIdx].rectanglePacker.init(width, height);
	m_atlas[atlasIdx].depth = (type == TEXTURE_TYPE_ALPHA)?1:4;

	// Create filler rectangle
	uint8_t buffer[4*4*4];
	memset( buffer, 255, 4 * 4 * 4);
	
	uint16_t x,y;
	assert( addBitmap(m_atlas[atlasIdx], 4, 4, buffer, x, y ) );
	return TextureAtlasHandle(atlasIdx);
}

uint16_t FontManager::getTextureHandle(TextureAtlasHandle handle)
{
	assert(handle.isValid());
	uint16_t textureIdx = m_fontHandles.getHandleAt(handle.idx);
	assert(textureIdx != bx::HandleAlloc::invalid);
	
	return m_atlas[textureIdx].textureHandle;
}

void FontManager::destroyTextureAtlas(TextureAtlasHandle handle)
{
	assert(handle.isValid());
	uint16_t idx = m_filesHandles.getHandleAt(handle.idx);
	assert(idx != bx::HandleAlloc::invalid);
	
	m_textureFactory->destroyTexture(idx);	
	m_filesHandles.free(handle.idx);
}

bool FontManager::addBitmap(TextureAtlas& atlas, uint16_t width, uint16_t height, const uint8_t* data, uint16_t& x, uint16_t& y)
{	
	// We want each bitmap to be separated by at least one black pixel
	if(!atlas.rectanglePacker.addRectangle(width + 1, height + 1,  x, y))
	{
		return false;
	}

	//but only update the bitmap region	(not the 1 pixel separator)
	m_textureFactory->update(atlas.textureHandle, x, y, width, height,atlas.depth, data);
	return true;
}


TrueTypeHandle FontManager::loadTrueTypeFromFile(const char* fontPath, int32_t fontIndex)
{
	FILE * pFile;
	pFile = fopen (fontPath, "rb");
	if (pFile==NULL)
	{
		return TrueTypeHandle(INVALID_HANDLE_ID);
	}
	
	// Go to the end of the file.
	if (fseek(pFile, 0L, SEEK_END) == 0)
	{
		// Get the size of the file.
		long bufsize = ftell(pFile);
		if (bufsize == -1) 
		{
			fclose(pFile);
			return TrueTypeHandle(INVALID_HANDLE_ID);
		}
		
		uint8_t* buffer = new uint8_t[bufsize];

		// Go back to the start of the file.
		fseek(pFile, 0L, SEEK_SET);

		// Read the entire file into memory.
		size_t newLen = fread((void*)buffer, sizeof(char), bufsize, pFile);						
		if (newLen == 0) 
		{
			fclose(pFile);
			delete [] buffer;
			return TrueTypeHandle(INVALID_HANDLE_ID);
		}
		fclose(pFile);

		TrueTypeFont* trueType = new TrueTypeFont();	
		if(trueType->init(buffer, bufsize, fontIndex))
		{
			uint16_t id = m_filesHandles.alloc();
			assert(id != bx::HandleAlloc::invalid);
			m_cachedFiles[id].buffer = buffer;
			m_cachedFiles[id].trueType = trueType;
			return TrueTypeHandle(id);
		}
		delete [] buffer;
		delete trueType;	
		return TrueTypeHandle(INVALID_HANDLE_ID);
	}	

	return TrueTypeHandle(INVALID_HANDLE_ID);
}

TrueTypeHandle FontManager::loadTrueTypeFromMemory(const uint8_t* buffer, uint32_t size, int32_t fontIndex)
{	
	TrueTypeFont* ttf = new TrueTypeFont();	
	if(ttf->init(buffer, size,fontIndex))
	{
		uint16_t id = m_filesHandles.alloc();
		assert(id != bx::HandleAlloc::invalid);
		m_cachedFiles[id].buffer = NULL;
		m_cachedFiles[id].trueType = ttf;

		return TrueTypeHandle(id);		
	}
	delete ttf;	
	return TrueTypeHandle(INVALID_HANDLE_ID);
}

void FontManager::unLoadTrueType(TrueTypeHandle handle)
{
	assert(handle.isValid());
	uint16_t idx = m_filesHandles.getHandleAt(handle.idx);
	assert(idx != bx::HandleAlloc::invalid);
	
	delete m_cachedFiles[idx].trueType;
	delete m_cachedFiles[idx].buffer;
	m_cachedFiles[idx].trueType = NULL;
	m_cachedFiles[idx].buffer = NULL;
	m_filesHandles.free(handle.idx);
}

FontHandle FontManager::createFontByPixelSize(TrueTypeHandle handle, uint32_t pixelSize, FontType fontType)
{
	assert(handle.isValid());
	uint16_t fileIdx = m_filesHandles.getHandleAt(handle.idx);
	assert(fileIdx != bx::HandleAlloc::invalid);

	//search first compatible texture
	//TODO improve this
	uint16_t texCount = m_atlasHandles.getNumHandles();
	const uint16_t* texHandles = m_atlasHandles.getHandles();
	uint16_t texIdx = 0;
	for(; texIdx < texCount; ++texIdx)
	{		
		TextureType texType = m_atlas[ texHandles[texIdx] ].type;
		if(texType == TEXTURE_TYPE_ALPHA && (fontType == FONT_TYPE_ALPHA || fontType == FONT_TYPE_DISTANCE))
		{
			break;
		}
		if(texType == TEXTURE_TYPE_RGBA && (fontType == FONT_TYPE_RGBA || fontType == FONT_TYPE_HINTED))
		{
			break;
		}
	}

	if(texIdx == texCount)
	{ 
		return FontHandle(INVALID_HANDLE_ID);
	}

	uint16_t fontIdx = m_filesHandles.alloc();
	assert(fontIdx != bx::HandleAlloc::invalid);
	
	m_cachedFonts[fontIdx].trueTypeFont = m_cachedFiles[fileIdx].trueType;
	m_cachedFonts[fontIdx].fontInfo = m_cachedFonts[fontIdx].trueTypeFont->getFontInfoByPixelSize((float) pixelSize);
	m_cachedFonts[fontIdx].fontInfo.fontType = fontType;
	m_cachedFonts[fontIdx].fontInfo.textureAtlas = TextureAtlasHandle(texHandles[texIdx]);
	m_cachedFonts[fontIdx].cachedGlyphs.clear();
		
	return FontHandle(fontIdx);
}
	
FontHandle FontManager::createFontByEmSize(TrueTypeHandle handle, uint32_t emSize, FontType fontType)
{
	assert(handle.isValid());
	uint16_t fileIdx = m_filesHandles.getHandleAt(handle.idx);
	assert(fileIdx != bx::HandleAlloc::invalid);

	//search first compatible texture
	//TODO improve this
	uint16_t texCount = m_atlasHandles.getNumHandles();
	const uint16_t* texHandles = m_atlasHandles.getHandles();
	uint16_t texIdx = 0;
	for(; texIdx < texCount; ++texIdx)
	{		
		TextureType texType = m_atlas[ texHandles[texIdx] ].type;
		if(texType == TEXTURE_TYPE_ALPHA && (fontType == FONT_TYPE_ALPHA || fontType == FONT_TYPE_DISTANCE))
		{
			break;
		}
		if(texType == TEXTURE_TYPE_RGBA && (fontType == FONT_TYPE_RGBA || fontType == FONT_TYPE_HINTED))
		{
			break;
		}
	}

	if(texIdx == texCount)
	{ 
		return FontHandle(INVALID_HANDLE_ID);
	}

	uint16_t fontIdx = m_filesHandles.alloc();
	assert(fontIdx != bx::HandleAlloc::invalid);

	m_cachedFonts[fontIdx].trueTypeFont = m_cachedFiles[fileIdx].trueType;
	m_cachedFonts[fontIdx].fontInfo = m_cachedFonts[fontIdx].trueTypeFont->getFontInfoByEmSize((float) emSize);
	m_cachedFonts[fontIdx].fontInfo.fontType = fontType;
	m_cachedFonts[fontIdx].fontInfo.textureAtlas = TextureAtlasHandle(texHandles[texIdx]);
	m_cachedFonts[fontIdx].cachedGlyphs.clear();
			
	return FontHandle(fontIdx);	
}

FontHandle FontManager::loadBakedFontFromFile(const char* fontPath,  const char* descriptorPath)
{
	assert(false); //TODO implement
	return FontHandle(INVALID_HANDLE_ID);
}

FontHandle FontManager::loadBakedFontFromMemory(const uint8_t* imageBuffer, uint32_t imageSize, const uint8_t* descriptorBuffer, uint32_t descriptorSize)
{
	assert(false); //TODO implement
	return FontHandle(INVALID_HANDLE_ID);
}

void FontManager::destroyFont(FontHandle _handle)
{
	assert(_handle.isValid());
	uint16_t fontIdx = m_fontHandles.getHandleAt(_handle.idx);
	assert(fontIdx != bx::HandleAlloc::invalid);

	m_cachedFonts[fontIdx].cachedGlyphs.clear();	
	m_fontHandles.free(_handle.idx);
}

bool FontManager::preloadGlyph(FontHandle handle, const wchar_t* _string)
{
	assert(handle.isValid());
	uint16_t fontIdx = m_fontHandles.getHandleAt(handle.idx);
	assert(fontIdx != bx::HandleAlloc::invalid);

	CachedFont& font = m_cachedFonts[fontIdx];
	FontInfo& fontInfo = font.fontInfo;
	TextureAtlas& textureAtlas = m_atlas[fontInfo.textureAtlas.idx];	

	//if truetype present
	if(font.trueTypeFont != NULL)
	{
		GlyphInfo glyphInfo;
		//parse string
		for( size_t i=0, end = wcslen(_string) ; i < end; ++i )
		{
			//if glyph cached, continue
			CodePoint_t codePoint = _string[i];
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
			assert(glyphInfo.width * glyphInfo.height * textureAtlas.depth < MAX_FONT_BUFFER_SIZE);

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
			if(!addBitmap(textureAtlas, glyphInfo.width, glyphInfo.height, m_buffer, glyphInfo.texture_x, glyphInfo.texture_y ) )
			{
				return false;
			}

			// store cached glyph
			font.cachedGlyphs[codePoint] = glyphInfo;
		}
		return true;
	}

	return false;
}


const FontInfo& FontManager::getFontInfo(FontHandle handle)
{ 
	assert(handle.isValid());
	uint16_t fontIdx = m_fontHandles.getHandleAt(handle.idx);
	assert(fontIdx != bx::HandleAlloc::invalid);
	
	return m_cachedFonts[fontIdx].fontInfo;
}
bool FontManager::getGlyphInfo(FontHandle fontHandle, CodePoint_t codePoint, GlyphInfo& outInfo)
{
	uint16_t fontIdx = m_fontHandles.getHandleAt(fontHandle.idx);
	assert(fontIdx != bx::HandleAlloc::invalid);

	GlyphHash_t::iterator iter = m_cachedFonts[fontIdx].cachedGlyphs.find(codePoint);
	if(iter == m_cachedFonts[fontIdx].cachedGlyphs.end())
	{
		return false;
	}
	outInfo = iter->second;
	return true;
}

}
