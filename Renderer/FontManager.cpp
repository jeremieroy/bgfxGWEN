#include <assert.h>
#include "FontManager.h"
#include <math.h>
#define BGFX_FONT_ASSERT(cond, message) assert((cond) && (message));

namespace bgfx_font
{
const uint16_t MAX_OPENED_FILES = 64;
const uint16_t MAX_OPENED_FONT = 64;
const uint16_t MAX_TEXTURE_ATLAS = 4;
const uint32_t MAX_FONT_BUFFER_SIZE = 128*128*4;

FontManager::FontManager():m_filesHandles(MAX_OPENED_FILES), m_fontHandles(MAX_OPENED_FONT), m_atlasHandles(MAX_TEXTURE_ATLAS)
{
	m_cachedFiles = new CachedFile[MAX_OPENED_FILES];
	m_cachedFonts = new CachedFont[MAX_OPENED_FONT];
	m_atlas = new TextureAtlas[MAX_TEXTURE_ATLAS];
	m_buffer = new uint8_t[MAX_FONT_BUFFER_SIZE];
}

FontManager::~FontManager()
{
	assert(m_fontHandles.getNumHandles() == 0 && "All the fonts must be destroyed before destroying the manager");
	delete [] m_cachedFonts;

	assert(m_filesHandles.getNumHandles() == 0 && "All the files must be destroyed before destroying the manager");
	delete [] m_cachedFiles;

	assert(m_atlasHandles.getNumHandles() == 0 && "All the texture atlas must be destroyed before destroying the manager");
	delete [] m_atlas;

	delete [] m_buffer;
}

TextureAtlasHandle FontManager::createTextureAtlas(TextureType type, uint16_t width, uint16_t height)
{
	bgfx::TextureHandle textureHandle = createTexture(type, width, height);
	assert(textureHandle.idx != bgfx::invalidHandle);

	assert(width >= 16 );
	assert(height >= 4 );

	uint16_t atlasIdx = m_atlasHandles.alloc();
	assert(atlasIdx != bx::HandleAlloc::invalid);
	m_atlas[atlasIdx].type = type;
	m_atlas[atlasIdx].rectanglePacker.init(width, height);
	m_atlas[atlasIdx].textureHandle = textureHandle;
	m_atlas[atlasIdx].width = width;
	m_atlas[atlasIdx].height = height;
	m_atlas[atlasIdx].depth = (type == TEXTURE_TYPE_ALPHA)?1:4;
	
	// Create filler rectangle
	uint8_t buffer[4*4*4];
	memset( buffer, 255, 4 * 4 * 4);
	
	//TODO use a glyph -_-
	GlyphInfo glyph;
	glyph.width=1;
	glyph.height=1;
	assert( addBitmap(m_atlas[atlasIdx], glyph, buffer) );
		
	m_atlas[atlasIdx].m_black_x0 = glyph.texture_x0;
	m_atlas[atlasIdx].m_black_y0 = glyph.texture_y0;
	m_atlas[atlasIdx].m_black_x1 = glyph.texture_x1;
	m_atlas[atlasIdx].m_black_y1 = glyph.texture_y1;

	return TextureAtlasHandle(atlasIdx);
}

bgfx::TextureHandle FontManager::getTextureHandle(TextureAtlasHandle handle)
{
	assert(handle.isValid());
	uint16_t atlasIdx = m_atlasHandles.getHandleAt(handle.idx);
	assert(atlasIdx != bx::HandleAlloc::invalid);	
	return m_atlas[atlasIdx].textureHandle;
}

void FontManager::getTextureSize(TextureAtlasHandle handle, uint16_t& width, uint16_t& height)
{
	uint16_t atlasIdx = m_atlasHandles.getHandleAt(handle.idx);
	assert(atlasIdx != bx::HandleAlloc::invalid);
	width = m_atlas[atlasIdx].width;
	height = m_atlas[atlasIdx].height;
}

void FontManager::getBlackGlyphUV(TextureAtlasHandle handle, int16_t& x0, int16_t& y0, int16_t& x1, int16_t& y1)
{
	uint16_t atlasIdx = m_atlasHandles.getHandleAt(handle.idx);
	x0 = m_atlas[atlasIdx].m_black_x0;
	y0 = m_atlas[atlasIdx].m_black_y0;
	x1 = m_atlas[atlasIdx].m_black_x1;
	y1 = m_atlas[atlasIdx].m_black_y1;
}

void FontManager::destroyTextureAtlas(TextureAtlasHandle handle)
{
	assert(handle.isValid());
	uint16_t idx = m_atlasHandles.getHandleAt(handle.idx);
	assert(idx != bx::HandleAlloc::invalid);
	bgfx::TextureHandle hd;
	hd.idx = idx;
	destroyTexture(hd);	
	m_atlasHandles.free(handle.idx);
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
	uint16_t hdIdx = 0;
	for(; hdIdx < texCount; ++hdIdx)
	{	
		uint16_t texIDX = texHandles[hdIdx];
		TextureType texType = m_atlas[ texIDX ].type;
		if(texType == TEXTURE_TYPE_ALPHA && (fontType == FONT_TYPE_ALPHA || fontType == FONT_TYPE_DISTANCE))
		{
			break;
		}
		if(texType == TEXTURE_TYPE_RGBA && (fontType == FONT_TYPE_RGBA || fontType == FONT_TYPE_HINTED))
		{
			break;
		}
	}

	if(hdIdx == texCount)
	{ 
		return FontHandle(INVALID_HANDLE_ID);
	}
	uint16_t texIDX = texHandles[hdIdx];

	uint16_t fontIdx = m_fontHandles.alloc();
	assert(fontIdx != bx::HandleAlloc::invalid);
	
	m_cachedFonts[fontIdx].trueTypeFont = m_cachedFiles[fileIdx].trueType;
	m_cachedFonts[fontIdx].fontInfo = m_cachedFonts[fontIdx].trueTypeFont->getFontInfoByPixelSize((float) pixelSize);
	m_cachedFonts[fontIdx].fontInfo.fontType = fontType;
	m_cachedFonts[fontIdx].fontInfo.textureAtlas = TextureAtlasHandle(texIDX);
	m_cachedFonts[fontIdx].cachedGlyphs.clear();
		
	return FontHandle(fontIdx);
}
	
FontHandle FontManager::createFontByEmSize(TrueTypeHandle handle, uint32_t pixelSize, FontType fontType)
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

	uint16_t fontIdx = m_fontHandles.alloc();
	assert(fontIdx != bx::HandleAlloc::invalid);

	m_cachedFonts[fontIdx].trueTypeFont = m_cachedFiles[fileIdx].trueType;
	m_cachedFonts[fontIdx].fontInfo = m_cachedFonts[fontIdx].trueTypeFont->getFontInfoByEmSize((float) pixelSize);
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
			case FONT_TYPE_DISTANCE:
				font.trueTypeFont->bakeGlyphDistance(fontInfo, glyphInfo, m_buffer);
				break;
			default:
				assert(false && "TextureType not supported yet");
			};

			//copy bitmap to texture
			if(!addBitmap(textureAtlas, glyphInfo, m_buffer) )
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

bool FontManager::preloadGlyph(FontHandle handle, CodePoint_t codePoint)
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
		
		GlyphHash_t::iterator iter = font.cachedGlyphs.find(codePoint);
		if(iter != font.cachedGlyphs.end())
		{
			return true;
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
		case FONT_TYPE_DISTANCE:
			font.trueTypeFont->bakeGlyphDistance(fontInfo, glyphInfo, m_buffer);
			break;
		default:
			assert(false && "TextureType not supported yet");
		};

		//copy bitmap to texture
		if(!addBitmap(textureAtlas, glyphInfo, m_buffer) )
		{
			return false;
		}

		// store cached glyph
		font.cachedGlyphs[codePoint] = glyphInfo;
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
		if(preloadGlyph(fontHandle, codePoint))
		{
			iter = m_cachedFonts[fontIdx].cachedGlyphs.find(codePoint);
		}else
		{
			return false;
		}
	}
	outInfo = iter->second;
	return true;
}

// ****************************************************************************

bgfx::TextureHandle FontManager::createTexture(TextureType textureType, uint16_t width, uint16_t height)
{
	//Uncomment this to debug atlas
	//const bgfx::Memory* mem = bgfx::alloc(width*height);
	//memset(mem->data, 255, mem->size);
	const bgfx::Memory* mem = NULL;
	uint32_t flags = BGFX_TEXTURE_MIN_POINT|BGFX_TEXTURE_MAG_POINT|BGFX_TEXTURE_MIP_POINT|BGFX_TEXTURE_U_CLAMP|BGFX_TEXTURE_V_CLAMP;
	bgfx::TextureHandle handle;
	switch(textureType)
	{
		case TEXTURE_TYPE_ALPHA:
			handle = bgfx::createTexture2D(width, height, 1, bgfx::TextureFormat::L8, flags, mem);
		break;
		case TEXTURE_TYPE_RGBA:
			handle = bgfx::createTexture2D(width, height, 1, bgfx::TextureFormat::BGRA8, flags, mem);
		break;
	};
	return handle;
}

void FontManager::destroyTexture(bgfx::TextureHandle handle)
{
	bgfx::destroyTexture(handle);
}

bool FontManager::addBitmap(TextureAtlas& atlas, GlyphInfo& glyphInfo, const uint8_t* data)
{	
	uint16_t x,y;
	// We want each bitmap to be separated by at least one black pixel
	if(!atlas.rectanglePacker.addRectangle(glyphInfo.width + 1, glyphInfo.height + 1,  x, y))
	{
		return false;
	}

	//this allocation could maybe be avoided, will see later
	const bgfx::Memory* mem = bgfx::alloc(glyphInfo.width*glyphInfo.height*atlas.depth);
	memcpy(mem->data, data, glyphInfo.width*glyphInfo.height*atlas.depth);	
	bgfx::updateTexture2D(atlas.textureHandle, 0, x, y, glyphInfo.width, glyphInfo.height, mem);

	glyphInfo.texture_x0 = x;
	glyphInfo.texture_y0 = y;
	glyphInfo.texture_x1 = x+glyphInfo.width;
	glyphInfo.texture_y1 = y+glyphInfo.height;
	
	float texMultX = 32767.0f / (float) (atlas.width);
	float texMultY = 32767.0f / (float) (atlas.height);
	glyphInfo.texture_x0 = (int16_t)ceil((glyphInfo.texture_x0) * texMultX);
	glyphInfo.texture_y0 = (int16_t)ceil((glyphInfo.texture_y0) * texMultY);
	glyphInfo.texture_x1 = (int16_t)ceil((glyphInfo.texture_x1) * texMultX);
	glyphInfo.texture_y1 = (int16_t)ceil((glyphInfo.texture_y1) * texMultY);

	assert(((glyphInfo.texture_x0 * atlas.width)/32767) == x);
	assert(((glyphInfo.texture_y0 * atlas.height)/32767) == y);
	assert(((glyphInfo.texture_x1 * atlas.width)/32767) == x+glyphInfo.width);
	assert(((glyphInfo.texture_y1 * atlas.height)/32767) == y+glyphInfo.height);

	return true;
}

}
