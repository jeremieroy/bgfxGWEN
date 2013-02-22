#include "TextureAtlas.h"
#include <assert.h>

namespace bgfx_font
{

TextureAtlas::TextureAtlas(ITextureFactory* texture)
{
	assert(texture != NULL);	
	assert(texture->getWidth() >= 16 );
	assert(texture->getHeight() >= 4 );
	assert((texture->getDepth() == 1 || texture->getDepth() == 4) &&"Incompatible texture depth, must be 1 or 4" );
	m_texture = texture;
	m_width = texture->getWidth();
	m_height = texture->getHeight();
	m_depth = texture->getDepth();
	m_type = texture->getTextureType();
	m_rectanglePacker.init(m_width, m_height);

	// Create filler rectangle
	uint8_t buffer[4*4*4];
	memset( buffer, 255, m_depth * 4 * 4);
	addBitmap(4, 4, buffer, m_fillerRectangle);
}

TextureAtlas::~TextureAtlas()
{
}

bool TextureAtlas::addBitmap(uint16_t width, uint16_t height, const uint8_t* data, Rectangle& rectangleOut)
{	
	// We want each bitmap to be separated by at least one black pixel
	if(!m_rectanglePacker.addRectangle(width + 1, height + 1,  rectangleOut.x, rectangleOut.y))
	{
		return false;
	}

	rectangleOut.width = width;
	rectangleOut.height = height;
	//but only update the bitmap region	(not the 1 pixel separator)
	m_texture->update(rectangleOut.x, rectangleOut.y, width, height, data);
	return true;
}

}
