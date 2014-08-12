#include "Renderer.h"
#include "Globals.h"
#include "GDIImageLoader.h"


Renderer::Renderer(void)
{
	hDC = NULL;
	hRC = NULL;
	bReady = false;
}


Renderer::~Renderer(void)
{
}
bool Renderer::Initialize(const HWND hwnd, int w, int h){
	GDIImageLoader::Initialize();
	if(!(hDC = GetDC(hwnd))){
		return false;
	}
	PIXELFORMATDESCRIPTOR pfd;
	WORD uSize = sizeof(PIXELFORMATDESCRIPTOR);
	ZeroMemory(&pfd, uSize);
	pfd.nSize = uSize;
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL  | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.cColorBits = 32;

	const int PixelFormat = ChoosePixelFormat(hDC, &pfd);
	if(PixelFormat < 1 || !SetPixelFormat(hDC, PixelFormat, &pfd)){
		return false;
	}
	if (!(hRC=wglCreateContext(hDC)) || !wglMakeCurrent(hDC,hRC)){
		return false;
	}
	Width = w;
	Height = h;
	ResizeFrame(w, h);
	PixelWidth = (Ratio * 2) / Width;
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 


	//Set up some things that are good for a 2D sprite engine...
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glShadeModel(GL_FLAT);
    glDisable(GL_DITHER);
    glDisable(GL_LIGHTING);  // We definitely don't need lighting
    glDisable(GL_DEPTH_TEST);  // or Z-Buffer in 2D
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 


	SetTexture("lala");
	//Use Linear filtering

	glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	bReady = true;
	return true;
}
void Renderer::Draw(int SpriteID){
	if(bReady){
		GLfloat TexCoords[8];
		const GLfloat Vertices[] = { -Ratio, 1.0f, 0.0f,
							  -Ratio, -1.0f, 0.0f,
							  Ratio, -1.0f, 0.0f,
							  Ratio, 1.0f, 0.0f,};
		const int QuadStride = 12;
		const int QuadTexStride = 8;

		this->pTexAtlas->PushUVsToBuffer(TexCoords, 0, SpriteID);
		glClear(GL_COLOR_BUFFER_BIT);
		glVertexPointer(3, GL_FLOAT, 0, Vertices);
		glEnable(GL_TEXTURE_2D);
		glTexCoordPointer(2, GL_FLOAT, 0, TexCoords);
		glDrawArrays(GL_QUADS, 0, 4);
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0f, 1.0f, 1.0f);
		float CDLeft = Globals::PreviewCD.Left * Ratio;
		float CDRight = Globals::PreviewCD.Right * Ratio;
		const GLfloat CDVertices[] = { CDLeft, Globals::PreviewCD.Top, 0.0f,
			CDLeft, Globals::PreviewCD.Bottom, 0.0f,
			CDRight, Globals::PreviewCD.Bottom, 0.0f,
			CDRight, Globals::PreviewCD.Top, 0.0f,};
		glVertexPointer(3, GL_FLOAT, 0, CDVertices);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		SwapBuffers(hDC);
	}
	return;
}

void Renderer::ResizeFrame(const GLsizei w, const GLsizei h){
	glViewport(0,0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	Ratio = (float)w/h;
	gluOrtho2D(-Ratio, Ratio, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return;
}
void Renderer::SetTexture(const char * cTexPath){
	int Width, Height;
	//void * ImageData = GDIImageLoader::LoadImage(L"C:/Users/Malvina/workspace/FoofGame/res/drawable-hdpi/forestlevel.png", &Width, &Height);
	void * ImageData = GDIImageLoader::LoadImage(L"C:/Users/Malvina/workspace/FoofGame/res/drawable-hdpi/mechlevel.png", &Width, &Height);
	glGenTextures(1, &Texture); //Generate room for 1 texture. (We only need one...)
	if(ImageData){
		
		glBindTexture(GL_TEXTURE_2D, Texture); //Bind it to the pipeline. Once and for all.
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ImageData);
		delete[] ImageData;
		ImageData = NULL;
	}
	return;
}

float Renderer::GetRatio(){
	return Ratio;
}
void Renderer::AssociateTextureAtlas(TextureAtlasManager * pNewAtlas){
	pTexAtlas = pNewAtlas;
	return;
}