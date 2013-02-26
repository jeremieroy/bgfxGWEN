#include "TextBufferManager.h"
#include <assert.h>

namespace bgfx_font
{

const uint16_t MAX_TEXT_BUFFER_COUNT = 64;

long int fsize(FILE* _file)
{
	long int pos = ftell(_file);
	fseek(_file, 0L, SEEK_END);
	long int size = ftell(_file);
	fseek(_file, pos, SEEK_SET);
	return size;
}

static const bgfx::Memory* loadShader(const char* _shaderPath, const char* _shaderName)
{
	char out[512];
	strcpy(out, _shaderPath);
	strcat(out, _shaderName);
	strcat(out, ".bin");

	FILE* file = fopen(out, "rb");
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


TextBufferManager::TextBufferManager(FontManager* fontManager):m_fontManager(fontManager), m_textBufferHandles(MAX_TEXT_BUFFER_COUNT)
{
	m_textBuffers = new BufferCache[MAX_TEXT_BUFFER_COUNT];
}

TextBufferManager::~TextBufferManager()
{
	assert(m_textBufferHandles.getNumHandles() == 0 && "All the text buffers must be destroyed before destroying the manager");
	delete[] m_textBuffers;

	bgfx::destroyUniform(m_u_texColor);
	bgfx::destroyProgram(m_basicProgram);	
	
}

void TextBufferManager::init(FontManager* fontManager, const char* shaderPath)
{
	m_fontManager = fontManager;

	m_vertexDecl.begin();
	m_vertexDecl.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Int16);
	m_vertexDecl.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Int16,true);
	m_vertexDecl.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8,true);
	m_vertexDecl.end();

	m_u_texColor = bgfx::createUniform("u_texColor", bgfx::UniformType::Uniform1iv);

	const bgfx::Memory* mem;
	mem = loadShader(shaderPath, "vs_font_basic");
	bgfx::VertexShaderHandle vsh = bgfx::createVertexShader(mem);
	mem = loadShader(shaderPath, "fs_font_basic");
	bgfx::FragmentShaderHandle fsh = bgfx::createFragmentShader(mem);
	m_basicProgram = bgfx::createProgram(vsh, fsh);
	bgfx::destroyVertexShader(vsh);
	bgfx::destroyFragmentShader(fsh);

	mem = loadShader(shaderPath, "vs_font_distance_field");
	vsh = bgfx::createVertexShader(mem);
	mem = loadShader(shaderPath, "fs_font_distance_field");
	fsh = bgfx::createFragmentShader(mem);
	m_distanceProgram = bgfx::createProgram(vsh, fsh);
	bgfx::destroyVertexShader(vsh);
	bgfx::destroyFragmentShader(fsh);

	
}

TextBufferHandle TextBufferManager::createTextBuffer(FontType _type, BufferType bufferType)
{	
	uint16_t textIdx = m_textBufferHandles.alloc();
	BufferCache& bc = m_textBuffers[textIdx];
	
	bc.textBuffer.setFontManager(m_fontManager);
	bc.textBuffer.clearTextBuffer();
	bc.fontType = _type;
	bc.bufferType = bufferType;	
	bc.indexBufferHandle = bgfx::invalidHandle;
	bc.vertexBufferHandle = bgfx::invalidHandle;

	return TextBufferHandle(textIdx);
}

void TextBufferManager::destroyTextBuffer(TextBufferHandle handle)
{	
	assert( handle.isValid() );
	
	BufferCache& bc = m_textBuffers[handle.idx];
	m_textBufferHandles.free(handle.idx);

	if(bc.vertexBufferHandle == bgfx::invalidHandle ) return;
	
	switch(bc.bufferType)
	{
	case STATIC:
		{
		bgfx::IndexBufferHandle ibh;
		bgfx::VertexBufferHandle vbh;
		ibh.idx = bc.indexBufferHandle;
		vbh.idx = bc.vertexBufferHandle;
		bgfx::destroyIndexBuffer(ibh);
		bgfx::destroyVertexBuffer(vbh);
		}

		break;
	case DYNAMIC:
		bgfx::DynamicIndexBufferHandle ibh;
		bgfx::DynamicVertexBufferHandle vbh;
		ibh.idx = bc.indexBufferHandle;
		vbh.idx = bc.vertexBufferHandle;
		bgfx::destroyDynamicIndexBuffer(ibh);
		bgfx::destroyDynamicVertexBuffer(vbh);
	
		break;
	case TRANSIENT: //naturally destroyed
		break;		
	}	
}

void TextBufferManager::submitTextBuffer(TextBufferHandle _handle, uint8_t _id, int32_t _depth)
{
	assert( _handle.isValid() );
	BufferCache& bc = m_textBuffers[_handle.idx];
	
	size_t indexSize = bc.textBuffer.getIndexCount() * bc.textBuffer.getIndexSize();
	size_t vertexSize = bc.textBuffer.getVertexCount() * bc.textBuffer.getVertexSize();
	const bgfx::Memory* mem;	

	if(bc.fontType==FONT_TYPE_DISTANCE)
	{
		bgfx::setProgram(m_distanceProgram);
	}else
	{
		bgfx::setProgram(m_basicProgram);
	}

	
	

	bgfx::setTexture(0, m_u_texColor, bc.textBuffer.getTextureHandle() );

	bgfx::setState( BGFX_STATE_RGB_WRITE
			|BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
			//|BGFX_STATE_ALPHA_TEST
			//|BGFX_STATE_DEPTH_WRITE
			//|BGFX_STATE_DEPTH_TEST_LESS
			);
	
	switch(bc.bufferType)
	{
		case STATIC:
		{
			bgfx::IndexBufferHandle ibh;
			bgfx::VertexBufferHandle vbh;

			if(bc.vertexBufferHandle == bgfx::invalidHandle)
			{
				mem = bgfx::alloc(indexSize);
				memcpy(mem->data, bc.textBuffer.getIndexBuffer(), indexSize);
				ibh = bgfx::createIndexBuffer(mem);

				mem = bgfx::alloc(vertexSize);
				memcpy(mem->data, bc.textBuffer.getVertexBuffer(), vertexSize);
				vbh = bgfx::createVertexBuffer(mem, m_vertexDecl);

				bc.indexBufferHandle = ibh.idx ;
				bc.vertexBufferHandle = vbh.idx;
			}else
			{
				ibh.idx = bc.indexBufferHandle;
				vbh.idx = bc.vertexBufferHandle;
			}
			bgfx::setVertexBuffer(vbh,  bc.textBuffer.getVertexCount());
			bgfx::setIndexBuffer(ibh, bc.textBuffer.getIndexCount());
		}break;
		case DYNAMIC:
		{
			bgfx::DynamicIndexBufferHandle ibh;
			bgfx::DynamicVertexBufferHandle vbh;

			if(bc.vertexBufferHandle == bgfx::invalidHandle)
			{
				mem = bgfx::alloc(indexSize);
				memcpy(mem->data, bc.textBuffer.getIndexBuffer(), indexSize);
				ibh = bgfx::createDynamicIndexBuffer(mem);

				mem = bgfx::alloc(vertexSize);
				memcpy(mem->data, bc.textBuffer.getVertexBuffer(), vertexSize);
				vbh = bgfx::createDynamicVertexBuffer(mem, m_vertexDecl);

				bc.indexBufferHandle = ibh.idx ;
				bc.vertexBufferHandle = vbh.idx;
			}else
			{
				ibh.idx = bc.indexBufferHandle;
				vbh.idx = bc.vertexBufferHandle;

				static int i=0;
				//if(i++ < 5)
				{				
				mem = bgfx::alloc(indexSize);
				memcpy(mem->data, bc.textBuffer.getIndexBuffer(), indexSize);
				bgfx::updateDynamicIndexBuffer(ibh, mem);

				mem = bgfx::alloc(vertexSize);
				memcpy(mem->data, bc.textBuffer.getVertexBuffer(), vertexSize);
				bgfx::updateDynamicVertexBuffer(vbh, mem);				
				}
			}
			bgfx::setVertexBuffer(vbh,  bc.textBuffer.getVertexCount());
			bgfx::setIndexBuffer(ibh, bc.textBuffer.getIndexCount());
			
		}break;
		case TRANSIENT:
		{
			bgfx::TransientIndexBuffer tib;
			bgfx::TransientVertexBuffer tvb;
			bgfx::allocTransientIndexBuffer(&tib, bc.textBuffer.getIndexCount());
			bgfx::allocTransientVertexBuffer(&tvb, bc.textBuffer.getVertexCount(), m_vertexDecl);
			memcpy(tib.data, bc.textBuffer.getIndexBuffer(), indexSize);
			memcpy(tvb.data, bc.textBuffer.getVertexBuffer(), vertexSize);
			bgfx::setVertexBuffer(&tvb,  bc.textBuffer.getVertexCount());
			bgfx::setIndexBuffer(&tib, bc.textBuffer.getIndexCount());
		}break;	
	}

	bgfx::submit(_id, _depth);
}

void TextBufferManager::submitTextBufferMask(TextBufferHandle _handle, uint32_t _viewMask, int32_t _depth)
{
}

TextBuffer* TextBufferManager::getTextBuffer(TextBufferHandle _handle)
{
	assert( _handle.isValid() );
	BufferCache& bc = m_textBuffers[_handle.idx];
	return &bc.textBuffer;
}

}
