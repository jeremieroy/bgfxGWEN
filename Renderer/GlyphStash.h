#pragma once

/// Glyph stash implementation
/// Inspired from texture-atlas from freetype-gl (http://code.google.com/p/freetype-gl/)
/// by Nicolas Rougier (Nicolas.Rougier@inria.fr)

/// The actual implementation is based on the article by Jukka Jylänki : "A
/// Thousand Ways to Pack the Bin - A Practical Approach to Two-Dimensional
/// Rectangle Bin Packing", February 27, 2010.
/// More precisely, this is an implementation of the Skyline Bottom-Left
/// algorithm based on C++ sources provided by Jukka Jylänki at:
/// http://clb.demon.fi/files/RectangleBinPack/

#include <stdint.h> // uint32_t
#include <stdlib.h> // size_t
#include <vector>

namespace bgfx_font
{

/// Quad packer algorithm
/// Given a rectangular zone, fit smaller rectangle inside as tightly as possible.
// QuadPacker

/// Font Helper
/// Given a font, provide information about glyph and bake glyph to images

/// Font Stash
/// Raster glyph on demand to a stash and give back glyph accordingly
/// if given a font helper, it can bake font when needed


struct Rect16
{
    Rect16(uint16_t _x, uint16_t _y, uint16_t _w, uint16_t _h):x(_x), y(_y), w(_w), h(_h) {}

    uint16_t x,y;
    uint16_t w,h;
};

/// engine abstraction
class ITextureProvider
{
public:
    virtual uint16_t getWidth() = 0;
    virtual uint16_t getHeight() = 0;
    virtual uint32_t getDepth() = 0;
    
    virtual void update(Rect16 rect, const uint8_t* data) = 0;
    virtual void clear() = 0;
};

  

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

class GlyphStash
{    
public:
    GlyphStash(ITextureProvider* texture);
    ~GlyphStash();    
    Rect16 allocateRegion(uint16_t w, uint16_t h);
    void updateRegion(Rect16 rect, const uint8_t* data);
    float getUsageRatio();
    void clear();
private:

    int32_t fit(uint32_t skylineNodeIndex, uint16_t width, uint16_t height);
	/// Merges all skyline nodes that are at the same level.
	void merge();

    /// Width (in pixels) of the underlying texture    
    uint32_t m_width;    
    /// Height (in pixels) of the underlying texture
    uint32_t m_height;
    /// Depth (in bytes) of the underlying texture
    uint32_t m_depth;
    
    /// Surface used in squared pixel
    uint32_t m_usedSpace;

    /// texture provider (abstract engine)
    ITextureProvider* m_texture;        
    
    /// node of the skyline algorithm
    std::vector<Node> m_skyline;
};

}
