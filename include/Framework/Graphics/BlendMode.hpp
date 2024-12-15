#ifndef BLENDMODE_H
#define BLENDMODE_H

#include "GL/glew.h"
struct BlendMode {
	BlendMode() : 
		srcRGB(GL_SRC_ALPHA), 
		dstRGB(GL_ONE_MINUS_SRC_ALPHA),
		srcAlpha(GL_SRC_ALPHA),
		dstAlpha(GL_ONE_MINUS_SRC_ALPHA),
		RGBequation(GL_FUNC_ADD),
		AlphaEquation(GL_FUNC_ADD)
	{}
	BlendMode(GLenum p_equation) :
		srcRGB(GL_SRC_ALPHA),
		dstRGB(GL_ONE_MINUS_SRC_ALPHA),
		srcAlpha(GL_SRC_ALPHA),
		dstAlpha(GL_ONE_MINUS_SRC_ALPHA),
		RGBequation(p_equation),
		AlphaEquation(p_equation)
	{}
	BlendMode(GLenum p_src, GLenum p_dst) :
		srcRGB(p_src),
		dstRGB(p_dst),
		srcAlpha(p_src),
		dstAlpha(p_dst),
		RGBequation(GL_FUNC_ADD),
		AlphaEquation(GL_FUNC_ADD)
	{}
	BlendMode(GLenum p_src, GLenum p_dst, GLenum p_equation) :
		srcRGB(p_src),
		dstRGB(p_dst),
		srcAlpha(p_src),
		dstAlpha(p_dst),
		RGBequation(p_equation),
		AlphaEquation(p_equation)
	{}
	BlendMode(GLenum p_srcRGB, GLenum p_dstRGB, GLenum p_srcAlpha, GLenum p_dstAlpha) :
		srcRGB(p_srcRGB),
		dstRGB(p_dstRGB),
		srcAlpha(p_srcAlpha),
		dstAlpha(p_dstAlpha),
		RGBequation(GL_FUNC_ADD),
		AlphaEquation(GL_FUNC_ADD)
	{}
	BlendMode(GLenum p_srcRGB, GLenum p_dstRGB, GLenum p_srcAlpha, GLenum p_dstAlpha, GLenum p_equation) :
		srcRGB(p_srcRGB),
		dstRGB(p_dstRGB),
		srcAlpha(p_srcAlpha),
		dstAlpha(p_dstAlpha),
		RGBequation(p_equation),
		AlphaEquation(p_equation)
	{}
	BlendMode(GLenum p_srcRGB, GLenum p_dstRGB, GLenum p_srcAlpha, GLenum p_dstAlpha, GLenum p_RGBequation, GLenum p_AlphaEquation) :
		srcRGB(p_srcRGB),
		dstRGB(p_dstRGB),
		srcAlpha(p_srcAlpha),
		dstAlpha(p_dstAlpha),
		RGBequation(p_RGBequation),
		AlphaEquation(p_AlphaEquation)
	{}
	void disable() { disabled = true; };
	void enable() { disabled = false; };
	GLenum srcRGB;
	GLenum dstRGB;
	GLenum srcAlpha;
	GLenum dstAlpha;
	GLenum RGBequation;
	GLenum AlphaEquation;
	// When blending is disabled, the source completely overwrites the destination.
	bool disabled = false;

};
#endif