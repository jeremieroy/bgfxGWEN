#include "RectanglePacker.h"

namespace bgfx_font
{

RectanglePacker::RectanglePacker(uint32_t width, uint32_t height):m_width(width), m_height(height)
{   
    // We want a one pixel border around the whole atlas to avoid any artefact when
    // sampling texture    
    m_skyline.push_back(Node(1,1, width-2));
	m_usedSpace = 0;
}

RectanglePacker::~RectanglePacker()
{
}

bool RectanglePacker::addRectangle(uint16_t width, uint16_t height, Rect16& outRectangle)
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
		return false;
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
	outRectangle = region;
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