#pragma once
/// Rectangle bin packer
/// Inspired from texture-atlas from freetype-gl (http://code.google.com/p/freetype-gl/)
/// by Nicolas Rougier (Nicolas.Rougier@inria.fr)
/// The actual implementation is based on the article by Jukka Jylänki : "A
/// Thousand Ways to Pack the Bin - A Practical Approach to Two-Dimensional
/// Rectangle Bin Packing", February 27, 2010.
/// More precisely, this is an implementation of the Skyline Bottom-Left
/// algorithm based on C++ sources provided by Jukka Jylänki at:
/// http://clb.demon.fi/files/RectangleBinPack/

#include <stdint.h> // uint32_t
#include <vector>

namespace bgfx_font
{

struct Rect16
{
	Rect16(){}
    Rect16(uint16_t _x, uint16_t _y, uint16_t _w, uint16_t _h):x(_x), y(_y), w(_w), h(_h) {}

    uint16_t x,y;
    uint16_t w,h;
};

class RectanglePacker
{ 
public:
	RectanglePacker();
	RectanglePacker(uint32_t width, uint32_t height);
	~RectanglePacker();
	
	/// non constructor initialization
	void init(uint32_t width, uint32_t height);
	/// find a suitable position for the given rectangle 
	/// @return true if the rectangle can be added, false otherwise
	bool addRectangle(uint16_t width, uint16_t height, Rect16& outRectangle);
	/// return the used surface in squared unit
	uint32_t getUsedSurface() { return m_usedSpace; }
	/// return the total available surface in squared unit
	uint32_t getTotalSurface() { return m_width*m_height; }
	/// return the usage ratio of the available surface [0:1]
	float getUsageRatio();
	/// reset to initial state
    void clear();
private:

	int32_t fit(uint32_t skylineNodeIndex, uint16_t width, uint16_t height);
	/// Merges all skyline nodes that are at the same level.
	void merge();

	struct Node
	{
		Node(uint16_t _x, uint16_t _y, uint16_t _width):x(_x), y(_y), width(_width) {} 
    
		/// The starting x-coordinate (leftmost).
		uint16_t x;
		/// The y-coordinate of the skyline level line.
		uint16_t y;
		/// The line width. The ending coordinate (inclusive) will be x+width-1.
		uint32_t width; //32bit to avoid padding
	};

	 /// Width (in pixels) of the underlying texture
    uint32_t m_width;
    /// Height (in pixels) of the underlying texture
    uint32_t m_height;
    /// Surface used in squared pixel
    uint32_t m_usedSpace;
	/// node of the skyline algorithm
    std::vector<Node> m_skyline;
};


/*
class TextureAtlas
{
public:
	TextureAtlas(ITextureProvider* texture);
    ~TextureAtlas();
	/// find a suitable position for the given rectangle 
	/// @return true if the rectangle can be added, false otherwise
    bool addRectangle(uint16_t width, uint16_t height, Rect16& outRectangle);
	/// update the underlying texture
    void updateRectangle(Rect16 rect, const uint8_t* data);
	/// return the used surface in squared unit
	uint32_t getUsedSurface(); 
	/// return the total available surface in squared unit
	uint32_t getTotalSurface();
	/// return the usage ratio of the available surface [0:1]
	float getUsageRatio();
	/// reset to initial state
    void clear();
private:
	RectanglePacker m_packer;
};
*/

}

