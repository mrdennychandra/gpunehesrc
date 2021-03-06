#define _CRT_SECURE_NO_WARNINGS 1

#ifndef Lesson11_CLASS
#define Lesson11_CLASS

#include "baseapp.h"
#include <stdio.h>			// Header File For Standard Input/Output
#include <math.h>			// Header File For The Math Library
#include "bmp.h"

class Lesson11 {
public:
	float points[45][45][3];    // The Array For The Points On The Grid Of Our "Wave"
	int wiggle_count = 0;		// Counter Used To Control How Fast Flag Waves

	GLfloat	xrot;				// X Rotation ( NEW )
	GLfloat	yrot;				// Y Rotation ( NEW )
	GLfloat	zrot;				// Z Rotation ( NEW )
	GLfloat hold;				// Temporarily Holds A Floating Point Value

	GLuint	texture[1];			// Storage For One Texture ( NEW )

public:
	AUX_RGBImageRec *LoadBMP(char *Filename)				// Loads A Bitmap Image
	{
		FILE *File = NULL;									// File Handle

		if (!Filename)										// Make Sure A Filename Was Given
		{
			return NULL;									// If Not Return NULL
		}

		File = fopen(Filename, "r");							// Check To See If The File Exists

		if (File)											// Does The File Exist?
		{
			fclose(File);									// Close The Handle
			return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
		}

		return NULL;										// If Load Failed Return NULL
	}

	int LoadGLTextures()									// Load Bitmaps And Convert To Textures
	{
		int Status = FALSE;									// Status Indicator

		AUX_RGBImageRec *TextureImage[1];					// Create Storage Space For The Texture

		memset(TextureImage, 0, sizeof(void *) * 1);           	// Set The Pointer To NULL

																// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
		if (TextureImage[0] = LoadBMP("Data/Tim.bmp"))
		{
			Status = TRUE;									// Set The Status To TRUE

			glGenTextures(1, &texture[0]);					// Create The Texture

															// Typical Texture Generation Using Data From The Bitmap
			glBindTexture(GL_TEXTURE_2D, texture[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		if (TextureImage[0])									// If Texture Exists
		{
			if (TextureImage[0]->data)							// If Texture Image Exists
			{
				free(TextureImage[0]->data);					// Free The Texture Image Memory
			}

			free(TextureImage[0]);								// Free The Image Structure
		}

		return Status;										// Return The Status
	}

	int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
	{
		if (!LoadGLTextures())								// Jump To Texture Loading Routine ( NEW )
		{
			return FALSE;									// If Texture Didn't Load Return FALSE
		}

		glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
		glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
		glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
		glClearDepth(1.0f);									// Depth Buffer Setup
		glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
		glPolygonMode(GL_BACK, GL_FILL);					// Back Face Is Solid
		glPolygonMode(GL_FRONT, GL_LINE);					// Front Face Is Made Of Lines

		for (int x = 0; x<45; x++)
		{
			for (int y = 0; y<45; y++)
			{
				points[x][y][0] = float((x / 5.0f) - 4.5f);
				points[x][y][1] = float((y / 5.0f) - 4.5f);
				points[x][y][2] = float(sin((((x / 5.0f)*40.0f) / 360.0f)*3.141592654*2.0f));
			}
		}

		return TRUE;										// Initialization Went OK
	}

	int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
	{
		int x, y;
		float float_x, float_y, float_xb, float_yb;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
		glLoadIdentity();									// Reset The View

		glTranslatef(0.0f, 0.0f, -12.0f);

		glRotatef(xrot, 1.0f, 0.0f, 0.0f);
		glRotatef(yrot, 0.0f, 1.0f, 0.0f);
		glRotatef(zrot, 0.0f, 0.0f, 1.0f);

		glBindTexture(GL_TEXTURE_2D, texture[0]);

		glBegin(GL_QUADS);
		for (x = 0; x < 44; x++)
		{
			for (y = 0; y < 44; y++)
			{
				float_x = float(x) / 44.0f;
				float_y = float(y) / 44.0f;
				float_xb = float(x + 1) / 44.0f;
				float_yb = float(y + 1) / 44.0f;

				glTexCoord2f(float_x, float_y);
				glVertex3f(points[x][y][0], points[x][y][1], points[x][y][2]);

				glTexCoord2f(float_x, float_yb);
				glVertex3f(points[x][y + 1][0], points[x][y + 1][1], points[x][y + 1][2]);

				glTexCoord2f(float_xb, float_yb);
				glVertex3f(points[x + 1][y + 1][0], points[x + 1][y + 1][1], points[x + 1][y + 1][2]);

				glTexCoord2f(float_xb, float_y);
				glVertex3f(points[x + 1][y][0], points[x + 1][y][1], points[x + 1][y][2]);
			}
		}
		glEnd();

		if (wiggle_count == 2)
		{
			for (y = 0; y < 45; y++)
			{
				hold = points[0][y][2];
				for (x = 0; x < 44; x++)
				{
					points[x][y][2] = points[x + 1][y][2];
				}
				points[44][y][2] = hold;
			}
			wiggle_count = 0;
		}

		wiggle_count++;

		xrot += 0.3f;
		yrot += 0.2f;
		zrot += 0.4f;

		return TRUE;										// Keep Going
	}

	/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
	*	title			- Title To Appear At The Top Of The Window				*
	*	width			- Width Of The GL Window Or Fullscreen Mode				*
	*	height			- Height Of The GL Window Or Fullscreen Mode			*
	*	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
	*	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/

	BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
	{
		GLuint		PixelFormat;			// Holds The Results After Searching For A Match
		WNDCLASS	wc;						// Windows Class Structure
		DWORD		dwExStyle;				// Window Extended Style
		DWORD		dwStyle;				// Window Style
		RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
		WindowRect.left = (long)0;			// Set Left Value To 0
		WindowRect.right = (long)width;		// Set Right Value To Requested Width
		WindowRect.top = (long)0;				// Set Top Value To 0
		WindowRect.bottom = (long)height;		// Set Bottom Value To Requested Height

		fullscreen = fullscreenflag;			// Set The Global Fullscreen Flag

		hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
		wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
		wc.cbClsExtra = 0;									// No Extra Window Data
		wc.cbWndExtra = 0;									// No Extra Window Data
		wc.hInstance = hInstance;							// Set The Instance
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
		wc.hbrBackground = NULL;									// No Background Required For GL
		wc.lpszMenuName = NULL;									// We Don't Want A Menu
		wc.lpszClassName = "OpenGL";								// Set The Class Name

		if (!RegisterClass(&wc))									// Attempt To Register The Window Class
		{
			MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;											// Return FALSE
		}

		if (fullscreen)												// Attempt Fullscreen Mode?
		{
			DEVMODE dmScreenSettings;								// Device Mode
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
			dmScreenSettings.dmPelsWidth = width;				// Selected Screen Width
			dmScreenSettings.dmPelsHeight = height;				// Selected Screen Height
			dmScreenSettings.dmBitsPerPel = bits;					// Selected Bits Per Pixel
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
				if (MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				{
					fullscreen = FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
				}
				else
				{
					// Pop Up A Message Box Letting User Know The Program Is Closing.
					MessageBox(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP);
					return FALSE;									// Return FALSE
				}
			}
		}

		if (fullscreen)												// Are We Still In Fullscreen Mode?
		{
			dwExStyle = WS_EX_APPWINDOW;								// Window Extended Style
			dwStyle = WS_POPUP;										// Windows Style
			ShowCursor(FALSE);										// Hide Mouse Pointer
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
			dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style
		}

		AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

																		// Create The Window
		if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
			"OpenGL",							// Class Name
			title,								// Window Title
			dwStyle |							// Defined Window Style
			WS_CLIPSIBLINGS |					// Required Window Style
			WS_CLIPCHILDREN,					// Required Window Style
			0, 0,								// Window Position
			WindowRect.right - WindowRect.left,	// Calculate Window Width
			WindowRect.bottom - WindowRect.top,	// Calculate Window Height
			NULL,								// No Parent Window
			NULL,								// No Menu
			hInstance,							// Instance
			NULL)))								// Dont Pass Anything To WM_CREATE
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			bits,										// Select Our Color Depth
			0, 0, 0, 0, 0, 0,							// Color Bits Ignored
			0,											// No Alpha Buffer
			0,											// Shift Bit Ignored
			0,											// No Accumulation Buffer
			0, 0, 0, 0,									// Accumulation Bits Ignored
			16,											// 16Bit Z-Buffer (Depth Buffer)  
			0,											// No Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
		};

		if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		if (!(hRC = wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		if (!wglMakeCurrent(hDC, hRC))					// Try To Activate The Rendering Context
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		ShowWindow(hWnd, SW_SHOW);						// Show The Window
		SetForegroundWindow(hWnd);						// Slightly Higher Priority
		SetFocus(hWnd);									// Sets Keyboard Focus To The Window
		ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

		if (!InitGL())									// Initialize Our Newly Created GL Window
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}

		return TRUE;									// Success
	}

	int WINAPI WinMain(HINSTANCE	hInstance,			// Instance
		HINSTANCE	hPrevInstance,		// Previous Instance
		LPSTR		lpCmdLine,			// Command Line Parameters
		int			nCmdShow)			// Window Show State
	{
		MSG		msg;									// Windows Message Structure
		BOOL	done = FALSE;								// Bool Variable To Exit Loop

															// Ask The User Which Screen Mode They Prefer
		if (MessageBox(NULL, "Would You Like To Run In Fullscreen Mode?", "Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			fullscreen = FALSE;							// Windowed Mode
		}

		// Create Our OpenGL Window
		if (!CreateGLWindow("bosco & NeHe's Waving Texture Tutorial", 640, 480, 16, fullscreen))
		{
			return 0;									// Quit If Window Was Not Created
		}

		while (!done)									// Loop That Runs While done=FALSE
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting?
			{
				if (msg.message == WM_QUIT)				// Have We Received A Quit Message?
				{
					done = TRUE;							// If So done=TRUE
				}
				else									// If Not, Deal With Window Messages
				{
					TranslateMessage(&msg);				// Translate The Message
					DispatchMessage(&msg);				// Dispatch The Message
				}
			}
			else										// If There Are No Messages
			{
				// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
				if ((active && !DrawGLScene()) || keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
				{
					done = TRUE;							// ESC or DrawGLScene Signalled A Quit
				}
				else									// Not Time To Quit, Update Screen
				{
					SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
				}

				if (keys[VK_F1])						// Is F1 Being Pressed?
				{
					keys[VK_F1] = FALSE;					// If So Make Key FALSE
					KillGLWindow();						// Kill Our Current Window
					fullscreen = !fullscreen;				// Toggle Fullscreen / Windowed Mode
															// Recreate Our OpenGL Window
					if (!CreateGLWindow("bosco & NeHe's Waving Texture Tutorial", 640, 480, 16, fullscreen))
					{
						return 0;						// Quit If Window Was Not Created
					}
				}
			}
		}

		// Shutdown
		KillGLWindow();									// Kill The Window
		return (msg.wParam);							// Exit The Program
	}

};
#endif