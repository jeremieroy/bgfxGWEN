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
#include "TrueTypeFont.h"
#include "RectanglePacker.h"

//#define BGFX_CONFIG_USE_TINYSTL 1
#if BGFX_CONFIG_USE_TINYSTL

namespace tinystl
{
	//struct bgfx_allocator
	//{
		//static void* static_allocate(size_t _bytes);
		//static void static_deallocate(void* _ptr, size_t /*_bytes*/);
	//};
} // namespace tinystl
//#	define TINYSTL_ALLOCATOR tinystl::bgfx_allocator

#	include <TINYSTL/string.h>
#	include <TINYSTL/vector.h>
#	include <TINYSTL/unordered_map.h>
//#	include <TINYSTL/unordered_set.h>
namespace stl = tinystl;
#else
#	include <string>
#	include <unordered_map>
#	include <unordered_set>
namespace std { namespace tr1 {} }
namespace stl {
	using namespace std;
	using namespace std::tr1;
}
#endif // BGFX_CONFIG_USE_TINYSTL

#include <unordered_map>

namespace bgfx_font
{
const size_t MAX_FONT_SIZE = 64;
enum FontFlag
{
	FONT_NORMAL = 0,
	FONT_ITALIC = 1,
	FONT_BOLD   = 2
};

enum FontType
{
	ALPHA,
	HINTED,
	DISTANCE_FIELD
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

typedef int32_t FontHandle;
const int32_t INVALID_FONT_HANDLE = -1;

class FontManager
{
public:
	FontManager(ITextureProvider* texture);
	~FontManager();
	/// load a truetype font
	/// @return INVALID_FONT_HANDLE if the loading fail
	FontHandle loadTrueTypeFont(const char* fontPath, uint16_t fontFlags, uint16_t pixelSize);

	/// unload a truetype font (free font memory) but keep loaded glyphs
	void unLoadTrueTypeFont(FontHandle handle);

	//FontHandle searchTrueType(const char* fontPath);
	
	/// load a baked font
	/// @return INVALID_FONT_HANDLE if the loading fail
	FontHandle loadBakedFont(const char* fontPath);

	/// bake a font to disk (the set of preloaded glyph)
	/// @return true if the baking succeed, false oherwise
	bool saveBakedFont(FontHandle handle, const char* fontDirectory, const char* fontName );

	/// Preload a set of glyphs
	/// @return true if every glyph could be preloaded, false otherwise
	/// if the Font is a baked font, this only do validation.
	bool preloadGlyph(FontHandle handle, const wchar_t* _string);
	
	/// Return the rendering informations about the glyph region
	/// Load the glyph from a truetype font if possible
	/// @return true if the Glyph is available
	bool getGlyphInfo(FontHandle fontHandle, uint32_t codePoint, GlyphInfo& outInfo);	

private:

	uint32_t m_width;
    uint32_t m_height;
	uint32_t m_depth;
	RectanglePacker m_rectanglePacker;
	ITextureProvider* m_texture;
		
	typedef stl::unordered_map<uint32_t, GlyphInfo> GlyphHash_t;

	/// font caching
	struct CachedFont
	{
		CachedFont(uint16_t _pixelSize = 0, uint16_t _flags = 0, TrueTypeFont* _trueTypeFont = NULL):pixelSize(_pixelSize), flags(_flags), trueTypeFont(_trueTypeFont){}
		~CachedFont(){ delete trueTypeFont; trueTypeFont = NULL; }
				
		uint16_t pixelSize;
		uint16_t flags;
		TrueTypeFont* trueTypeFont;

		//float emSize;
		//float scale;
		GlyphHash_t cachedGlyphs;
	};
	stl::vector<CachedFont*> m_cachedFonts;
	
	/*
	/// file buffer caching	
	struct FontBuffer
	{		
		FontBuffer(const char* filePath, size_t fileSize);
		~FontBuffer();
		char filePath[512];
		char* buffer;
	};
	std::vector<FontBuffer*> m_cachedBuffer;
	*/
	
	uint8_t* m_buffer;
	
};

}

/*
// ----------------------------------------------- texture_atlas_set_region ---
void
texture_atlas_set_region( texture_atlas_t * self,
						 const size_t x,
						 const size_t y,
						 const size_t width,
						 const size_t height,
						 const unsigned char * data,
						 const size_t stride )
{
	assert( self );
	assert( x > 0);
	assert( y > 0);
	assert( x < (self->width-1));
	assert( (x + width) <= (self->width-1));
	assert( y < (self->height-1));
	assert( (y + height) <= (self->height-1));

	size_t i;
	size_t depth = self->depth;
	size_t charsize = sizeof(char);
	for( i=0; i<height; ++i )
	{
		memcpy( self->data+((y+i)*self->width + x ) * charsize * depth, 
			data + (i*stride) * charsize, width * charsize * depth  );
	}
}


// ------------------------------------------------------ texture_atlas_fit ---
int
texture_atlas_fit( texture_atlas_t * self,
				  const size_t index,
				  const size_t width,
				  const size_t height )
{
	assert( self );

	ivec3 *node = (ivec3 *) (vector_get( self->nodes, index ));
	int x = node->x, y, width_left = width;
	size_t i = index;

	if ( (x + width) > (self->width-1) )
	{
		return -1;
	}
	y = node->y;
	while( width_left > 0 )
	{
		node = (ivec3 *) (vector_get( self->nodes, i ));
		if( node->y > y )
		{
			y = node->y;
		}
		if( (y + height) > (self->height-1) )
		{
			return -1;
		}
		width_left -= node->z;
		++i;
	}
	return y;
}


// ---------------------------------------------------- texture_atlas_merge ---
void
texture_atlas_merge( texture_atlas_t * self )
{
	assert( self );

	ivec3 *node, *next;
	size_t i;

	for( i=0; i< self->nodes->size-1; ++i )
	{
		node = (ivec3 *) (vector_get( self->nodes, i ));
		next = (ivec3 *) (vector_get( self->nodes, i+1 ));
		if( node->y == next->y )
		{
			node->z += next->z;
			vector_erase( self->nodes, i+1 );
			--i;
		}
	}
}


// ----------------------------------------------- texture_atlas_get_region ---
ivec4
texture_atlas_get_region( texture_atlas_t * self,
						 const size_t width,
						 const size_t height )
{
	assert( self );

	int y, best_height, best_width, best_index;
	ivec3 *node, *prev;
	ivec4 region = {{0,0,width,height}};
	size_t i;

	best_height = INT_MAX;
	best_index  = -1;
	best_width = INT_MAX;
	for( i=0; i<self->nodes->size; ++i )
	{
		y = texture_atlas_fit( self, i, width, height );
		if( y >= 0 )
		{
			node = (ivec3 *) vector_get( self->nodes, i );
			if( ( (y + height) < best_height ) ||
				( ((y + height) == best_height) && (node->z < best_width)) )
			{
				best_height = y + height;
				best_index = i;
				best_width = node->z;
				region.x = node->x;
				region.y = y;
			}
		}
	}

	if( best_index == -1 )
	{
		region.x = -1;
		region.y = -1;
		region.width = 0;
		region.height = 0;
		return region;
	}

	node = (ivec3 *) malloc( sizeof(ivec3) );
	if( node == NULL)
	{
		fprintf( stderr,
			"line %d: No more memory for allocating data\n", __LINE__ );
		exit( EXIT_FAILURE );
	}
	node->x = region.x;
	node->y = region.y + height;
	node->z = width;
	vector_insert( self->nodes, best_index, node );
	free( node );

	for(i = best_index+1; i < self->nodes->size; ++i)
	{
		node = (ivec3 *) vector_get( self->nodes, i );
		prev = (ivec3 *) vector_get( self->nodes, i-1 );

		if (node->x < (prev->x + prev->z) )
		{
			int shrink = prev->x + prev->z - node->x;
			node->x += shrink;
			node->z -= shrink;
			if (node->z <= 0)
			{
				vector_erase( self->nodes, i );
				--i;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	texture_atlas_merge( self );
	self->used += width * height;
	return region;
}


// ---------------------------------------------------- texture_atlas_clear ---
void
texture_atlas_clear( texture_atlas_t * self )
{
	assert( self );
	assert( self->data );

	vector_clear( self->nodes );
	self->used = 0;
	// We want a one pixel border around the whole atlas to avoid any artefact when
	// sampling texture
	ivec3 node = {{1,1,self->width-2}};
	vector_push_back( self->nodes, &node );
	memset( self->data, 0, self->width*self->height*self->depth );
}

*/