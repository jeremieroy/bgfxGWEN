#pragma once
/// Inspired from texture-atlas from freetype-gl (http://code.google.com/p/freetype-gl/)
/// by Nicolas Rougier (Nicolas.Rougier@inria.fr)

/// The actual implementation is based on the article by Jukka Jylänki : "A
/// Thousand Ways to Pack the Bin - A Practical Approach to Two-Dimensional
/// Rectangle Bin Packing", February 27, 2010.
/// More precisely, this is an implementation of the Skyline Bottom-Left
/// algorithm based on C++ sources provided by Jukka Jylänki at:
/// http://clb.demon.fi/files/RectangleBinPack/

#include "bgfx_font_types.h"
#include "RectanglePacker.h"

namespace bgfx_font
{

/// engine abstraction
class ITextureFactory
{
public:
	virtual uint16_t createTexture(uint16_t width, uint16_t height, TextureType textureType) = 0;
	virtual void destroyTexture(uint16_t textureHandle) = 0;
    virtual void update(uint16_t textureHandle, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t* data) = 0;
};

class TextureAtlas
{
public:
	struct Rectangle
	{
		uint16_t x, y;
		uint16_t width, height;
	};


	TextureAtlas(ITextureFactory* textureFactory);
	~TextureAtlas();
	
	bool addBitmap(uint16_t width, uint16_t height, const uint8_t* data, Rectangle& rectangleOut);	
	const Rectangle& getFillerRectangle(){ return m_fillerRectangle; };	

	uint32_t getWidth() { return m_width; }
    uint32_t getHeight() { return m_height; }
    uint32_t getDepth() { return m_depth; }
	TextureType getTextureType() { return m_type; }

private:

	RectanglePacker m_rectanglePacker;
	ITextureFactory* m_textureFactory;
	Rectangle m_fillerRectangle;
	//cached to avoid unnecessary virtual call
	uint32_t m_width;
    uint32_t m_height;
	uint32_t m_depth;
	TextureType m_type;
};
}
