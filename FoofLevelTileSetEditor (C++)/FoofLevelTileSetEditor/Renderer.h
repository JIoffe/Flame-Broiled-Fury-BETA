#pragma once
#include <Windows.h>
#include <gl\gl.h>                                // Header File For The OpenGL32 Library
#include <gl\glu.h>                               // Header File For The GLu32 Library
#pragma comment(lib, "Opengl32.lib")
#pragma comment(lib, "Glu32.lib")

#include "TextureAtlasManager.h"
class Renderer
{
	int Width;
	int Height;
	float Ratio;
	float PixelWidth;
	GLenum Shader_Program;
	GLenum SpriteVertexShader;
	GLenum SpritePixelShader;

	HDC hDC;  //Handle to DEVICE CONTEXT
	HGLRC hRC; //Handle to GL Rendering Context

	GLuint Texture;

	TextureAtlasManager * pTexAtlas;

	bool bReady;
public:
	Renderer(void);
	~Renderer(void);

	void ResizeFrame(GLsizei w, GLsizei h);
	bool Initialize(const HWND hwnd, int w, int h);
	void Draw(int SpriteID);
	void SetTexture(const char * cTexPath);

	float GetRatio();

	void AssociateTextureAtlas(TextureAtlasManager * pNewAtlas);
};

