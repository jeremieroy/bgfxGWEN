//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <assert.h>
//#include <limits.h>
#include "GlyphStash.h"

#define BGFX_FONT_ASSERT(cond, message) assert((cond) && (message));

namespace bgfx_font
{
     
GlyphStash::GlyphStash(ITextureProvider* texture)
{
    BGFX_FONT_ASSERT(texture != NULL, "A texture provider cannot be NULL");

    uint32_t depth = texture->getDepth();
    uint32_t width = texture->getWidth();
    uint32_t height = texture->getHeight();
    
    BGFX_FONT_ASSERT( (depth == 1) || (depth == 4) , "Incompatible texture depth, must be 1 or 4");
    
    m_width = width;
    m_height = height;
    m_depth = depth;
    m_texture = texture;
    m_usedSpace = 0;
    // We want a one pixel border around the whole atlas to avoid any artefact when
    // sampling texture
    Node node(1,1, width-2);
    m_skyline.push_back(node);
}

GlyphStash::~GlyphStash()
{
}

void GlyphStash::updateRegion(Rect16 rect, const uint8_t* data)
{   
    assert( rect.x < (m_width-1));
    assert( (uint32_t)(rect.x + rect.w) <= (uint32_t)(m_width-1));
    assert( rect.y < (m_height-1));
    assert( (uint32_t)(rect.y + rect.h) <= (uint32_t)(m_height-1));


    m_texture->update(rect, data);//, stride);
    /*
    size_t i;
    size_t depth = self->depth;
    size_t charsize = sizeof(char);

    size_t height = rect.h;
    for( size_t i = 0; i < height; ++i )
    {
        memcpy( self->data+((y+i)*self->width + x ) * charsize * depth, 
                data + (i*stride) * charsize, width * charsize * depth  );
    }*/
}

float GlyphStash::getUsageRatio()
{
    return 0;
}

void GlyphStash::clear()
{  
    m_skyline.clear();
    m_usedSpace = 0;
    
    // We want a one pixel border around the whole atlas to avoid any artefact when
    // sampling texture
    Node node(1,1, m_width-2);
    m_skyline.push_back(node);
    m_texture->clear();
}

int32_t GlyphStash::fit(uint32_t skylineNodeIndex, uint16_t _width, uint16_t _height)
{
    uint32_t width = _width;
    uint32_t height = _height;
    
    Node& baseNode = m_skyline[skylineNodeIndex];
    
    uint32_t x = baseNode.x, y;
	int32_t width_left = width;
	uint32_t i = skylineNodeIndex;

    if ( (x + width) > (m_width-1) )
    {
		return -1;
    }
    y = baseNode.y;
	while( width_left > 0 )
	{
        Node& node = m_skyline[i];
        if( node.y > y )
        {
            y = node.y;
        }
		if( (y + height) > (m_height-1) )
        {
			return -1;
        }
        width_left -= node.width;
		++i;
	}
	return y;

}

void GlyphStash::merge()
{
    Node* node;
    Node* next;
    uint32_t i;

	for( i=0; i < m_skyline.size()-1; ++i )
    {
        node = (Node *) &m_skyline[i];
        next = (Node *) &m_skyline[i+1];
		if( node->y == next->y )
		{
			node->width += next->width;
            m_skyline.erase(m_skyline.begin() + i + 1);
			--i;
		}
    }
}

Rect16 GlyphStash::allocateRegion(uint16_t width, uint16_t height)
{
	int y, best_height, best_index;
    uint32_t best_width;
    Node* node;
    Node* prev;
    Rect16 region(0, 0, width, height);
    size_t i;

    best_height = INT_MAX;
    best_index  = -1;
    best_width = INT_MAX;
	for( i = 0; i < m_skyline.size(); ++i )
	{
        y = fit( i, width, height );
		if( y >= 0 )
		{
            node = &m_skyline[i];
			if( ( (y + height) < best_height ) ||
                ( ((y + height) == best_height) && (node->width < best_width)) )
			{
				best_height = y + height;
				best_index = i;
				best_width = node->width;
				region.x = node->x;
				region.y = y;
			}
        }
    }

    if( best_index == -1 )
    {
        region.x = -1;
        region.y = -1;
        region.w = 0;
        region.h = 0;
        return region;
    }
    
    Node newNode(region.x,region.y + height, width);
    m_skyline.insert(m_skyline.begin() + best_index, newNode);

    for(i = best_index+1; i < m_skyline.size(); ++i)
    {
        node = &m_skyline[i];
        prev = &m_skyline[i-1];
        if (node->x < (prev->x + prev->width) )
        {
            int shrink = prev->x + prev->width - node->x;
            node->x += shrink;
            node->width -= shrink;
            if (node->width <= 0)
            {
                 m_skyline.erase(m_skyline.begin() + i);
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

    merge();
    m_usedSpace += width * height;
    return region;
}


}