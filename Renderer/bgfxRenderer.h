#pragma once

#include "Gwen/Gwen.h"
#include "Gwen/BaseRender.h"

#include <bgfx.h>
#include <bx/bx.h>
#include <bx/timer.h>
/*
#include "../common/entry.h"
#include "../common/dbg.h"
#include "../common/math.h"
#include "../common/processevents.h"

#include <D3D9.h>
#include <D3DX9Core.h>

#pragma comment( lib, "D3D9.lib" )
#pragma comment( lib, "D3Dx9.lib" )
*/

namespace Gwen 
{
	namespace Renderer 
	{

		class bgfxRenderer : public Gwen::Renderer::Base
		{
			public:                
				bgfxRenderer( int viewID, const char* shaderPath, const char* texturePath  );
				~bgfxRenderer();

                virtual void Init();

				virtual void Begin();
				virtual void End();				

				virtual void SetDrawColor(Gwen::Color color);

				virtual void DrawFilledRect( Gwen::Rect rect );

                void StartClip();
				void EndClip();

                virtual void LoadTexture( Gwen::Texture* pTexture );
				virtual void FreeTexture( Gwen::Texture* pTexture );
				virtual void DrawTexturedRect( Gwen::Texture* pTexture, Gwen::Rect pTargetRect, float u1=0.0f, float v1=0.0f, float u2=1.0f, float v2=1.0f );
                //use default implementation
				//virtual void DrawMissingImage( Gwen::Rect pTargetRect );
				virtual Gwen::Color PixelColour( Gwen::Texture* pTexture, unsigned int x, unsigned int y, const Gwen::Color& col_default = Gwen::Color( 255, 255, 255, 255 ) );

                //virtual ICacheToTexture* GetCTT() { return NULL; }

				virtual void LoadFont( Gwen::Font* pFont );
				virtual void FreeFont( Gwen::Font* pFont );
				virtual void RenderText( Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString& text );
				virtual Gwen::Point MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString& text );

                //
				// No need to implement these functions in your derived class, but if 
				// you can do them faster than the default implementation it's a good idea to.
				//
                /*
                virtual void DrawLinedRect( Gwen::Rect rect );
				virtual void DrawPixel( int x, int y );
				virtual void DrawShavedCornerRect( Gwen::Rect rect, bool bSlight = false );
				virtual Gwen::Point MeasureText( Gwen::Font* pFont, const Gwen::String& text );
				virtual void RenderText( Gwen::Font* pFont, Gwen::Point pos, const Gwen::String& text );
                */

			public:

				//
				// Self Initialization
				//

				virtual bool InitializeContext( Gwen::WindowProvider* pWindow );
				virtual bool ShutdownContext( Gwen::WindowProvider* pWindow );
				virtual bool PresentContext( Gwen::WindowProvider* pWindow );
				virtual bool ResizedContext( Gwen::WindowProvider* pWindow, int w, int h );
				virtual bool BeginContext( Gwen::WindowProvider* pWindow );
				virtual bool EndContext( Gwen::WindowProvider* pWindow );

			protected:
                int m_viewID;                
                //bgfx::DynamicVertexBufferHandle m_vertexBufferHandle;
                //bgfx::DynamicIndexBufferHandle m_indexBufferHandle;
                bgfx::VertexDecl m_posUVColorDecl;
                bgfx::ProgramHandle m_flatProgram;
                bgfx::ProgramHandle m_texturedProgram;

                bgfx::TextureHandle m_currentTexture;

                uint32_t m_color;
                uint32_t m_width;
	            uint32_t m_height;
            protected:
                void Flush();
				void AddVert( int x, int y );
				void AddVert( int x, int y, float u, float v );

                const bgfx::Memory* loadShader(const char* _name);
                const bgfx::Memory* loadTexture(const char* _name);
                const char* m_shaderPath;
                const char* m_texturePath;

                
                struct VertexFormat
				{
					float x, y; //could be packed
					float u, v; //could be packed
					uint32_t color; //0xAABBGGRR
				};

                static const  int		MAX_VERTICES = 1024;                
				VertexFormat		    m_vertices[MAX_VERTICES];
				int						m_verticesCount;
                //uint16_t                m_indices[ ];

                /*
				virtual void FillPresentParameters( Gwen::WindowProvider* pWindow, D3DPRESENT_PARAMETERS& Params );

			protected:

				void*				m_pCurrentTexture;
				IDirect3DDevice9*	m_pDevice;
				IDirect3D9*			m_pD3D;
				DWORD				m_Color;
				Gwen::Font::List	m_FontList;
				//Gwen::Texture::List	m_TextureList;				
                */

		};

	}
}
