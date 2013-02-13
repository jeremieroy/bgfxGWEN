/*
 * Copyright 2011-2013 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef __BGFX_SHADER_H__
#define __BGFX_SHADER_H__

#ifndef __cplusplus

#if BGFX_SHADER_LANGUAGE_HLSL
#	define dFdx(_x) ddx(_x)
#	define dFdy(_y) ddy(-_y)

#	if BGFX_SHADER_LANGUAGE_HLSL > 3
struct BgfxSampler2D
{
	SamplerState m_sampler;
	Texture2D m_texture;
};

vec4 bgfxTexture2D(BgfxSampler2D _sampler, vec2 _coord)
{
	return _sampler.m_texture.Sample(_sampler.m_sampler, _coord);
}

vec4 bgfxTexture2DLod(BgfxSampler2D _sampler, vec2 _coord, float _level)
{
	return _sampler.m_texture.SampleLevel(_sampler.m_sampler, _coord, _level);
}

struct BgfxSampler3D
{
	SamplerState m_sampler;
	Texture3D m_texture;
};

vec4 bgfxTexture3D(BgfxSampler3D _sampler, vec3 _coord)
{
	return _sampler.m_texture.Sample(_sampler.m_sampler, _coord);
}

vec4 bgfxTexture3DLod(BgfxSampler3D _sampler, vec3 _coord, float _level)
{
	return _sampler.m_texture.SampleLevel(_sampler.m_sampler, _coord, _level);
}

struct BgfxSamplerCube
{
	SamplerState m_sampler;
	TextureCube m_texture;
};

vec4 bgfxTextureCube(BgfxSamplerCube _sampler, vec3 _coord)
{
	return _sampler.m_texture.Sample(_sampler.m_sampler, _coord);
}

vec4 bgfxTextureCubeLod(BgfxSamplerCube _sampler, vec3 _coord, float _level)
{
	return _sampler.m_texture.SampleLevel(_sampler.m_sampler, _coord, _level);
}

#		define SAMPLER2D(_name, _reg) \
			uniform SamplerState _name ## Sampler : register(s[_reg]); \
			uniform Texture2D _name ## Texture : register(t[_reg]); \
			static BgfxSampler2D _name = { _name ## Sampler, _name ## Texture }
#		define sampler2D BgfxSampler2D
#		define texture2D(_sampler, _coord) bgfxTexture2D(_sampler, _coord)
#		define texture2DLod(_sampler, _coord, _level) bgfxTexture2DLod(_sampler, _coord, _level)

#		define SAMPLER3D(_name, _reg) \
			uniform SamplerState _name ## Sampler : register(s[_reg]); \
			uniform Texture3D _name ## Texture : register(t[_reg]); \
			static BgfxSampler3D _name = { _name ## Sampler, _name ## Texture }
#		define sampler3D BgfxSampler3D
#		define texture3D(_sampler, _coord) bgfxTexture3D(_sampler, _coord)
#		define texture3DLod(_sampler, _coord, _level) bgfxTexture3DLod(_sampler, _coord, _level)

#		define SAMPLERCUBE(_name, _reg) \
			uniform SamplerState _name ## Sampler : register(s[_reg]); \
			uniform TextureCube _name ## Texture : register(t[_reg]); \
			static BgfxSamplerCube _name = { _name ## Sampler, _name ## Texture }
#		define samplerCube BgfxSamplerCube
#		define textureCube(_sampler, _coord) bgfxTextureCube(_sampler, _coord)
#		define textureCubeLod(_sampler, _coord, _level) bgfxTextureCubeLod(_sampler, _coord, _level)
#	else
#		define SAMPLER2D(_name, _reg) uniform sampler2D _name : register(s ## _reg)
#		define texture2D(_sampler, _coord) tex2D(_sampler, _coord)
#		define texture2DLod(_sampler, _coord, _level) tex2Dlod(_sampler, vec3( (_coord).xy, _level) )
#		define SAMPLER3D(_name, _reg) uniform sampler3D _name : register(s ## _reg)
#		define texture3D(_sampler, _coord) tex3D(_sampler, _coord)
#		define texture3DLod(_sampler, _coord, _level) tex3Dlod(_sampler, vec4( (_coord).xyz, _level) )
#		define SAMPLERCUBE(_name, _reg) uniform samplerCUBE _name : register(s[_reg])
#		define textureCube(_sampler, _coord) texCUBE(_sampler, _coord)
#		define textureCubeLod(_sampler, _coord, _level) texCUBElod(_sampler, vec4( (_coord).xyz, _level) )
#	endif //

#	define vec2_splat(_x) float2(_x, _x)
#	define vec3_splat(_x) float3(_x, _x, _x)
#	define vec4_splat(_x) float4(_x, _x, _x, _x)

#	define bvec2 bool2
#	define bvec3 bool3
#	define bvec4 bool4

vec3 instMul(vec3 _vec, mat3 _mtx) { return mul(_mtx, _vec); }
vec3 instMul(mat3 _mtx, vec3 _vec) { return mul(_vec, _mtx); }
vec4 instMul(vec4 _vec, mat4 _mtx) { return mul(_mtx, _vec); }
vec4 instMul(mat4 _mtx, vec4 _vec) { return mul(_vec, _mtx); }

bvec2 lessThan(vec2 _a, vec2 _b) { return _a < _b; }
bvec3 lessThan(vec3 _a, vec3 _b) { return _a < _b; }
bvec4 lessThan(vec4 _a, vec4 _b) { return _a < _b; }

bvec2 lessThanEqual(vec2 _a, vec2 _b) { return _a <= _b; }
bvec2 lessThanEqual(vec3 _a, vec3 _b) { return _a <= _b; }
bvec2 lessThanEqual(vec4 _a, vec4 _b) { return _a <= _b; }

bvec2 greaterThan(vec2 _a, vec2 _b) { return _a > _b; }
bvec3 greaterThan(vec3 _a, vec3 _b) { return _a > _b; }
bvec4 greaterThan(vec4 _a, vec4 _b) { return _a > _b; }

bvec2 greaterThanEqual(vec2 _a, vec2 _b) { return _a >= _b; }
bvec3 greaterThanEqual(vec3 _a, vec3 _b) { return _a >= _b; }
bvec4 greaterThanEqual(vec4 _a, vec4 _b) { return _a >= _b; }

bvec2 notEqual(vec2 _a, vec2 _b) { return _a != _b; }
bvec3 notEqual(vec3 _a, vec3 _b) { return _a != _b; }
bvec4 notEqual(vec4 _a, vec4 _b) { return _a != _b; }

bvec2 equal(vec2 _a, vec2 _b) { return _a == _b; }
bvec3 equal(vec3 _a, vec3 _b) { return _a == _b; }
bvec4 equal(vec4 _a, vec4 _b) { return _a == _b; }

vec2 mix(vec2 _a, vec2 _b, vec2 _t) { return lerp(_a, _b, _t); }
vec3 mix(vec3 _a, vec3 _b, vec3 _t) { return lerp(_a, _b, _t); }
vec4 mix(vec4 _a, vec4 _b, vec4 _t) { return lerp(_a, _b, _t); }

#elif BGFX_SHADER_LANGUAGE_GLSL
#	define atan2(_x, _y) atan(_x, _y)
#	define frac(_x) fract(_x)
#	define lerp(_x, _y, _t) mix(_x, _y, _t)
#	define mul(_a, _b) ( (_a) * (_b) )
#	define saturate(_x) clamp(_x, 0.0, 1.0)
#	define SAMPLER2D(_name, _reg) uniform sampler2D _name
#	define SAMPLER3D(_name, _reg) uniform sampler3D _name
#	define SAMPLERCUBE(_name, _reg) uniform samplerCube _name
#	define vec2_splat(_x) vec2(_x)
#	define vec3_splat(_x) vec3(_x)
#	define vec4_splat(_x) vec4(_x)

vec3 instMul(vec3 _vec, mat3 _mtx) { return mul(_vec, _mtx); }
vec3 instMul(mat3 _mtx, vec3 _vec) { return mul(_mtx, _vec); }
vec4 instMul(vec4 _vec, mat4 _mtx) { return mul(_vec, _mtx); }
vec4 instMul(mat4 _mtx, vec4 _vec) { return mul(_mtx, _vec); }
#endif // BGFX_SHADER_LANGUAGE_HLSL

#endif // __cplusplus

#endif // __BGFX_SHADER_H__
