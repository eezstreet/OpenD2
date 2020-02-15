#pragma once
#include "Renderer.hpp"



class Renderer_GL : public IRenderer
{
private:
	SDL_GLContext context;
	SDL_Window* targetWindow;

public:
	Renderer_GL(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow);
	~Renderer_GL();

	//
	virtual void Present();

	//
	virtual tex_handle TextureFromStitchedDC6(const char* dc6Path, const char* handle, DWORD start, DWORD end, int palette);
	virtual tex_handle TextureFromAnimatedDC6(const char* dc6Path, const char* handle, int palette);
	virtual void DrawTexture(tex_handle texture, int x, int y, int w, int h, int u, int v);
	virtual void DrawTextureFrames(tex_handle texture, int x, int y, DWORD startFrame, DWORD endFrame);
	virtual void DrawTextureFrame(tex_handle texture, int x, int y, DWORD frame);
	virtual void DeregisterTexture(const char* handleName, tex_handle texture);
	virtual void SetTextureBlendMode(tex_handle texture, D2ColorBlending blendMode);
	virtual void PollTexture(tex_handle texture, DWORD* width, DWORD* height);
	virtual bool PixelPerfectDetect(anim_handle anim, int srcX, int srcY, int drawX, int drawY, bool bAllowAlpha);

	//
	virtual anim_handle RegisterDC6Animation(tex_handle texture, const char* szHandlename, DWORD startingFrame);
	virtual void DeregisterAnimation(anim_handle anim);
	virtual void Animate(anim_handle anim, DWORD framerate, int x, int y);
	virtual void SetAnimFrame(anim_handle anim, DWORD frame);
	virtual DWORD GetAnimFrame(anim_handle anim);
	virtual void AddAnimKeyframe(anim_handle anim, int frame, AnimKeyframeCallback callback, int extraInt);
	virtual void RemoveAnimKeyframe(anim_handle anim);
	virtual DWORD GetAnimFrameCount(anim_handle anim);

	//
	virtual font_handle RegisterFont(const char* fontName);
	virtual void DeregisterFont(font_handle font);
	virtual void DrawText(font_handle font, const char16_t* text, int x, int y, int w, int h,
		D2TextAlignment alignHorz, D2TextAlignment alignVert);

	//
	virtual void AlphaModTexture(tex_handle texture, int alpha);
	virtual void ColorModTexture(tex_handle texture, int red, int green, int blue);
	virtual void AlphaModFont(font_handle font, int alpha);
	virtual void ColorModFont(font_handle font, int red, int green, int blue);

	//
	virtual void DrawRectangle(int x, int y, int w, int h, int r, int g, int b, int a);

	virtual void DrawTokenInstance(anim_handle instance, int x, int y, int translvl, int palette);

	virtual void Clear();
};