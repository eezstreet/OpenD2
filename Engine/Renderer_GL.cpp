#include "Renderer_GL.hpp"
#include <GL/glew.h>
#include "DC6.hpp"
#include "Logging.hpp"

// windows...please...
#ifdef WIN32
#undef LoadImage
#endif // WIN32

Renderer_GL::Renderer_GL(D2GameConfigStrc * pConfig, OpenD2ConfigStrc * pOpenConfig, SDL_Window * pWindow)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	context = SDL_GL_CreateContext(pWindow);
	if (context == nullptr)
	{
		Log::Error("SDL_GL_CreateContext() failed: %s\n", SDL_GetError());
		return;
	}

	if (SDL_GL_MakeCurrent(pWindow, context))
	{
		Log::Error("SDL_GL_MakeCurrent() failed: %s\n", SDL_GetError());
		return;
	}

	GLenum result = glewInit();
	if (result != GLEW_OK)
	{
		Log::Error("glewInit() failed: %s\n", glewGetErrorString(result));
		return;
	}

	targetWindow = pWindow;
}

Renderer_GL::~Renderer_GL()
{
	if (context)
	{
		SDL_GL_DeleteContext(context);
	}
}

void Renderer_GL::Present()
{
	//clear color and depth buffer 
	glViewport(0, 0, 800, 600);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	SDL_GL_SwapWindow(targetWindow);
}

tex_handle Renderer_GL::TextureFromStitchedDC6(const char * dc6Path, const char * handle, DWORD start, DWORD end, int palette)
{
	DC6Image dc6;
	DWORD columns, rows;
	DWORD totalWidth, totalHeight;
	DC6::StitchStats(&dc6, start, end, &columns, &rows, &totalWidth, &totalHeight);
	return tex_handle();
}

tex_handle Renderer_GL::TextureFromAnimatedDC6(const char * dc6Path, const char * handle, int palette)
{
	return tex_handle();
}

void Renderer_GL::DrawTexture(tex_handle texture, int x, int y, int w, int h, int u, int v)
{
}

void Renderer_GL::DrawTextureFrames(tex_handle texture, int x, int y, DWORD startFrame, DWORD endFrame)
{
}

void Renderer_GL::DrawTextureFrame(tex_handle texture, int x, int y, DWORD frame)
{
}

void Renderer_GL::DeregisterTexture(const char * handleName, tex_handle texture)
{
}

void Renderer_GL::SetTextureBlendMode(tex_handle texture, D2ColorBlending blendMode)
{
}

void Renderer_GL::PollTexture(tex_handle texture, DWORD * width, DWORD * height)
{
}

bool Renderer_GL::PixelPerfectDetect(anim_handle anim, int srcX, int srcY, int drawX, int drawY, bool bAllowAlpha)
{
	return false;
}

anim_handle Renderer_GL::RegisterDC6Animation(tex_handle texture, const char * szHandlename, DWORD startingFrame)
{
	return anim_handle();
}

void Renderer_GL::DeregisterAnimation(anim_handle anim)
{
}

void Renderer_GL::Animate(anim_handle anim, DWORD framerate, int x, int y)
{
}

void Renderer_GL::SetAnimFrame(anim_handle anim, DWORD frame)
{
}

DWORD Renderer_GL::GetAnimFrame(anim_handle anim)
{
	return 0;
}

void Renderer_GL::AddAnimKeyframe(anim_handle anim, int frame, AnimKeyframeCallback callback, int extraInt)
{
}

void Renderer_GL::RemoveAnimKeyframe(anim_handle anim)
{
}

DWORD Renderer_GL::GetAnimFrameCount(anim_handle anim)
{
	return 0;
}

font_handle Renderer_GL::RegisterFont(const char * fontName)
{
	return font_handle();
}

void Renderer_GL::DeregisterFont(font_handle font)
{
}

// Windows is so silly.
#ifdef _WIN32
#undef DrawText
#endif // _WIN32
void Renderer_GL::DrawText(font_handle font, const char16_t * text, int x, int y, int w, int h, D2TextAlignment alignHorz, D2TextAlignment alignVert)
{
}

void Renderer_GL::AlphaModTexture(tex_handle texture, int alpha)
{
}

void Renderer_GL::ColorModTexture(tex_handle texture, int red, int green, int blue)
{
}

void Renderer_GL::AlphaModFont(font_handle font, int alpha)
{
}

void Renderer_GL::ColorModFont(font_handle font, int red, int green, int blue)
{
}

void Renderer_GL::DrawRectangle(int x, int y, int w, int h, int r, int g, int b, int a)
{
}

void Renderer_GL::DrawTokenInstance(anim_handle instance, int x, int y, int translvl, int palette)
{
}

void Renderer_GL::Clear()
{
}
