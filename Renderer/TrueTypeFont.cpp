#include "TrueTypeFont.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "edtaa3func.h"

#include <assert.h>
#include <math.h>

namespace bgfx_font
{

TrueTypeFont::TrueTypeFont(): m_font(NULL)
{	
}

TrueTypeFont::~TrueTypeFont()
{
	delete m_font;
	m_font = NULL;
}

bool TrueTypeFont::init(const uint8_t* buffer, uint32_t size, int32_t fontIndex)
{
	assert((size > 256 && size < 100000000) && "TrueType buffer size is suspicious");
	assert(m_font == NULL && "TrueTypeFont already initialized" );
	
	stbtt_fontinfo fnt;
	//check if valid
	if( 0 == stbtt_InitFont( &fnt, (const unsigned char*) buffer, stbtt_GetFontOffsetForIndex((const unsigned char*)buffer, fontIndex)))
	{
		return false;
	}

	m_font = new stbtt_fontinfo(fnt);
	return true;
}

FontInfo TrueTypeFont::getFontInfoByEmSize(float pixelSize)
{
	assert(m_font != NULL && "TrueTypeFont not initialized" );
	stbtt_fontinfo* fnt = (stbtt_fontinfo*)m_font;
	
	int ascent, descent, lineGap;	
	stbtt_GetFontVMetrics(fnt, &ascent, &descent, &lineGap);

	float scale = stbtt_ScaleForMappingEmToPixels(fnt, pixelSize);	
	
	//Simply scaling the font ascent or descent might not give correct results. A possible solution is to keep the ceiling of the scaled ascent, and floor of the scaled descent.		
	FontInfo outFontInfo;
	outFontInfo.scale = scale;
	outFontInfo.ascender = ceil(ascent*scale);
	outFontInfo.descender = floor(descent*scale);
	outFontInfo.lineGap = ceil(lineGap*scale);
	//using lineGap for thickness is pbly abusive but it seems to give good results so far
	outFontInfo.underline_thickness = outFontInfo.lineGap;
	if(outFontInfo.underline_thickness<1)outFontInfo.underline_thickness = 1;
	return outFontInfo;
}

FontInfo TrueTypeFont::getFontInfoByPixelSize(float pixelSize )
{
	assert(m_font != NULL && "TrueTypeFont not initialized" );
	stbtt_fontinfo* fnt = (stbtt_fontinfo*)m_font;
	
	int ascent, descent, lineGap;	
	stbtt_GetFontVMetrics(fnt, &ascent, &descent, &lineGap);

	float scale = stbtt_ScaleForPixelHeight(fnt, pixelSize);	
	//Simply scaling the font ascent or descent might not give correct results. A possible solution is to keep the ceiling of the scaled ascent, and floor of the scaled descent.
	FontInfo outFontInfo;
	outFontInfo.scale = scale;
	outFontInfo.ascender = ceil(ascent*scale);
	outFontInfo.descender = floor(descent*scale);
	outFontInfo.lineGap = ceil(lineGap*scale);
	//using lineGap for thickness is pbly abusive but it seems to give good results so far
	outFontInfo.underline_thickness = outFontInfo.lineGap ;//(int16_t)(1.0f*scale );
	if(outFontInfo.underline_thickness<1)outFontInfo.underline_thickness = 1;
	return outFontInfo;
}

bool TrueTypeFont::getGlyphInfo(const FontInfo& fontInfo, CodePoint_t codePoint, GlyphInfo& outGlyphInfo)
{
	assert(m_font != NULL && "TrueTypeFont not initialized" );
	stbtt_fontinfo* fnt = (stbtt_fontinfo*)m_font;

	int glyphIndex = stbtt_FindGlyphIndex(fnt, codePoint);
	//TODO check glyph validity ?
	
	int x0, y0, x1, y1;
	const float shift_x = 0;
	const float shift_y = 0;
	stbtt_GetGlyphBitmapBoxSubpixel(fnt, glyphIndex, fontInfo.scale, fontInfo.scale, shift_x, shift_y, &x0,&y0,&x1,&y1);
	// get the bbox of the bitmap centered around the glyph origin; so the
	// bitmap width is x1-x0, height is y1-y0, and location to place
	// the bitmap top left is (leftSideBearing*scale, y0).
	// (Note that the bitmap uses y-increases-down, but the shape uses
	// y-increases-up, so CodepointBitmapBox and CodepointBox are inverted.)	
		
	int advanceWidth, leftSideBearing;
	stbtt_GetGlyphHMetrics(fnt, glyphIndex, &advanceWidth, &leftSideBearing);

	int16_t offset_x = leftSideBearing * fontInfo.scale;
	int16_t offset_y = y0;

	outGlyphInfo.glyphIndex = glyphIndex;
	if(fontInfo.fontType == FONT_TYPE_DISTANCE)
	{
		uint32_t w= x1-x0;
		uint32_t h= y1-y0;
		uint32_t dw = 6;
		uint32_t dh = 6;
		if(dw<2) dw = 2;
		if(dh<2) dh = 2;
		outGlyphInfo.width = w + dw*2;
		outGlyphInfo.height = h + dh*2;
		outGlyphInfo.offset_x = offset_x -dw;
		outGlyphInfo.offset_y = offset_y-dh;
	}else
	{
		outGlyphInfo.width = x1-x0;
		outGlyphInfo.height = y1-y0;
		outGlyphInfo.offset_x = offset_x;
		outGlyphInfo.offset_y = offset_y;
	}

	outGlyphInfo.advance_x = advanceWidth;	
	outGlyphInfo.advance_y = (fontInfo.ascender - fontInfo.descender + fontInfo.lineGap);
	outGlyphInfo.texture_x0 = 0;
	outGlyphInfo.texture_y0 = 0;
	outGlyphInfo.texture_x1 = 0;
	outGlyphInfo.texture_y1 = 0;	
	return true;
}

void TrueTypeFont::bakeGlyphAlpha(const FontInfo& fontInfo, const GlyphInfo& glyphInfo, uint8_t* outBuffer)
{
	assert(m_font != NULL && "TrueTypeFont not initialized" );
	stbtt_fontinfo* fnt = (stbtt_fontinfo*)m_font;
    
    const float shift_x = 0;
	const float shift_y = 0;	
	stbtt_MakeGlyphBitmapSubpixel(fnt, outBuffer, glyphInfo.width, glyphInfo.height, glyphInfo.width, fontInfo.scale, fontInfo.scale, shift_x, shift_y, glyphInfo.glyphIndex);
}

void TrueTypeFont::bakeGlyphHinted(const FontInfo& fontInfo, const GlyphInfo& glyphInfo, uint32_t* outBuffer)
{
	assert(m_font != NULL && "TrueTypeFont not initialized" );
	stbtt_fontinfo* fnt = (stbtt_fontinfo*)m_font;
}

void make_distance_map( unsigned char *img, unsigned char *outImg, unsigned int width, unsigned int height )
{
    short * xdist = (short *)  malloc( width * height * sizeof(short) );
    short * ydist = (short *)  malloc( width * height * sizeof(short) );
    double * gx   = (double *) calloc( width * height, sizeof(double) );
    double * gy      = (double *) calloc( width * height, sizeof(double) );
    double * data    = (double *) calloc( width * height, sizeof(double) );
    double * outside = (double *) calloc( width * height, sizeof(double) );
    double * inside  = (double *) calloc( width * height, sizeof(double) );
    int i;

    // Convert img into double (data)
    double img_min = 255, img_max = -255;
    for( i=0; i<width*height; ++i)
    {
        double v = img[i];
        data[i] = v;
        if (v > img_max) img_max = v;
        if (v < img_min) img_min = v;
    }
    // Rescale image levels between 0 and 1
    for( i=0; i<width*height; ++i)
    {
        data[i] = (img[i]-img_min)/img_max;
    }

    // Compute outside = edtaa3(bitmap); % Transform background (0's)
    computegradient( data, width, height, gx, gy);
    edtaa3(data, gx, gy, width, height, xdist, ydist, outside);
    for( i=0; i<width*height; ++i)
        if( outside[i] < 0 )
            outside[i] = 0.0;

    // Compute inside = edtaa3(1-bitmap); % Transform foreground (1's)
    memset(gx, 0, sizeof(double)*width*height );
    memset(gy, 0, sizeof(double)*width*height );
    for( i=0; i<width*height; ++i)
        data[i] = 1 - data[i];
    computegradient( data, width, height, gx, gy);
    edtaa3(data, gx, gy, height, width, xdist, ydist, inside);
    for( i=0; i<width*height; ++i)
        if( inside[i] < 0 )
            inside[i] = 0.0;

    // distmap = outside - inside; % Bipolar distance field
    unsigned char *out = outImg;//(unsigned char *) malloc( width * height * sizeof(unsigned char) );
    for( i=0; i<width*height; ++i)
    {
        outside[i] -= inside[i];
        outside[i] = 128+outside[i]*16;
        if( outside[i] < 0 ) outside[i] = 0;
        if( outside[i] > 255 ) outside[i] = 255;
        out[i] = 255 - (unsigned char) outside[i];
        //out[i] = (unsigned char) outside[i];
    }

    free( xdist );
    free( ydist );
    free( gx );
    free( gy );
    free( data );
    free( outside );
    free( inside );
}

void TrueTypeFont::bakeGlyphDistance(const FontInfo& fontInfo, const GlyphInfo& glyphInfo, uint8_t* outBuffer)
{
	assert(m_font != NULL && "TrueTypeFont not initialized" );
	stbtt_fontinfo* fnt = (stbtt_fontinfo*)m_font;
	
	int x0, y0, x1, y1;
	const float shift_x = 0;
	const float shift_y = 0;
	stbtt_GetGlyphBitmapBoxSubpixel(fnt, glyphInfo.glyphIndex, fontInfo.scale, fontInfo.scale, shift_x, shift_y, &x0,&y0,&x1,&y1);
	uint32_t w= x1-x0;
	uint32_t h= y1-y0;
	stbtt_MakeGlyphBitmapSubpixel(fnt, outBuffer, w, h, w, fontInfo.scale, fontInfo.scale, shift_x, shift_y, glyphInfo.glyphIndex);
	
	uint32_t dw = 6;
	uint32_t dh = 6;	
	if(dw<2) dw = 2;
	if(dh<2) dh = 2;
	
	uint32_t nw = w + dw*2;
	uint32_t nh = h + dh*2;
	assert(nw*nh < 128*128);
	uint32_t buffSize = nw*nh*sizeof(uint8_t);
	uint8_t * alphaImg = (uint8_t *)  malloc( buffSize );	
	memset(alphaImg, 0, nw*nh*sizeof(uint8_t));

	//move it
	
	for(uint32_t  i= dh; i< nh-dh; ++i)
	{
		memcpy(alphaImg+i*nw+dw, outBuffer+(i-dh)*w, w);
	}
	make_distance_map(alphaImg, outBuffer, nw, nh);

	free(alphaImg);
}

}
