#include <bgfx.h>

#include <Gwen/Gwen.h>
#include <Gwen/Skins/Simple.h>
#include <Gwen/Skins/TexturedBase.h>
#include <Gwen/UnitTest/UnitTest.h>
#include <Gwen/Input/Windows.h>

#include "bgfxRenderer.h"

#include "Gwen/Renderers/OpenGL_DebugFont.h"

#include <common/entry.h>
#include <common/dbg.h>
#include <common/math.h>
#include <common/processevents.h>

#include "bgfx_font.h"
//#include "TrueTypeFont.h"
//#include "FontManager.h"

#include <stdio.h>
#include <string.h>


static const char* s_shaderPath = NULL;

static void shaderFilePath(char* _out, const char* _name)
{
	strcpy(_out, s_shaderPath);
	strcat(_out, _name);
	strcat(_out, ".bin");
}

long int fsize(FILE* _file)
{
	long int pos = ftell(_file);
	fseek(_file, 0L, SEEK_END);
	long int size = ftell(_file);
	fseek(_file, pos, SEEK_SET);
	return size;
}

static const bgfx::Memory* load(const char* _filePath)
{
	FILE* file = fopen(_filePath, "rb");
	if (NULL != file)
	{
		uint32_t size = (uint32_t)fsize(file);
		const bgfx::Memory* mem = bgfx::alloc(size+1);
		size_t ignore = fread(mem->data, 1, size, file);
		BX_UNUSED(ignore);
		fclose(file);
		mem->data[mem->size-1] = '\0';
		return mem;
	}

	return NULL;
}

static const bgfx::Memory* loadShader(const char* _name)
{
	char filePath[512];
	shaderFilePath(filePath, _name);
	return load(filePath);
}

static const bgfx::Memory* loadTexture(const char* _name)
{
	char filePath[512];
	strcpy(filePath, "textures/");
	strcat(filePath, _name);
	return load(filePath);
}




struct Vertex
{
	uint16_t x,y;
	uint16_t u,v;
	float r,g,b,a;
};

/*
(-1 -1) (1 -1)
(-1  1) (1  1) 
*/

//1 - 0
const uint16_t TW = 512;  //512.0f;
const uint16_t TM = 32767;//512*50;//65535; //1.0f;
static Vertex s_quadVertices[4] =
{
	{  0, TW,  0,  0, 1.0f, 1.0 ,1.0, 1.0f},
	{ TW, TW, TM,  0, 1.0f, 1.0 ,1.0, 1.0f},
	{  0,  0,  0, TM, 1.0f, 1.0 ,1.0, 1.0f},
	{ TW,  0, TM, TM, 1.0f, 1.0 ,1.0, 1.0f}
};

static uint16_t s_quadVerticesIdx[6] = { 0,2,1, 1,2,3 };

int _main_(int _argc, char** _argv)
{
    uint32_t width = 1280;
	uint32_t height = 720;
	uint32_t debug = BGFX_DEBUG_TEXT;
	uint32_t reset = 0;

	bgfx::init();
	

	bgfx::reset(width, height);

	// Enable debug text.
	bgfx::setDebug(debug);

	// Set view 0 clear state.
	bgfx::setViewClear(0
		, BGFX_CLEAR_COLOR_BIT|BGFX_CLEAR_DEPTH_BIT
		, 0x303030ff
		, 1.0f
		, 0
		);

    // Setup root path for binary shaders. Shader binaries are different 
	// for each renderer.
	switch (bgfx::getRendererType() )
	{
	default:
	case bgfx::RendererType::Direct3D9:
		s_shaderPath = "shaders/dx9/";
		break;

	case bgfx::RendererType::Direct3D11:
		s_shaderPath = "shaders/dx11/";
		break;

	case bgfx::RendererType::OpenGL:
		s_shaderPath = "shaders/glsl/";
		break;

	case bgfx::RendererType::OpenGLES2:
	case bgfx::RendererType::OpenGLES3:
		s_shaderPath = "shaders/gles/";
		break;
	}
	bgfx_font::init(s_shaderPath);

   
    Gwen::Renderer::bgfxRenderer * pRenderer = new Gwen::Renderer::bgfxRenderer(0,s_shaderPath, "textures/");
	pRenderer->Init();

	//
	// Create a GWEN skin
	//
	//Gwen::Skin::TexturedBase* pSkin = new Gwen::Skin::TexturedBase( pRenderer );
	//pSkin->Init("DefaultSkin.png");

	Gwen::Skin::Simple* pSkin = new Gwen::Skin::Simple();
	pSkin->SetRender(pRenderer);
	//pSkin->Init("DefaultSkin.png");

	//
	// Create a Canvas (it's root, on which all other GWEN panels are created)
	//
	Gwen::Controls::Canvas* pCanvas = new Gwen::Controls::Canvas( pSkin );
	pCanvas->SetSize( 998, 650 - 24 );
	pCanvas->SetDrawBackground( true );
	pCanvas->SetBackgroundColor( Gwen::Color( 150, 170, 170, 255 ) );

	//
	// Create our unittest control (which is a Window with controls in it)
	//
	UnitTest* pUnit = new UnitTest( pCanvas );
	pUnit->SetPos( 10, 10 );

	//
	// Create a Windows Control helper 
	// (Processes Windows MSG's and fires input at GWEN)
	//
	Gwen::Input::Windows GwenInput;
	GwenInput.Initialize( pCanvas );
    
	//bgfx_font::TrueTypeFont* font = new  bgfx_font::TrueTypeFont();
    //font->loadFont("c:/windows/fonts/times.ttf");

	bgfx_font::TextureAtlasHandle atlas = bgfx_font::createTextureAtlas(bgfx_font::TEXTURE_TYPE_ALPHA, 512,512);
 	
	bgfx_font::TrueTypeHandle times_tt = bgfx_font::loadTrueTypeFont("c:/windows/fonts/times.ttf");
	bgfx_font::TrueTypeHandle comic_tt = bgfx_font::loadTrueTypeFont("c:/windows/fonts/comic.ttf");
	bgfx_font::TrueTypeHandle calibri_tt = bgfx_font::loadTrueTypeFont("c:/windows/fonts/calibri.ttf");

	std::vector<bgfx_font::FontHandle> fonts;	
	for(int i = 12; i < 36; i+=2)
	{		
		bgfx_font::FontHandle font = bgfx_font::getFontByEmSize(times_tt, i);
		bgfx_font::preloadGlyph(font, L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \n");
		fonts.push_back(font);
		/*
		bgfx_font::FontHandle font2 = bgfx_font::getFontByPixelSize(comic_tt, i);
		bgfx_font::preloadGlyph(font2, L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ");	
		fonts.push_back(font2);

		bgfx_font::FontHandle font3 = bgfx_font::getFontByPixelSize(calibri_tt, i);
		bgfx_font::preloadGlyph(font3, L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ");	
		fonts.push_back(font3);
		*/
	}

	
	
	std::vector<uint8_t> buffer;
	//float emSize = 18.0f;
	//bgfx_font::GlyphInfo glyphInfo;
	//stash.getGlyphInfo(font, 'a', glyphInfo);

/*
    bgfx_font::Rect16 rect = stash.allocateRegion(glyphSize.width, glyphSize.height);
	buffer.resize(glyphSize.width*glyphSize.height);
    font->bakeGlyphAlpha('a', emSize, &buffer[0]);
	stash.updateRegion(rect, &buffer[0]);

	glyphSize = font->getGlyphSize('M', 192.0f);
    rect = stash.allocateRegion(glyphSize.width, glyphSize.height);
	buffer.resize(glyphSize.width*glyphSize.height);
    font->bakeGlyphAlpha('M', 192.0f, &buffer[0]);
	stash.updateRegion(rect, &buffer[0]);
	*/
	
	// Create vertex stream declaration.
	bgfx::VertexDecl vertexDecl;
	vertexDecl.begin();
		vertexDecl.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Int16);
		vertexDecl.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Int16,true,false);
		vertexDecl.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float);
	vertexDecl.end();
	
	const bgfx::Memory* mem;

	// Create static vertex buffer.
	mem = bgfx::makeRef(s_quadVertices, sizeof(s_quadVertices) );
	bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(mem, vertexDecl);

	// Create static index buffer.
	mem = bgfx::makeRef(s_quadVerticesIdx, sizeof(s_quadVerticesIdx) );
	bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(mem);

	// Create texture sampler uniforms.
	bgfx::UniformHandle u_texColor = bgfx::createUniform("u_texColor", bgfx::UniformType::Uniform1iv);	
	//bgfx::UniformHandle u_color0 = bgfx::createUniform("v_color0", bgfx::UniformType::Uniform4fv);

	// Load vertex shader.
	mem = loadShader("vs_font_basic");
	bgfx::VertexShaderHandle vsh = bgfx::createVertexShader(mem);

	// Load fragment shader.
	mem = loadShader("fs_font_basic");
	bgfx::FragmentShaderHandle fsh = bgfx::createFragmentShader(mem);

	// Create program from shaders.
	bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh);

	// We can destroy vertex and fragment shader here since
	// their reference is kept inside bgfx after calling createProgram.
	// Vertex and fragment shader will be destroyed once program is^
	// destroyed.
	bgfx::destroyVertexShader(vsh);
	bgfx::destroyFragmentShader(fsh);
	
	bgfx_font::TextBufferHandle staticText = bgfx_font::createTextBuffer(bgfx_font::FONT_TYPE_ALPHA, bgfx_font::STATIC, 1024);
	bgfx_font::setPenPosition(staticText, 520.0f,500.f);
	
	for(size_t i = 0; i< fonts.size(); ++i)
	{		
		bgfx_font::appendText(staticText, fonts[i], L"The quick brown fox jumps over the lazy dog\n");		
	}
	

	bgfx_font::TextBufferHandle staticText2 = bgfx_font::createTextBuffer(bgfx_font::FONT_TYPE_ALPHA, bgfx_font::STATIC, 1024);
	//bgfx_font::setPenPosition(staticText2, 52.f,52.f);
	//bgfx_font::appendText(staticText2, fonts[6], L"The quick brown fox jump over the lazy dogs");

    //void bakeGlyphAlpha(uint32_t codePoint, float size, uint8_t* outBuffer);

    while (!processEvents(width, height, debug, reset) )
	{
		// Set view 0 default viewport.
		bgfx::setViewRect(0, 0, 0, width, height);

		// This dummy draw call is here to make sure that view 0 is cleared
		// if no other draw calls are submitted to view 0.
		bgfx::submit(0);

		// Use debug font to print information about this example.
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(0, 1, 0x4f, "bgfx/examples/00-helloworld");
		bgfx::dbgTextPrintf(0, 2, 0x6f, "Description: Initialization and debug text.");

		
		float at[3] = { -10.0f, -10.0f, 0.0f };
		float eye[3] = {-10.0f, -10.0f, -1.0f };
		
		float view[16];
		float proj[16];
		mtxLookAt(view, eye, at);
		//mtxProj(proj, 60.0f, 16.0f/9.0f, 0.1f, 1000.0f);
		mtxOrtho(proj, 0,width,0,height,0.1f, 1000.0f);
		// Set view and projection matrix for view 0.
		bgfx::setViewTransform(0, view, proj);

		// Set vertex and fragment shaders.
		bgfx::setProgram(program);

		// Set vertex and index buffer.
		bgfx::setVertexBuffer(vbh);
		bgfx::setIndexBuffer(ibh);

		bgfx::setTexture(0, u_texColor, bgfx_font::getTextureHandle(atlas));

		bgfx::setState( BGFX_STATE_RGB_WRITE
				|BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
				//|BGFX_STATE_ALPHA_TEST
				|BGFX_STATE_DEPTH_WRITE
				|BGFX_STATE_DEPTH_TEST_LESS
				);

		// Submit primitive for rendering to view 0.
		bgfx::submit(0);


		bgfx_font::submitTextBuffer(staticText, 0);
		//bgfx_font::submitTextBuffer(staticText2, 0);
		//pCanvas->RenderCanvas();
		
        // Advance to next frame. Rendering thread will be kicked to 
		// process submitted rendering primitives.
		bgfx::frame();
	}

	bgfx::destroyIndexBuffer(ibh);
	bgfx::destroyVertexBuffer(vbh);
	bgfx::destroyUniform(u_texColor);
	//bgfx::destroyUniform(u_color0);

	bgfx::destroyProgram(program);


	bgfx_font::unloadTrueTypeFont(times_tt);
	bgfx_font::unloadTrueTypeFont(comic_tt);
	bgfx_font::unloadTrueTypeFont(calibri_tt);

	for(size_t i=0; i<fonts.size();++i)
	{
		bgfx_font::destroyFont(fonts[i]);
	}
	bgfx_font::destroyTextBuffer(staticText);
	bgfx_font::destroyTextBuffer(staticText2);
	bgfx_font::destroyTextureAtlas(atlas);
	bgfx_font::shutdown();
	// Shutdown bgfx.
    bgfx::shutdown();

	return 0;

    /*
	//
	// Begin the main game loop
	//
	MSG msg;
	while( true )
	{
		// Skip out if the window is closed
		if ( !IsWindowVisible( g_pHWND ) )
			break;

		// If we have a message from windows..
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			// .. give it to the input handler to process
			GwenInput.ProcessMessage( msg );

			// if it's QUIT then quit..
			if ( msg.message == WM_QUIT )
				break;


			// Handle the regular window stuff..
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}

		// Main OpenGL Render Loop
		{
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			pCanvas->RenderCanvas();

			SwapBuffers( GetDC( g_pHWND ) );
		}
	}

	// Clean up OpenGL
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( OpenGLContext );

	delete pCanvas;
	delete pSkin;
	delete pRenderer;
    */

}
