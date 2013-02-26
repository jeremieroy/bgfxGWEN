#pragma once
#include "bgfx_font_types.h"

namespace bgfx_font
{
class TrueTypeFont
{
public:	
	TrueTypeFont();
	~TrueTypeFont();

	/// Initialize from  an external buffer
	/// @remark The ownership of the buffer is external, and you must ensure it stays valid up to this object lifetime
	/// @return true if the initialization succeed
    bool init(const uint8_t* buffer, uint32_t size, int32_t fontIndex = 0);

	/// seems hacky :s
	bool isValid() { return m_font!=NULL; }

	/// return a font descriptor for a given pixel size
	FontInfo getFontInfoByPixelSize(float pixelSize);

	/// return a font descriptor for a given em pixelSize
	FontInfo getFontInfoByEmSize(float pixelSize);

	/// return the details of a glyph
	/// return false if the glyph couldn't be found
	bool getGlyphInfo(const FontInfo& fontInfo, CodePoint_t codePoint, GlyphInfo& outGlyphInfo);

	/// raster a glyph as 8bit alpha to a memory buffer
	/// @ remark buffer min size: glyphInfo.width * glyphInfo * height * sizeof(char)
    void bakeGlyphAlpha(const FontInfo& fontInfo, const GlyphInfo& glyphInfo, uint8_t* outBuffer);

	/// raster a glyph as 32bit rgba to a memory buffer
	/// @ remark buffer min size: glyphInfo.width * glyphInfo * height * sizeof(uint32_t)
    void bakeGlyphHinted(const FontInfo& fontInfo, const GlyphInfo& glyphInfo, uint32_t* outBuffer);

	void bakeGlyphDistance(const FontInfo& fontInfo, const GlyphInfo& glyphInfo, uint8_t* outBuffer);

private:
	void* m_font;
};

}
