#include "RectanglePacker.h"
#include <assert.h>

namespace bgfx_font
{
RectanglePacker::RectanglePacker(): m_width(0), m_height(0), m_usedSpace(0)
{	
}

RectanglePacker::RectanglePacker(uint32_t width, uint32_t height):m_width(width), m_height(height), m_usedSpace(0)
{   
    // We want a one pixel border around the whole atlas to avoid any artefact when
    // sampling texture    
    m_skyline.push_back(Node(1,1, width-2));
}

RectanglePacker::~RectanglePacker()
{
}

void RectanglePacker::init(uint32_t width, uint32_t height)
{
	assert(width > 2);
	assert(height > 2);
	m_width = width;
	m_height = height;
	m_usedSpace = 0;

	m_skyline.clear();
	// We want a one pixel border around the whole atlas to avoid any artifact when
    // sampling texture    
    m_skyline.push_back(Node(1,1, width-2));
}

bool RectanglePacker::addRectangle(uint16_t width, uint16_t height, uint16_t& outX, uint16_t& outY)
{
	int y, best_height, best_index;
    int32_t best_width;
    Node* node;
    Node* prev;
    outX = 0;
	outY = 0;
	
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
				outX = node->x;
				outY = y;
			}
        }
    }

    if( best_index == -1 )
    {
		return false;
    }
    
    Node newNode(outX,outY + height, width);
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
    return true;
}
		
float RectanglePacker::getUsageRatio()
{ 
	uint32_t total = m_width*m_height;
	if(total > 0) 
		return (float) m_usedSpace / (float) total;
	else
		return 0.0f;
}

void RectanglePacker::clear()
{
	m_skyline.clear();
    m_usedSpace = 0;
    
    // We want a one pixel border around the whole atlas to avoid any artefact when
    // sampling texture
    m_skyline.push_back(Node(1,1, m_width-2));
}

int32_t RectanglePacker::fit(uint32_t skylineNodeIndex, uint16_t _width, uint16_t _height)
{
	int32_t width = _width;
    int32_t height = _height;
    
    const Node& baseNode = m_skyline[skylineNodeIndex];
    
    int32_t x = baseNode.x, y;
	int32_t width_left = width;
	int32_t i = skylineNodeIndex;

    if ( (x + width) > (int32_t)(m_width-1) )
    {
		return -1;
    }
    y = baseNode.y;
	while( width_left > 0 )
	{
        const Node& node = m_skyline[i];
        if( node.y > y )
        {
            y = node.y;
        }
		if( (y + height) > (int32_t)(m_height-1) )
        {
			return -1;
        }
        width_left -= node.width;
		++i;
	}
	return y;
}
	
void RectanglePacker::merge()
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

}