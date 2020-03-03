#pragma once
#include "Renderer.hpp"
#include <GL/glew.h>

enum GLRenderPasses
{
	RenderPass_UI,
//	RenderPass_Text,
	RenderPass_NumRenderPasses,
};

/**
 *	Generic OpenGL render objects only contain an EBO and a VAO.
 *	VAOs hold our VBOs which have our vertices in them.
 */
class GLRenderObject : public IRenderObject
{
protected:
	BYTE palshift;
	float screenCoord[4]; // x y w h
	float textureCoord[2];
	float textureSize[2];
	

public:
	unsigned int texture;

	GLRenderPasses renderPass;

	GLRenderObject();
	~GLRenderObject();

	bool bInUse;

	virtual void Draw();
	virtual void Render();	// Internal rendering function, used by the renderer itself.
	virtual void SetPalshift(BYTE palette);
	virtual void SetDrawCoords(int x, int y, int w, int h);
	virtual void SetTextureCoords(int x, int y, int w, int h);
	virtual void GetDrawCoords(int* x, int* y, int* w, int* h);
	void SetWidthHeight(int w, int h);
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
 *	Atlas
 */
class GLAtlas : public IAtlas
{
private:
	uint32_t numFiles;
	char** filesInAtlas;

public:
	GLAtlas();

	virtual bool WasPreCached();
	virtual void Finalize();
	virtual bool AddFile(const char* filePath);
	virtual uint32_t GetTotalAtlasElementCount();
	virtual void GetAtlasElementTexCoords(uint32_t index, uint32_t* x, uint32_t* y, uint32_t* w, uint32_t* h);
	virtual void GetAtlasSize(uint32_t* totalWidth, uint32_t* totalHeight, uint32_t* elementWidth, uint32_t* elementHeight);

	GLAtlas* next;
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
	BYTE defaultPalshift[256];
	GLAtlas* atlasChain;

	unsigned int VAO, VBO;
public:
	virtual void Present();
	virtual void Clear();
	virtual void SetGlobalPalette(const D2Palettes palette);

	virtual IRenderObject* AddStaticDC6(const char* dc6Path, DWORD start, DWORD end);
	virtual IAtlas* CreateOrLoadAtlas(const char* fileName);
	virtual void Remove(IRenderObject* Object);

	Renderer_GL(D2GameConfigStrc* pConfig, OpenD2ConfigStrc* pOpenConfig, SDL_Window* pWindow);
	~Renderer_GL();

	void AddToRenderQueue(class GLRenderObject* Object);
};
