#pragma once
#include "Renderer.hpp"



class Renderer_GL : public IRenderer
{
private:
	SDL_GLContext context;
	SDL_Window* targetWindow;

public:
	Renderer_GL(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow);
	~Renderer_GL() override;

	//
	void Present() override;

	//
	tex_handle TextureFromStitchedDC6(const char* dc6Path, const char* handle, DWORD start, DWORD end, int palette) override;
	tex_handle TextureFromAnimatedDC6(const char* dc6Path, const char* handle, int palette) override;
	void DrawTexture(tex_handle texture, int x, int y, int w, int h, int u, int v) override;
	void DrawTextureFrames(tex_handle texture, int x, int y, DWORD startFrame, DWORD endFrame) override;
	void DrawTextureFrame(tex_handle texture, int x, int y, DWORD frame) override;
	void DeregisterTexture(const char* handleName, tex_handle texture) override;
	void SetTextureBlendMode(tex_handle texture, D2ColorBlending blendMode) override;
	void PollTexture(tex_handle texture, DWORD* width, DWORD* height) override;
	bool PixelPerfectDetect(anim_handle anim, int srcX, int srcY, int drawX, int drawY, bool bAllowAlpha) override;

	//
	anim_handle RegisterDC6Animation(tex_handle texture, const char* szHandlename, DWORD startingFrame) override;
	void DeregisterAnimation(anim_handle anim) override;
	void Animate(anim_handle anim, DWORD framerate, int x, int y) override;
	void SetAnimFrame(anim_handle anim, DWORD frame) override;
	DWORD GetAnimFrame(anim_handle anim) override;
	void AddAnimKeyframe(anim_handle anim, int frame, AnimKeyframeCallback callback, int extraInt) override;
	void RemoveAnimKeyframe(anim_handle anim) override;
	DWORD GetAnimFrameCount(anim_handle anim) override;

	//
	font_handle RegisterFont(const char* fontName) override;
	void DeregisterFont(font_handle font) override;
	void DrawText(font_handle font, const char16_t* text, int x, int y, int w, int h,
		D2TextAlignment alignHorz, D2TextAlignment alignVert) override;

	//
	void AlphaModTexture(tex_handle texture, int alpha) override;
	void ColorModTexture(tex_handle texture, int red, int green, int blue) override;
	void AlphaModFont(font_handle font, int alpha) override;
	void ColorModFont(font_handle font, int red, int green, int blue) override;

	//
	void DrawRectangle(int x, int y, int w, int h, int r, int g, int b, int a) override;

	void DrawTokenInstance(anim_handle instance, int x, int y, int translvl, int palette) override;

	void Clear() override;
};
