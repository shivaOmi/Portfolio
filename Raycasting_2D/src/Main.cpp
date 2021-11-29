#include<Windows.h>
#include<GL/glew.h>
#include<GL/GL.h>
#include<stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<iostream>
#include<ctime>
#include"Ray.h"
#include"perlin.h"


constexpr auto FILE_OPEN_SUCCESS = 0;
constexpr auto WIN_WIDTH = 800;
constexpr auto WIN_HEIGHT = 800;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

FILE* gpFile = nullptr;

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC hglrc = NULL;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

glm::mat4 gOrthoProjectionMatrix(1);

GLuint gMVPLocation = 0;

DWORD dwStyle = NULL;
WINDOWPLACEMENT wpPrev = { sizeof(wpPrev) };

GLfloat lineSegments[1000];

GLuint CubeVbo;

GLfloat x_trans = 0.0f;
GLfloat y_trans = 0.0f;
GLfloat x_norm = 0.0f;
GLfloat y_norm = 0.0f;

GLfloat gWidth = 800.0f;
GLfloat gHeight = 800.0f;

GLfloat x1 = 0.9f;
GLfloat y_1 = 1.0f;
GLfloat x2 = 0.9f;
GLfloat y2 = -1.0f;

GLfloat x3 = 0.0f;
GLfloat y3 = 0.0f;

GLfloat x4 = 0.0f;
GLfloat y4 = 0.0f;
GLfloat den;
GLfloat t;
GLfloat u;

Boundary* walls[9];
Ray* rays[144];

float L0 = -0.5f;
float H0 = 0.5f;

double xoff = 1.0;
double yoff = 1000.0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	void initialize(void);
	void display(void);
	void update(void);
	void uninitialize_all_and_exit(void);

	WNDCLASSEX wndClass;
	HWND hwnd = NULL;
	MSG msg;
	TCHAR szClassName[] = TEXT("OGL_Template");
	bool bDone = false;

	// Code to print log in file
	if (fopen_s(&gpFile, "Log.txt", "w+") == FILE_OPEN_SUCCESS) {
		fprintf_s(gpFile, "File open successfully.\n");
	}
	else {
		MessageBox(NULL, TEXT("fopen_s() : Failed"), TEXT("Error"), MB_ICONERROR);
		exit(0);
	}

	// Code to show window at center of screen
	RECT rc;
	if (SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0) == FALSE) {
		fprintf_s(gpFile, "SystemParametersInfo() : Failed\n");
		fclose(gpFile);
		exit(0);
	}
	else {
		fprintf_s(gpFile, "SystemParametersInfo() : Success\n");
	}

	int screen_width = rc.right - rc.left;
	int screen_height = rc.bottom - rc.top;

	wndClass.cbSize = sizeof(wndClass);
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.lpszClassName = szClassName;
	wndClass.lpszMenuName = NULL;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);

	if (RegisterClassEx(&wndClass) == FALSE) {
		fprintf_s(gpFile, "RegisterClassEx() : Failed\n");
		fclose(gpFile);
		exit(0);
	}
	else {
		fprintf_s(gpFile, "RegisterClassEx() : Success\n");
	}

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szClassName, TEXT("Ray-Collision-Detector"), WS_OVERLAPPEDWINDOW, (screen_width / 2) - (WIN_WIDTH / 2), (screen_height / 2) - (WIN_HEIGHT / 2), WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, (LPVOID)0);

	if (hwnd == NULL) {
		fprintf_s(gpFile, "CreateWindowEx() : Failed\n");
		fclose(gpFile);
		exit(0);
	}
	else {
		fprintf_s(gpFile, "CreateWindowEx() : Success\n");
	}

	ghwnd = hwnd;

	initialize();

	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	ShowWindow(hwnd, SW_NORMAL);

	while (!bDone) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				bDone = true;
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			// Rendering loop
			if (gbActiveWindow) {
				if (gbEscapeKeyIsPressed) {
					bDone = true;
				}

				// call display() below this line
				display();
				//update();
			}
		}
	}


	uninitialize_all_and_exit();
	return((int)msg.wParam);
}

void CalculateClosest(float mouseX, float mouseY) {	
	// Old approach
	/*
	rays[0] = new Ray(vec3(mouseX, mouseY, 0.0f), vec3(1.0f, 0.0f, 0.0f));
	rays[1] = new Ray(vec3(mouseX, mouseY, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	rays[2] = new Ray(vec3(mouseX, mouseY, 0.0f), vec3(-1.0f, 0.0f, 0.0f));
	rays[3] = new Ray(vec3(mouseX, mouseY, 0.0f), vec3(0.0f, -1.0f, 0.0f));

	rays[4] = new Ray(vec3(mouseX, mouseY, 0.0f), vec3(1.0f, 1.0f, 0.0f));
	rays[5] = new Ray(vec3(mouseX, mouseY, 0.0f), vec3(-1.0f, 1.0f, 0.0f));
	rays[6] = new Ray(vec3(mouseX, mouseY, 0.0f), vec3(-1.0f, -1.0f, 0.0f));
	rays[7] = new Ray(vec3(mouseX, mouseY, 0.0f), vec3(1.0f, -1.0f, 0.0f));
	*/

	// New approach
	for (float angle = 1.0f, ray_index = 0; angle <= 360.0f; angle += 2.5f, ray_index++) {
		rays[(int)ray_index] = new Ray(vec3(mouseX, mouseY, 0.0f), vec3(glm::cos(angle), glm::sin(angle), 0.0f));
	}

	int i = 0;
	for (Ray *ray : rays) {
		vec3 closest;
		float minimum = 1000000.0f;
		for (Boundary* wall : walls) {
			vec3 pos = ray->cast(wall);
			if (pos.e[0] != 0.0f && pos.e[1] != 0.0f) {
				float d = ray->A.distance(pos);
				if (d < minimum) {
					minimum = d;
					closest = pos;
				}
			}
		}
		lineSegments[i++] = ray->A.e[0];
		lineSegments[i++] = ray->A.e[1];
		lineSegments[i++] = ray->A.e[2];

		lineSegments[i++] = closest.e[0];
		lineSegments[i++] = closest.e[1];
		lineSegments[i++] = closest.e[2];
	}

	for (Boundary* wall : walls) {
		lineSegments[i++] = wall->a[0];
		lineSegments[i++] = wall->a[1];
		lineSegments[i++] = wall->a[2];

		lineSegments[i++] = wall->b[0];
		lineSegments[i++] = wall->b[1];
		lineSegments[i++] = wall->b[2];
	}

	glNamedBufferSubData(CubeVbo, 0, sizeof(lineSegments), lineSegments);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	void MakeFullscreen(void);
	void resize(int, int);
	void LeaveFullscreen(void);
	POINT p;
	float tX;
	float tY;

	switch (iMsg) {
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0) {
			gbActiveWindow = true;
		}
		else {
			gbActiveWindow = false;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case 0x46: // 'F' or 'f'
			if (gbFullscreen == false) {
				MakeFullscreen();
				gbFullscreen = true;
			}
			else {
				LeaveFullscreen();
				gbFullscreen = false;
			}
			break;

		case 0x1B: // For'Esc'
			gbEscapeKeyIsPressed = true;
			break;
		}
		break;

	case WM_MOUSEMOVE:
		GetCursorPos(&p);

		ScreenToClient(ghwnd, &p);

		tX = (float)p.x;
		tY = (float)p.y;

		x_norm = 2.0f * (tX) / gWidth - 1.0f;
		y_norm = 2.0f * (tY) / gHeight - 1.0f;
		y_norm = -y_norm;

		CalculateClosest(x_norm, y_norm);
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void initialize() {
	void uninitialize_all_and_exit(void);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 24;

	int iPixelFormatIndex = 0;

	ghdc = GetDC(ghwnd);
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0) {
		fprintf_s(gpFile, "ChoosePixelFormat() : Failed\n");
		uninitialize_all_and_exit();
	}
	else {
		fprintf_s(gpFile, "ChoosePixelFormat() : Success\n");
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE) {
		fprintf_s(gpFile, "SetPixelFormat() : Failed\n");
		uninitialize_all_and_exit();
	}
	else {
		fprintf_s(gpFile, "SetPixelFormat() : Success\n");
	}

	hglrc = wglCreateContext(ghdc);
	if (hglrc == NULL) {
		fprintf_s(gpFile, "wglCreateContext() : Failed\n");
		uninitialize_all_and_exit();
	}
	else {
		fprintf_s(gpFile, "wglCreateContext() : Success\n");
	}

	if (wglMakeCurrent(ghdc, hglrc) == FALSE) {
		fprintf_s(gpFile, "wglMakeCurrent() : Failed\n");
		uninitialize_all_and_exit();
	}
	else {
		fprintf_s(gpFile, "wglMakeCurrent() : Success\n");
	}

	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK) {
		fprintf_s(gpFile, "glewInit() : Failed\n");
		uninitialize_all_and_exit();
	}
	else {
		fprintf_s(gpFile, "glewInit() : Success\n");
	}

	// write Vertex Shader here(Including compiling and linking)

	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar* szLogInfo = NULL;

	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	const GLchar* shaderSource =
		"#version 440\n" \
		"\n" \
		"layout(location=0) in vec4 cubeVertices;" \
		"layout(location=1) uniform mat4 MVP;"
		"void main(){" \
		"gl_Position = MVP * cubeVertices;" \
		"}";

	glShaderSource(gVertexShaderObject, 1, &shaderSource, NULL);

	glCompileShader(gVertexShaderObject);

	iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szLogInfo = NULL;
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szLogInfo = (char*)malloc(iInfoLogLength);
			if (szLogInfo != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szLogInfo);
				fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szLogInfo);
				free(szLogInfo);
				uninitialize_all_and_exit();
				exit(0);
			}
		}
	}

	// write Fragment Shader here(Including compiling and linking)
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* fragmentShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"out vec4 color;" \
		"void main(){" \
		"	color = vec4(1.0f,1.0f,1.0f,1.0f);" \
		"}";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
	glCompileShader(gFragmentShaderObject);

	iInfoLogLength = 0;
	iShaderCompileStatus = 0;
	szLogInfo = NULL;
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szLogInfo = (char*)malloc(iInfoLogLength);
			if (szLogInfo != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szLogInfo);
				fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szLogInfo);
				free(szLogInfo);
				uninitialize_all_and_exit();
				exit(0);
			}
		}
	}

	gShaderProgramObject = glCreateProgram();

	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	glLinkProgram(gShaderProgramObject);

	GLint iShaderProgramLinkStatus = 0;

	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);

	if (iShaderProgramLinkStatus == GL_FALSE) {
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);

		if (iInfoLogLength > 0) {
			szLogInfo = (GLchar*)malloc(iInfoLogLength);

			if (szLogInfo != NULL) {
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szLogInfo);
				fprintf_s(gpFile, "Shader Program Link Log: %s\n", szLogInfo);
				free(szLogInfo);
				uninitialize_all_and_exit();
			}
		}
	}

	glDetachShader(gShaderProgramObject, gVertexShaderObject);
	glDeleteShader(gVertexShaderObject);
	glDetachShader(gShaderProgramObject, gFragmentShaderObject);
	glDeleteShader(gFragmentShaderObject);

	GLuint vao;
	glCreateBuffers(1, &CubeVbo);
	glNamedBufferStorage(CubeVbo, sizeof(lineSegments), lineSegments, GL_DYNAMIC_STORAGE_BIT);
	//glNamedBufferSubData(CubeVbo, 0, sizeof(lineSegments), lineSegments);

	glCreateVertexArrays(1, &vao);

	glVertexArrayVertexBuffer(vao, 0, CubeVbo, 0, 3 * sizeof(GLfloat));
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao, 0, 0);

	glEnableVertexArrayAttrib(vao, 0);

	glBindVertexArray(vao);

	// Wall Borders
	walls[0] = new Boundary(1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f);
	walls[1] = new Boundary(-1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	walls[2] = new Boundary(-1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	walls[3] = new Boundary(1.0f, -1.0f, 0.0f, -1.0f, -1.0f, 0.0f);

	walls[4] = new Boundary(-0.3f, 0.7f, 0.0f, -0.7f, -0.3f, 0.0f);
	walls[5] = new Boundary(-0.15f, 0.55f, 0.0f, 0.3f, 0.72f, 0.0f);
	walls[6] = new Boundary(0.25f, 0.64f, 0.0f, 0.85f, 0.40f, 0.0f);
	walls[7] = new Boundary(-0.2f, -0.7f, 0.0f, 0.6f, -0.5f, 0.0f);
	walls[8] = new Boundary(0.55f, 0.1f, 0.0f, 0.42f, -0.5f, 0.0f);

	glLineWidth(1.0f);

	//setNoiseFrequency(10);
}

void MakeFullscreen(void) {
	MONITORINFO mi = { sizeof(mi) };

	dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
	if (dwStyle & WS_OVERLAPPEDWINDOW) {
		if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
			SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
		}
	}
	//ShowCursor(FALSE);
}

void resize(int width, int height) {
	if (height == 0)
		height = 1;

	gWidth = (float)width;
	gHeight = (float)height;

	glViewport(0, 0, width, height);

	//gPerspectiveProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	gOrthoProjectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 100.0f);
}

void update(void) {
	CalculateClosest(float(noise1(xoff)*3), float(noise1(yoff)*3));
	xoff += 0.005;
	yoff += 0.005;
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);
	glm::mat4 modelViewMatrix = glm::mat4(1);
	glm::mat4 cameraMatrix = glm::mat4(1);
	glm::mat4 modelViewProjectionMatrix = glm::mat4(1);

	modelViewMatrix = glm::translate(modelViewMatrix, glm::vec3(0.0f, 0.0f, -5.0f));

	modelViewProjectionMatrix = gOrthoProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
	glDrawArrays(GL_LINES, 0, sizeof(lineSegments) / sizeof(lineSegments[0]));
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void LeaveFullscreen(void) {
	SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
	SetWindowPlacement(ghwnd, &wpPrev);
	SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
	ShowCursor(TRUE);
}

void uninitialize_all_and_exit() {
	if (gbFullscreen) {
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	if (hglrc) {
		wglDeleteContext(hglrc);
		wglMakeCurrent(NULL, NULL);
		hglrc = NULL;
	}

	if (ghdc) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (ghwnd) {
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	if (gpFile) {
		fprintf_s(gpFile, "File close successfully.\n");
		fclose(gpFile);
		gpFile = nullptr;
	}
}
