#include "Renderer_GL.hpp"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include "FileSystem.hpp"
#include "GraphicsManager.hpp"
#include "Logging.hpp"
#include "Palette.hpp"
#include "DC6.hpp"

static size_t CurrentPoolSize[RenderPass_NumRenderPasses];
static size_t NumberDrawnThisFrame[RenderPass_NumRenderPasses];
static GLRenderObject*** DrawList;
static GLRenderObject*** NewDrawList;
static size_t DrawListSize[RenderPass_NumRenderPasses];

// UI phase
static GLint uniform_ui_texture;
static GLint uniform_ui_globalpal;
static GLint uniform_ui_modelViewProjection;
static GLint uniform_ui_globalPaletteNum;
static GLint uniform_ui_drawPosition;
static GLint uniform_ui_textureCoords;
static GLint uniform_ui_colorModulate;

static unsigned int global_palette = 0;

static unsigned int global_program[RenderPass_NumRenderPasses];
static unsigned int global_palette_texture;
static unsigned int global_palshift_textures[256];

static GLfloat defaultQuadVertices[] = {
	// Pos      // Tex
	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 0.0f
};

/**
 * GLRenderObjects are things that are rendered onscreen.
 */
GLRenderObject::GLRenderObject()
{
	bInUse = true;
	memset(&data, 0, sizeof(data));
	objectType = RO_Static;
	drawMode = 0;

	textureCoord[0] = 0.0f;
	textureCoord[1] = 0.0f;
	textureCoord[2] = 1.0f;
	textureCoord[3] = 1.0f;

	colorModulate[0] = colorModulate[1] = colorModulate[2] = colorModulate[3] = 1.0f;
}

GLRenderObject::~GLRenderObject()
{
	bInUse = false;
}

void GLRenderObject::Draw()
{
	Renderer_GL* glRenderer = dynamic_cast<Renderer_GL*>(RenderTarget);
	glRenderer->AddToRenderQueue(this);
}

void GLRenderObject::Render()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, global_palette_texture);
	glUniform1i(uniform_ui_texture, 0);
	glUniform1i(uniform_ui_globalpal, 1);
	glUniform1i(uniform_ui_globalPaletteNum, global_palette);
	glUniform4fv(uniform_ui_colorModulate, 1, colorModulate);

	switch (drawMode)
	{
		case 0:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case 1:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case 2:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case 3:
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
			break;
		case 4:
			glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
			break;
		default:
		case 5:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case 6:
			glBlendFunc(GL_SRC_COLOR, GL_ONE);
			break;
		case 7:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
	}

	if (objectType == RO_Animated)
	{
		// update animation frame and set texture coordinates, if appropriate
		uint64_t ticks = SDL_GetTicks();
		uint16_t lastFrame = data.animationData.currentFrame;
		
		data.animationData.currentFrame += (ticks - data.animationData.lastFrameTime) / (1000 / data.animationData.frameRate);
		data.animationData.currentFrame %= data.animationData.numFrames;
		if (lastFrame != data.animationData.currentFrame)
		{
			// the animation changed frames, as such, we need to define new texture coordinates
			data.animationData.lastFrameTime = ticks;
		}

		uint32_t frameWidth, frameHeight;
		int32_t offsetX, offsetY;
		data.animationData.attachedAnimationResource->GetGraphicsData(nullptr, data.animationData.currentFrame,
			&frameWidth, &frameHeight, &offsetX, &offsetY);

		GLfloat drawCoords[] = { screenCoord[0] + offsetX,
			screenCoord[1] + 400 - frameHeight + offsetY, // offset is supposed to be additive downwards, perhaps..?
			frameWidth,
			frameHeight - 1
		};

		uint32_t x, y, w, h;
		data.animationData.attachedAnimationResource->GetAtlasInfo(data.animationData.currentFrame, &x, &y, &w, &h);
		textureCoord[0] = (x) / (float)w;
		textureCoord[1] = (y) / (float)h;
		textureCoord[2] = frameWidth / (float)w;
		textureCoord[3] = frameHeight / (float)h;

		glUniform4fv(uniform_ui_drawPosition, 1, drawCoords);
	}
	else if(objectType == RO_Text)
	{
		char16_t* p = data.textData.text;
		IGraphicsHandle* fontResource = data.textData.attachedFontResource;
		uint32_t glyphWidth, glyphHeight;
		int32_t glyphOffsetX, glyphOffsetY;
		uint32_t currentDrawX = screenCoord[0];

		uint32_t w, h;
		fontResource->GetGraphicsInfo(true, 0, 255, &w, &h);

		GLfloat drawCoord[] = {
			screenCoord[0], screenCoord[1], 0, 0
		};

		while(p && *p)
		{	// draw each individual letter
			fontResource->GetGraphicsData(nullptr, *p, &glyphWidth, &glyphHeight, &glyphOffsetX, &glyphOffsetY);
			
			textureCoord[0] = glyphOffsetX / (float)w;
			textureCoord[1] = (glyphOffsetY) / (float)h;
			textureCoord[2] = (glyphWidth) / (float)w;
			textureCoord[3] = (glyphHeight) / (float)h;
				
			drawCoord[0] = currentDrawX;
			drawCoord[2] = glyphWidth;
			drawCoord[3] = glyphHeight;

			glUniform4fv(uniform_ui_textureCoords, 1, textureCoord);
			glUniform4fv(uniform_ui_drawPosition, 1, drawCoord);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			currentDrawX += glyphWidth;
			p++;
		}
		return;	
	}
	else
	{
		glUniform4fv(uniform_ui_drawPosition, 1, screenCoord);
	}


	glUniform4fv(uniform_ui_textureCoords, 1, textureCoord);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void GLRenderObject::AttachTextureResource(IGraphicsHandle* handle, int32_t frame)
{
	if(frame < 0)
	{
		frame = 0;
	}

	if(!handle)
	{
		return;
	}

	objectType = RO_Static;

	if(handle->AreGraphicsLoaded())
	{
		texture = (unsigned int)handle->GetLoadedGraphicsData();
	}
	else
	{
		void* pixels;
		uint32_t frameW, frameH;

		handle->GetGraphicsData(&pixels, frame, &frameW, &frameH, nullptr, nullptr);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, frameW, frameH, 0, GL_RED,
				GL_UNSIGNED_BYTE, pixels);

		handle->SetLoadedGraphicsData((void*)texture);
	}
}

void GLRenderObject::AttachCompositeTextureResource(IGraphicsHandle* handle,
		int32_t startFrame, int32_t endFrame)
{
	uint32_t width, height;

	if(!handle)
	{
		return;
	}

	objectType = RO_Static;

	if(startFrame < 0)
	{
		startFrame = 0;
	}

	if(startFrame > endFrame && endFrame != -1)
	{
		int32_t swap;
		swap = startFrame;
		startFrame = endFrame;
		endFrame = swap;
	}

	if(startFrame == 0 && endFrame == -1 && handle->AreGraphicsLoaded())
	{
		texture = (unsigned int)handle->GetLoadedGraphicsData();
	}
	else
	{
		handle->GetGraphicsInfo(false, startFrame, endFrame, &width, &height);
		screenCoord[2] = width;
		screenCoord[3] = height;

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
				GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		handle->IterateFrames(false, startFrame, endFrame,
			[](void* pixels, int32_t frameNum, int32_t frameX, int32_t frameY, int32_t frameW, int32_t frameH) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, frameX, frameY, frameW, frameH, GL_RED,
						GL_UNSIGNED_BYTE, pixels);
			}
		);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		if(startFrame == 0 && endFrame == -1)
		{
			handle->SetLoadedGraphicsData((void*)texture);
		}
	}
}

void GLRenderObject::AttachPaletteResource(IGraphicsHandle* handle)
{
}

void GLRenderObject::AttachAnimationResource(IGraphicsHandle* handle)
{
	if (!handle)
	{
		return;
	}

	// turn on animation
	objectType = RO_Animated;
	data.animationData.numFrames = handle->GetNumberOfFrames();
	data.animationData.currentFrame = 0;
	data.animationData.lastFrameTime = SDL_GetTicks();
	data.animationData.frameRate = 25;
	data.animationData.attachedAnimationResource = handle;

	if(handle->AreGraphicsLoaded())
	{
		texture = (unsigned int)handle->GetLoadedGraphicsData();
	}
	else
	{
		// make texture atlas
		uint32_t width, height;
		handle->GetGraphicsInfo(true, 0, -1, &width, &height);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
			GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		handle->IterateFrames(true, 0, -1, 
			[](void* pixels, int32_t frameNum, int32_t frameX, int32_t frameY, int32_t frameW, int32_t frameH) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, frameX, frameY, frameW, frameH, GL_RED,
					GL_UNSIGNED_BYTE, pixels);
				
			}
		);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		handle->SetLoadedGraphicsData((void*)texture);
	}
}

void GLRenderObject::AttachTokenResource(IGraphicsHandle* handle)
{
}

void GLRenderObject::AttachFontResource(IGraphicsHandle* handle)
{
	if (!handle)
	{
		return;
	}

	objectType = RO_Text;
	data.textData.attachedFontResource = handle;
	data.textData.text[0] = '\0';

	if(handle->AreGraphicsLoaded())
	{
		texture = (unsigned int)handle->GetLoadedGraphicsData();
	}
	else
	{
		// make texture atlas
		uint32_t width, height;
		handle->GetGraphicsInfo(true, 0, -1, &width, &height);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
			GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		handle->IterateFrames(true, 0, -1,
			[](void* pixels, int32_t frameNum, int32_t frameX, int32_t frameY, int32_t frameW, int32_t frameH) {
			glTexSubImage2D(GL_TEXTURE_2D, 0, frameX, frameY, frameW, frameH, GL_RED,
				GL_UNSIGNED_BYTE, pixels);

		}
		);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		handle->SetLoadedGraphicsData((void*)texture);
	}
}

void GLRenderObject::SetPalshift(BYTE palette)
{
	
}

void GLRenderObject::SetDrawCoords(int x, int y, int w, int h)
{
	screenCoord[0] = x;
	screenCoord[1] = y;
	if (w >= 0)
	{
		screenCoord[2] = w;
	}
		
	if (h >= 0)
	{
		screenCoord[3] = h;
	}
}

void GLRenderObject::GetDrawCoords(int* x, int* y, int* w, int* h)
{
	if (x)
	{
		*x = screenCoord[0];
	}
		
	if (y)
	{
		*y = screenCoord[1];
	}
		
	if (w)
	{
		*w = screenCoord[2];
	}
		
	if (h)
	{
		*h = screenCoord[3];
	}
}

void GLRenderObject::SetWidthHeight(int w, int h)
{
	screenCoord[2] = w;
	screenCoord[3] = h;
}

void GLRenderObject::SetFramerate(int framerate)
{
	data.animationData.frameRate = framerate;
}

void GLRenderObject::SetDrawMode(int _drawMode)
{
	drawMode = _drawMode;
}

void GLRenderObject::SetText(const char16_t* text)
{
	size_t s = 0;
	char16_t* p = (char16_t*)text;
	uint32_t width = 0;
	uint32_t glyphWidth, glyphHeight;
	uint32_t height = 0;
	IGraphicsHandle* fontHandle = data.textData.attachedFontResource;

	while (*p && s < 128)
	{
		fontHandle->GetGraphicsData(nullptr, *p, &glyphWidth, &glyphHeight, nullptr, nullptr);
		if (height < glyphHeight)
		{
			height = glyphHeight;
		}
		width += glyphWidth;
		data.textData.text[s++] = *p;
		p++;
	}

	if (s < 128)
	{
		data.textData.text[p - text] = '\0';
	}
	else
	{
		data.textData.text[127] = '\0';
	}

	screenCoord[2] = width;
	screenCoord[3] = height;
}

void GLRenderObject::SetColorModulate(float r, float g, float b, float a)
{
	colorModulate[0] = r;
	colorModulate[1] = g;
	colorModulate[2] = b;
	colorModulate[3] = a;
}

/**
 *	The GLRenderPool is responsible for managing the lifetime of the GLRenderObjects.
 */
GLRenderPool::GLRenderPool(size_t initialSize)
{
	remaining = initialSize;
	initial = initialSize;
	elementsRemaining = initialSize / sizeof(GLRenderObject);
	next = nullptr;
	MemoryPool = malloc(initialSize);
	if (MemoryPool != nullptr)
	{
		// crash if nullptr?
		memset(MemoryPool, 0, sizeof(initialSize));
		head = (GLRenderObject*)MemoryPool;
	}
}

GLRenderPool::~GLRenderPool()
{
	if (next)
	{
		delete next;
	}

	free(MemoryPool);
}

GLRenderObject* GLRenderPool::Allocate()
{
	if (elementsRemaining <= 0)
	{
		if (!next)
		{
			next = new GLRenderPool(initial);
		}
		return next->Allocate();
	}

	elementsRemaining--;
	remaining -= sizeof(GLRenderObject);

	GLRenderObject* newObject = new (head) GLRenderObject();
	head++;
	if ((unsigned char*)head - (unsigned char*)MemoryPool >= initial)
	{
		head = (GLRenderObject*)MemoryPool;
		if (elementsRemaining > 0)
		{	// find next free
			while ((unsigned char*)head - (unsigned char*)MemoryPool < initial)
			{
				if (head->bInUse)
				{
					head++;
				}
				else
				{
					break;
				}
			}
		}
	}

	return newObject;
}

void GLRenderPool::Deallocate(GLRenderObject* object)
{
	unsigned char* location = (unsigned char*)object;
	unsigned char* poolLocation = (unsigned char*)MemoryPool;

	if (location - poolLocation < 0 || location - poolLocation > initial)
	{
		if (next)
		{
			next->Deallocate(object);
		}
	}
	else
	{
		object->~GLRenderObject();
		head = object;
	}
}

/**
 *	Renderer_GL does all the rendering.
 */
void Renderer_GL::SetGlobalPalette(const D2Palettes palette)
{
	global_palette = palette;
}

D2Palettes Renderer_GL::GetGlobalPalette()
{
	return (D2Palettes)global_palette;
}

IRenderObject* Renderer_GL::AllocateObject(int stage)
{
	GLRenderObject* allocated = pool->Allocate();
	allocated->renderPass = RenderPass_UI;
	return allocated;
}

void Renderer_GL::Remove(IRenderObject* Object)
{
	pool->Deallocate((GLRenderObject*)Object);
}

Renderer_GL::Renderer_GL(D2GameConfigStrc * pConfig, OpenD2ConfigStrc * pOpenConfig, SDL_Window * pWindow)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	DrawList = new GLRenderObject**[RenderPass_NumRenderPasses];
	for (int i = 0; i < RenderPass_NumRenderPasses; i++)
	{
		DrawListSize[i] = 2048;
		CurrentPoolSize[i] = sizeof(GLRenderObject) * DrawListSize[i];
		DrawList[i] = new GLRenderObject*[DrawListSize[i]];
		NumberDrawnThisFrame[i] = 0;
	}

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
	InitShaders();

	pool = new GLRenderPool(sizeof(GLRenderObject) * 2048);

	for (int i = 0; i < 256; i++)
	{
		defaultPalshift[i] = i;
	}
	glGenTextures(256, global_palshift_textures);
	glBindTexture(GL_TEXTURE_2D, global_palshift_textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 256, 1, 0, GL_RED, GL_UNSIGNED_BYTE, defaultPalshift);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glGenTextures(1, &global_palette_texture);
	glBindTexture(GL_TEXTURE_2D, global_palette_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, PAL_MAX_PALETTES, 0, GL_BGR, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	for (int i = 0; i < PAL_MAX_PALETTES; i++)
	{
		BYTE rgba[] = { 0, 0, 0, 0 };

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, 256, 1, GL_BGR, GL_UNSIGNED_BYTE, Pal::GetPalette(i));
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(defaultQuadVertices), defaultQuadVertices, GL_DYNAMIC_DRAW);

	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
}

void Renderer_GL::LoadShaderText(const char* shaderText, unsigned int& shader, unsigned int shaderType)
{
	char shaderLog[1024];
	GLint status;

	// Create the shader.
	shader = glCreateShader(shaderType);

	glShaderSource(shader, 1, &shaderText, nullptr);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		glGetShaderInfoLog(shader, sizeof(shaderLog), nullptr, shaderLog);
		Log::Error("Error compiling shader: %s\n", shaderLog);
	}
}

void Renderer_GL::LoadProgram(const char* shaderFile, unsigned int& program, void(*fallbackIfNotFound)(unsigned int &program, const char *programFile))
{
	GLint numFormats, desiredFormat, status;
	GLint* formats;
	program = glCreateProgram();

	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);
	if (numFormats < 1)
	{	// no supported formats = fail
		fallbackIfNotFound(program, shaderFile);
		return;
	}

	// Try loading the program from disk
	fs_handle f;
	size_t programSize = FS::Open(shaderFile, &f, FS_READ, true);

	if (!f || !programSize)
	{	// Program file was not found
		if (!programSize)
		{
			FS::CloseFile(f);
		}
		fallbackIfNotFound(program, shaderFile);
		return;
	}

	formats = (GLint*)malloc(sizeof(GLint) * numFormats);
	glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, formats);
	desiredFormat = formats[0]; // just use the first available format
	free(formats);

	void* programMemory = malloc(programSize);
	glProgramBinary(program, desiredFormat, programMemory, programSize);
	free(programMemory);

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		Log::Warning("Failed to load precompiled shader %s\n", shaderFile);
		fallbackIfNotFound(program, shaderFile);
	}
}

void Renderer_GL::SaveProgram(unsigned int program, const char* shaderFile)
{
	GLint numFormats, desiredFormat, binaryLength;
	GLint* formats;

	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);
	if (numFormats < 1)
	{
		return; // not useful
	}

	// get list of available formats
	formats = (GLint*)malloc(sizeof(GLint) * numFormats);
	glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, formats);
	desiredFormat = formats[0]; // just use the first available format
	free(formats);

	// Try opening the file
	fs_handle f;
	FS::Open(shaderFile, &f, FS_WRITE, true);

	// Figure out how big of a binary blob the program is
	glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &binaryLength);

	// allocate memory, fill with program binary from GPU, then write it and free the allocated memory
	void* buffer = malloc(binaryLength);
	glGetProgramBinary(program, binaryLength, nullptr, (GLenum*)&desiredFormat, buffer);
	FS::Write(f, buffer, binaryLength);
	free(buffer);

	// Close the file and we're done!
	FS::CloseFile(f);
}

void Renderer_GL::LinkProgram(unsigned int program, const char* programName)
{
	GLint compileStatus;

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE)
	{
		char errorBuffer[1024];
		glGetProgramInfoLog(program, sizeof(errorBuffer), nullptr, errorBuffer);

		Log::Warning("Failed to link program %s: %s\n", programName, errorBuffer);
	}
}

// TODO: Build the shaders.
// Static DC6 takes the following parameters for each vertex:
// location = 0; in vec2 - render location (x, y)
// location = 1; in vec2 - texture coordinate
// location = 2; in sampler2D - palshift
// There are also the following uniforms:
// uniform byte globalPalette; -- this is the game's global palette
const char* __staticDC6_Vertex = "#version 330 core                            \n\
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>       \n\
uniform mat4 ModelViewProjection;                                              \n\
uniform vec4 DrawPosition;                                                     \n\
uniform vec4 TexPosition;                                                      \n\
out vec2 TexCoords;                                                            \n\
                                                                               \n\
void main()                                                                    \n\
{                                                                              \n\
	vec2 NewPos = vec2((vertex.x * DrawPosition.z) + DrawPosition.x, (vertex.y * DrawPosition.w) + DrawPosition.y);                                 \n\
	TexCoords = vec2(TexPosition.x + (vertex.z * TexPosition.z), TexPosition.y + (vertex.w * TexPosition.w));                                                     \n\
	gl_Position = ModelViewProjection * vec4(NewPos.xy, 0.0, 1.0);             \n\
}                                                                              \
";

const char* __staticDC6_Fragment = "#version 330 core                          \n\
uniform sampler2D Texture;                                                     \n\
uniform sampler2D GlobalPalette;                                               \n\
uniform vec4 ColorModulate;                                                    \n\
uniform int GlobalPaletteNum;                                                 \n\
in vec2 TexCoords;                                                           \n\
                                                                               \n\
out vec4 outputColor;                                                          \n\
                                                                               \n\
void main()                                                                    \n\
{                                                                              \n\
	vec4 BaseTexture = texture2D(Texture, TexCoords);                           \n\
	vec4 FinalColor = texture2D(GlobalPalette, vec2(BaseTexture.r, GlobalPaletteNum / 17.0)); \n\
   outputColor = FinalColor;                                                  \n\
    outputColor *= ColorModulate;                                             \n\
}                                                                              \
";

void Renderer_GL::InitShaders()
{
	// Load the program.
	// The lambda function gets called when the program hasn't been previously compiled.
	LoadProgram("pass_ui.gl", global_program[RenderPass_UI], [](unsigned int& program, const char* programFile) {
		unsigned int vertexShader, fragmentShader;

		glm::mat4 projMat = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
		glm::mat4 mvp = glm::translate(projMat, glm::vec3(0.375f, 0.375f, 0.0f));

		LoadShaderText(__staticDC6_Vertex, vertexShader, GL_VERTEX_SHADER);
		LoadShaderText(__staticDC6_Fragment, fragmentShader, GL_FRAGMENT_SHADER);

		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		LinkProgram(program, "shadercache");

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		SaveProgram(program, programFile);

		uniform_ui_globalpal = glGetUniformLocation(program, "GlobalPalette");
		uniform_ui_texture = glGetUniformLocation(program, "Texture");
		uniform_ui_globalPaletteNum = glGetUniformLocation(program, "GlobalPaletteNum");
		uniform_ui_drawPosition = glGetUniformLocation(program, "DrawPosition");
		uniform_ui_textureCoords = glGetUniformLocation(program, "TexPosition");
		uniform_ui_colorModulate = glGetUniformLocation(program, "ColorModulate");
		glUseProgram(program);
		glUniformMatrix4fv(glGetUniformLocation(program, "ModelViewProjection"), 1, false, glm::value_ptr(mvp));
	});
}

void Renderer_GL::DeleteShaders()
{
	glDeleteProgram(global_program[RenderPass_UI]);
}

Renderer_GL::~Renderer_GL()
{
	// Delete all of the render objects' shaders
	DeleteShaders();

	// Free the memory pool
	delete pool;
	delete[] DrawList;

	// Destroy the context
	if (context)
	{
		SDL_GL_DeleteContext(context);
	}
}

void Renderer_GL::AddToRenderQueue(class GLRenderObject* Object)
{
	if (NumberDrawnThisFrame[Object->renderPass] < DrawListSize[Object->renderPass])
	{
		DrawList[Object->renderPass][NumberDrawnThisFrame[Object->renderPass]++] = Object;
	}
	else
	{
		DrawListSize[Object->renderPass] *= 2;
		NewDrawList[Object->renderPass] = new GLRenderObject*[DrawListSize[Object->renderPass]];
		for (int i = 0; i < DrawListSize[Object->renderPass] / 2; i++)
		{
			NewDrawList[Object->renderPass][i] = DrawList[Object->renderPass][i];
		}
		NewDrawList[Object->renderPass][NumberDrawnThisFrame[Object->renderPass]++] = Object;
		delete[] DrawList[Object->renderPass];
		DrawList[Object->renderPass] = NewDrawList[Object->renderPass];
	}
}

void Renderer_GL::Present()
{
	//clear color and depth buffer 
	glViewport(0, 0, 800, 600);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Iterate through the number of drawn things this frame
	for (int i = 0; i < RenderPass_NumRenderPasses; i++)
	{
		glUseProgram(global_program[i]);

		for (size_t j = 0; j < NumberDrawnThisFrame[i]; j++)
		{
			if (DrawList[i][j]->bInUse)
			{
				DrawList[i][j]->Render();
			}
		}
		NumberDrawnThisFrame[i] = 0;
	}

	SDL_GL_SwapWindow(targetWindow);
}

void Renderer_GL::Clear()
{
	for (int i = 0; i < RenderPass_NumRenderPasses; i++)
	{
		NumberDrawnThisFrame[i] = 0;
	}
}
