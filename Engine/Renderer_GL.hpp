#pragma once
#include "Renderer.hpp"
#include <GL/glew.h>

enum GLRenderPrograms
{
	RenderProgram_Diffuse,
	RenderProgram_FlatColor,
	RenderProgram_NumPrograms,
};

#define MAX_ANIMATION_CALLBACKS 8

/**
 *	Generic OpenGL render objects only contain an EBO and a VAO.
 *	VAOs hold our VBOs which have our vertices in them.
 */
class GLRenderObject : public IRenderObject
{
protected:
	BYTE palshift;
	float screenCoord[4]; // x y w h
	float textureCoord[4]; //x y w h (normalized)
	float colorModulate[4]; //r g b a (normalized)
	int drawMode;

	float drawBounds[4]; // temporary hack

	enum
	{
		RO_Static,
		RO_Animated,
		RO_Text,
		RO_Tile,
		RO_Token,
		RO_Primitive,
	} objectType;

	union
	{
		struct
		{
			uint64_t lastFrameTime;
			float currentFrame;
			uint16_t numFrames;
			uint32_t frameRate;
			class IGraphicsReference* attachedAnimationResource;
			bool bLoop;

			// Animation finish callbacks
			struct
			{
				bool bHaveRun;
				AnimationFrameCallback callback;
				void* extraData;
				int frame;
			} frameCallback[MAX_ANIMATION_CALLBACKS];
			int numFrameCallbacks;

			struct
			{
				bool bHaveRun;
				AnimationFinishCallback callback;
				void* extraData;
			} finishCallback[MAX_ANIMATION_CALLBACKS];
			int numFinishCallbacks;
		} animationData;

		struct
		{
			class IGraphicsReference* attachedFontResource;
			char16_t text[128];
		} textData;

		struct
		{
			class ITokenReference* attachedTokenResource;
			int currentMode;
			int hitClass;
			int direction;
			float currentFrame;
			uint64_t lastFrameTime;
			uint16_t frameRate;
			char armorType[COMP_MAX][4];
		} tokenData;

		struct
		{
			IGraphicsReference* tileReference;
			int tileIndex;
			int layer;
		} tileData;

		struct
		{
			enum 
			{
				PT_RECTANGLE,
				PT_LINE
			} type;

			float strokeWidth;
			float strokeColor[4];
			float fillColor[4];
			float position[4];
		} primitiveData;
	} data;

public:
	unsigned int texture;

	GLRenderPrograms renderProgram;

	GLRenderObject();
	~GLRenderObject();

	bool bInUse;

	virtual void Draw();
	virtual void PrerenderDrawMode(int drawMode);
	virtual void Render();	// Internal rendering function, used by the renderer itself.
	virtual void SetPalshift(BYTE palette);
	virtual void SetDrawCoords(int x, int y, int w, int h);
	virtual void GetDrawCoords(int* x, int* y, int* w, int* h);
	void SetWidthHeight(int w, int h);

	virtual void AttachTextureResource(class IGraphicsReference* handle, int32_t frame);
	virtual void AttachCompositeTextureResource(class IGraphicsReference* handle, int32_t startFrame, int32_t endFrame);
	virtual void AttachAnimationResource(class IGraphicsReference* handle, bool bResetFrame);
	virtual void AttachTokenResource(class ITokenReference* handle);
	virtual void AttachFontResource(class IGraphicsReference* handle);
	virtual void AttachTileResource(class IGraphicsReference* handle, int tileIndex, int layer);

	virtual void SetFramerate(int framerate);
	virtual void SetDrawMode(int drawMode);
	virtual void SetText(const char16_t* text);
	virtual void SetTextAlignment(int x, int y, int w, int h, int horzAlign, int vertAlign);
	virtual void SetTextColor(int color);
	virtual void SetColorModulate(float r, float g, float b, float a);

	virtual void SetAnimationLoop(bool bLoop);
	virtual void AddAnimationFinishedCallback(void* extraData, AnimationFinishCallback callback);
	virtual void AddAnimationFrameCallback(int32_t frame, void* extraData, AnimationFrameCallback callback);
	virtual void RemoveAnimationFinishCallbacks();

	virtual bool PixelPerfectDetection(int x, int y);

	virtual void SetAnimationDirection(int direction);
	virtual void SetTokenMode(int newMode);
	virtual void SetTokenArmorLevel(int component, const char* armorLevel);
	virtual void SetTokenHitClass(int hitclass);

	void SetAsRectanglePrimitive(float x, float y, float w, float h, float strokeWidth, float* strokeColor, float* fillColor);
	void SetAsLinePrimitive(float x1, float x2, float y1, float y2, float strokeWidth, float* strokeColor);
};

/**
 *	Memory pool
 */
class GLRenderPool
{
	void* MemoryPool;
	size_t remaining;
	size_t elementsRemaining;
	size_t initial;

	GLRenderPool* next;
	GLRenderObject* head;
public:
	GLRenderPool(size_t initialSize);
	~GLRenderPool();

	GLRenderObject* Allocate();
	void Deallocate(GLRenderObject* object);
};

/**
 * OpenGL renderer
 */
class Renderer_GL : public IRenderer
{
private:
	SDL_GLContext context;
	SDL_Window* targetWindow;

	static void LoadShaderText(const char* shaderText, unsigned int& shader, unsigned int shaderType);
	static void LoadProgram(const char* shaderFile, unsigned int& program, void(*fallbackIfNotFound)(unsigned int &program, const char *programFile));
	static void SaveProgram(unsigned int program, const char* shaderFile);
	static void LinkProgram(unsigned int program, const char* programName);
	static void InitShaders();
	static void DeleteShaders();

	GLRenderPool* pool;
	GLRenderPool* primitivePool;
	BYTE defaultPalshift[256];

	unsigned int VAO, VBO;
public:
	virtual void Present();
	virtual void Clear();
	virtual void SetGlobalPalette(const D2Palettes palette);
	virtual D2Palettes GetGlobalPalette();

	virtual IRenderObject* AllocateObject(int stage);
	virtual void Remove(IRenderObject* Object);

	Renderer_GL(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow);
	~Renderer_GL();

	void AddToRenderQueue(class GLRenderObject* Object);
	virtual void DeleteLoadedGraphicsData(void* loadedData, class IGraphicsReference* ref);

	virtual void DrawRectangle(float x, float y, float w, float h, float strokeWidth, float* strokeColor, float* fillColor);
	virtual void DrawLine(float x1, float x2, float y1, float y2, float strokeWidth, float* strokeColor);
};
