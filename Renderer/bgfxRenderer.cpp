#include "bgfxRenderer.h"

#include "Gwen/Utility.h"
#include "Gwen/Font.h"
#include "Gwen/Texture.h"
#include "Gwen/WindowProvider.h"

/*
#define D3DFVF_VERTEXFORMAT2D ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

struct FontData
{
	ID3DXFont*	pFont;
	int			iSpaceWidth;
};
*/

namespace Gwen
{
	namespace Renderer
	{
		bgfx::bgfx()
		{
            /*
			m_pD3D = NULL;
			m_pDevice = pDevice;
			m_iVertNum = 0;

			for ( int i=0; i<MaxVerts; i++ )
			{
				m_pVerts[ i ].z = 0.5f;
				m_pVerts[ i ].rhw = 1.0f;
			}
            */
		}

		bgfx::~bgfx()
		{

		}

		void bgfx::Begin()
		{
            /*
			m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ); 
			m_pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			m_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			m_pDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

			m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			m_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

			m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );
			m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
			m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_CURRENT );

			m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE );
			m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
			m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );

			m_pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			m_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
            */
		}
/*
		void bgfx::End()
		{
			//Flush();
		}


		void bgfx::Flush()
		{
        
			if ( m_iVertNum > 0 )
			{
				m_pDevice->SetFVF( D3DFVF_VERTEXFORMAT2D );
				m_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, m_iVertNum/3, &m_pVerts[0], sizeof(VertexFormat) );

				m_iVertNum = 0;
			}
        
		}

		void bgfx::AddVert( int x, int y )
		{
    
			if ( m_iVertNum >= MaxVerts-1 )
			{
				Flush();
			}

			m_pVerts[ m_iVertNum ].x = (float)x;
			m_pVerts[ m_iVertNum ].y = (float)y;
			m_pVerts[ m_iVertNum ].color = m_Color;

			m_iVertNum++;
    
		}

		void bgfx::AddVert( int x, int y, float u, float v )
		{

			if ( m_iVertNum >= MaxVerts-1 )
			{
				Flush();
			}

			m_pVerts[ m_iVertNum ].x = -0.5f + (float)x;
			m_pVerts[ m_iVertNum ].y = -0.5f + (float)y;
			m_pVerts[ m_iVertNum ].u = u;
			m_pVerts[ m_iVertNum ].v = v;

			m_pVerts[ m_iVertNum ].color = m_Color;

			m_iVertNum++;

		}
        */

		void bgfx::DrawFilledRect( Gwen::Rect rect )
		{
            /*
			if ( m_pCurrentTexture != NULL )
			{
				Flush();
				m_pDevice->SetTexture( 0, NULL );
				m_pCurrentTexture = NULL;
			}	

			Translate( rect );

			AddVert( rect.x, rect.y );
			AddVert( rect.x+rect.w, rect.y );
			AddVert( rect.x, rect.y + rect.h );

			AddVert( rect.x+rect.w, rect.y );
			AddVert( rect.x+rect.w, rect.y+rect.h );
			AddVert( rect.x, rect.y + rect.h );
            */
		}

		void bgfx::SetDrawColor(Gwen::Color color)
		{
            /*
			m_Color = D3DCOLOR_ARGB( color.a, color.r, color.g, color.b );
            */
		}

		void bgfx::LoadFont( Gwen::Font* font )
		{
            /*
			m_FontList.push_back( font );
			// Scale the font according to canvas
			font->realsize = font->size * Scale();

			D3DXFONT_DESC fd;

			memset( &fd, 0, sizeof(fd) );

			wcscpy_s( fd.FaceName, LF_FACESIZE, font->facename.c_str() );

			fd.Width = 0;
			fd.MipLevels = 1;
			fd.CharSet = DEFAULT_CHARSET;
			fd.Height = font->realsize * -1.0f;
			fd.OutputPrecision = OUT_DEFAULT_PRECIS;
			fd.Italic = 0;
			fd.Weight = font->bold ? FW_BOLD : FW_NORMAL;
#ifdef CLEARTYPE_QUALITY
			fd.Quality = font->realsize < 14 ? DEFAULT_QUALITY : CLEARTYPE_QUALITY;
#else 
			fd.Quality = PROOF_QUALITY;
#endif 
			fd.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

			LPD3DXFONT pD3DXFont;
			HRESULT hr = D3DXCreateFontIndirect( m_pDevice, &fd, &pD3DXFont );

			FontData*	pFontData = new FontData();
			pFontData->pFont = pD3DXFont;

			// ID3DXFont doesn't measure trailing spaces, so we measure the width of a space here and store it
			// in the font data - then we can add it to the width when we measure text with trailing spaces.
			{
				RECT rctA = {0,0,0,0};
				pFontData->pFont->DrawTextW( NULL, L"A", -1, &rctA, DT_CALCRECT | DT_LEFT | DT_TOP | DT_SINGLELINE, 0 );

				RECT rctSpc = {0,0,0,0};
				pFontData->pFont->DrawTextW( NULL, L"A A", -1, &rctSpc, DT_CALCRECT | DT_LEFT | DT_TOP | DT_SINGLELINE, 0 );

				pFontData->iSpaceWidth = rctSpc.right - rctA.right * 2;
			}

			font->data = pFontData;
            */
		}

		void bgfx::FreeFont( Gwen::Font* pFont )
		{
            /*
			m_FontList.remove( pFont );

			if ( !pFont->data ) return;

			FontData* pFontData = (FontData*) pFont->data;

			if ( pFontData->pFont )
			{
				pFontData->pFont->Release();
				pFontData->pFont = NULL;
			}

			delete pFontData;
			pFont->data = NULL;
            */

		}

		void bgfx::RenderText( Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString& text )
		{
            /*
			Flush();

			// If the font doesn't exist, or the font size should be changed
			if ( !pFont->data || fabs( pFont->realsize - pFont->size * Scale() ) > 2 )
			{
				FreeFont( pFont );
				LoadFont( pFont );
			}

			FontData* pFontData = (FontData*) pFont->data;
			
			Translate( pos.x, pos.y );

			RECT ClipRect = { pos.x, pos.y, 0, 0 };
			pFontData->pFont->DrawTextW( NULL, text.c_str(), -1, &ClipRect, DT_LEFT | DT_TOP | DT_NOCLIP | DT_SINGLELINE, m_Color );
            */
		}

		Gwen::Point bgfx::MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString& text )
		{
            /*
			// If the font doesn't exist, or the font size should be changed
			if ( !pFont->data || fabs( pFont->realsize - pFont->size * Scale() ) > 2 )
			{
				FreeFont( pFont );
				LoadFont( pFont );
			}

			FontData* pFontData = (FontData*) pFont->data;

			Gwen::Point size;

			if ( text.empty() )
			{
				RECT rct = {0,0,0,0};
				pFontData->pFont->DrawTextW( NULL, L"W", -1, &rct, DT_CALCRECT, 0 );

				return Gwen::Point( 0, rct.bottom );
			}

			RECT rct = {0,0,0,0};
			pFontData->pFont->DrawTextW( NULL, text.c_str(), -1, &rct, DT_CALCRECT | DT_LEFT | DT_TOP | DT_SINGLELINE, 0 );

			for (int i=text.length()-1; i>=0 && text[i] == L' '; i-- )
			{
				rct.right += pFontData->iSpaceWidth;
			}

			return Gwen::Point( rct.right / Scale(), rct.bottom / Scale() );
            */
            return Gwen::Point();
		}

		void bgfx::StartClip()
		{
            /*
			Flush();

			m_pDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );

			const Gwen::Rect& rect = ClipRegion();

			RECT r;

			r.left = ceil( ((float)rect.x) * Scale() );
			r.right = ceil(((float)(rect.x + rect.w)) * Scale());
			r.top = ceil( (float)rect.y * Scale() );
			r.bottom = ceil( ((float)(rect.y + rect.h)) * Scale() );

			m_pDevice->SetScissorRect( &r );
            */
		}

		void bgfx::EndClip()
		{
            /*
			Flush();
			m_pDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
            */
		}

		void bgfx::DrawTexturedRect( Gwen::Texture* pTexture, Gwen::Rect rect, float u1, float v1, float u2, float v2 )
		{
            /*
			IDirect3DTexture9* pImage = (IDirect3DTexture9*) pTexture->data;

			// Missing image, not loaded properly?
			if ( !pImage )
			{
				return DrawMissingImage( rect );
			}

			Translate( rect );

			if ( m_pCurrentTexture != pImage )
			{
				Flush();
				m_pDevice->SetTexture( 0, pImage );
				m_pCurrentTexture = pImage;
			}		

			AddVert( rect.x, rect.y,			u1, v1 );
			AddVert( rect.x+rect.w, rect.y,		u2, v1 );
			AddVert( rect.x, rect.y + rect.h,	u1, v2 );

			AddVert( rect.x+rect.w, rect.y,		u2, v1 );
			AddVert( rect.x+rect.w, rect.y+rect.h, u2, v2 );
			AddVert( rect.x, rect.y + rect.h, u1, v2 );
            */
			
		}

		void bgfx::LoadTexture( Gwen::Texture* pTexture )
		{
            /*
			IDirect3DTexture9* ptr = NULL;
			D3DXIMAGE_INFO ImageInfo;
			HRESULT hr = D3DXCreateTextureFromFileExW( m_pDevice, pTexture->name.GetUnicode().c_str(), 0, 0, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, &ImageInfo, NULL, &ptr );
			if ( hr != S_OK )
			{
				return;
			}

			pTexture->data = ptr;
			pTexture->width = ImageInfo.Width;
			pTexture->height = ImageInfo.Height;
            */
		}

		void bgfx::FreeTexture( Gwen::Texture* pTexture )
		{
            /*
			IDirect3DTexture9* pImage = (IDirect3DTexture9*) pTexture->data;
			if ( !pImage ) return;

			pImage->Release();
			pTexture->data = NULL;

			return;
            */
		}

		Gwen::Color bgfx::PixelColour( Gwen::Texture* pTexture, unsigned int x, unsigned int y, const Gwen::Color& col_default )
		{
            /*
			IDirect3DTexture9* pImage = (IDirect3DTexture9*) pTexture->data;
			if ( !pImage ) return col_default;

			IDirect3DSurface9* pSurface = NULL;

			if ( pImage->GetSurfaceLevel( 0, &pSurface ) != S_OK ) return col_default;
			if ( !pSurface ) return col_default;

			D3DLOCKED_RECT lockedRect;
			pSurface->LockRect( &lockedRect, NULL, D3DLOCK_READONLY );
			DWORD* pixels = (DWORD*)lockedRect.pBits;
			D3DXCOLOR color = pixels[lockedRect.Pitch / sizeof(DWORD) * y + x];
			pSurface->UnlockRect();

			pSurface->Release();

			return Gwen::Color( color.r*255, color.g*255, color.b*255, color.a*255 );
            */
             return Gwen::Color();
		}

		void bgfx::Release()
		{
            /*
			Font::List::iterator it = m_FontList.begin();

			while ( it != m_FontList.end() )
			{
				FreeFont( *it );
				it = m_FontList.begin();
			}
            */
		}
/*
		void bgfx::FillPresentParameters( Gwen::WindowProvider* pWindow, D3DPRESENT_PARAMETERS& Params )
		{
        
			HWND pHWND = (HWND)pWindow->GetWindow();

			RECT ClientRect;
			GetClientRect( pHWND, &ClientRect );

			ZeroMemory( &Params, sizeof( Params ) );

			Params.Windowed						= true;
			Params.SwapEffect					= D3DSWAPEFFECT_DISCARD;
			Params.BackBufferWidth				= ClientRect.right;
			Params.BackBufferHeight				= ClientRect.bottom;
			Params.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
			Params.BackBufferFormat				= D3DFMT_X8R8G8B8;
			Params.PresentationInterval			= D3DPRESENT_INTERVAL_IMMEDIATE;
            
		}
*/
		bool bgfx::InitializeContext( Gwen::WindowProvider* pWindow )
		{
            /*
			HWND pHWND = (HWND)pWindow->GetWindow();

			m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
			if ( !m_pD3D ) return false;

			

			D3DCAPS9 D3DCaps;
			m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &D3DCaps );

			DWORD BehaviourFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			if( D3DCaps.VertexProcessingCaps != 0 ) BehaviourFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

			D3DPRESENT_PARAMETERS Params;
			FillPresentParameters( pWindow, Params );

			HRESULT hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, pHWND, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Params, &m_pDevice );
			if ( FAILED(hr) )
			{
				return false;
			}

            */

			return true;
		}

		bool bgfx::ShutdownContext( Gwen::WindowProvider* pWindow )
		{
			/*
            if ( m_pDevice )
			{
				m_pDevice->Release();
				m_pDevice = NULL;
			}

			if ( m_pD3D )
			{
				m_pD3D->Release();
				m_pD3D = NULL;
			}
            */
			return true;
		}

		bool bgfx::PresentContext( Gwen::WindowProvider* pWindow )
		{
			//m_pDevice->Present( NULL, NULL, NULL, NULL );
			return true;
		}

		bool bgfx::ResizedContext( Gwen::WindowProvider* pWindow, int w, int h )
		{
            /*
			// Force setting the current texture again
			m_pCurrentTexture = NULL;

			// Free any unmanaged resources (fonts)
			Release();

			// Get the new window size from the HWND
			D3DPRESENT_PARAMETERS Params;
			FillPresentParameters( pWindow, Params );

			// And reset the device!
			m_pDevice->Reset( &Params );
            */
			return true;
		}

		bool bgfx::BeginContext( Gwen::WindowProvider* pWindow )
		{
			//m_pDevice->BeginScene();
			//m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 128, 128, 128 ), 1, 0 );

			return true;
		}

		bool bgfx::EndContext( Gwen::WindowProvider* pWindow )
		{
			//m_pDevice->EndScene();
			return true;
		}

	}
}
