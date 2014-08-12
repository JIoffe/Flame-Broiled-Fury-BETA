#pragma once
#include <Windows.h>
#include <gl\gl.h>                                // Header File For The OpenGL32 Library
#include <gl\glu.h>                               // Header File For The GLu32 Library
#pragma comment(lib, "Opengl32.lib")
#pragma comment(lib, "Glu32.lib")

#include "ScrollingComponent.h"
#include "TextureAtlasManager.h"
#include "LevelScene.h"
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

	GLuint Texture[3];
	
	GLfloat * pGridLinesVB; //This will hold the vertex data for the grid lines
	GLfloat * pSmallSpotGridVB;
	GLfloat * pMediumSpotGridVB;
	int GridPoints;
	int SmallPoints;
	int MediumPoints;

	ScrollingComponent * pScroller;
	TextureAtlasManager * pTexAtlas;
	TextureAtlasManager * pGlobalAtlas;

	void PushQuadToVertexBuffer(float Left, float Right, float Top, float Bottom, GLfloat * pVertexBuffer, int RenderCount);
	void PushColor(float R, float G, float B, float A, GLfloat * pColorBuffer, int RenderCount);
public:
	Renderer(void);
	~Renderer(void);

	void ResizeFrame(GLsizei w, GLsizei h);
	bool Initialize(const HWND hwnd, int w, int h);
	void Draw(LevelScene * pScene);
	void SetTexture(const char * cTexPath);
	void RebuildGrid(int Width, int Height);

	float GetRatio();
	float GetPixelWidth();
	void AssociateScroller(ScrollingComponent * pNewScroller);
	void AssociateTextureAtlas(TextureAtlasManager * pNewAtlas);
	void AssociateGlobalTextureAtlas(TextureAtlasManager * pNewAtlas);
};

