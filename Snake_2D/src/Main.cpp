#include<Windows.h>
#include<gl/GL.h>
#include<stdio.h>
#include<time.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 800

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"Winmm.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

typedef struct List {
	GLfloat x1, y1, x2, y2, x3, y3, x4, y4;
	struct List* next;
}Node;

Node* head = NULL, * Iterate = NULL, * Prev = NULL;
GLfloat initX1 = 400.0f, initY1 = 100.0f, initX2 = 410.0f, initY2 = 100.0f, randomPos = 0.0f;
GLfloat initX3 = 410.0f, initY3 = 110.0f, initX4 = 400.0f, initY4 = 110.0f, b = 0.0f, frameIncreaseCnt = 1.0f, frames = 0.0f;
GLint snakeLength = 10, x = 10, y = 10;

HWND ghwnd;
HDC ghdc;
HGLRC hglrc;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(wpPrev) };
bool gbFullScreen = false, gbActiveWindow = false, gbEscapeKeyIsPressed = false, gbUpKeyIsPressed = true;
bool gbRightKeyIsPressed = false, gbLeftKeyIsPressed = false, gbDownKeyIsPressed = false, coloredHead = true;
GLint igWidth = 0, igHeight = 0;

FILE* gpFile;

int grid[80][80] = { 0 };

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	void Initialize(void);
	void Move(void);
	void Display(void);
	void Uninitialize(void);

	WNDCLASSEX WndClass;
	HWND hwnd;
	MSG msg;
	TCHAR szWndClsName[] = TEXT("Snake Game");
	int iScreenWidth = 0, iScreenHeight = 0, iCenter_X = CW_USEDEFAULT, iCenter_Y = CW_USEDEFAULT;
	RECT rc;
	bool bDone = false;

	if (fopen_s(&gpFile, "Log.txt", "w") != 0) {
		MessageBox(NULL, TEXT("Log File Cannot be created"), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else {
		fprintf(gpFile, "Log file is succesfully open\n");
	}

	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.lpszClassName = szWndClsName;
	WndClass.lpszMenuName = NULL;
	WndClass.lpfnWndProc = WndProc;
	WndClass.hInstance = hInstance;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);

	if (SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0) == FALSE) {
		MessageBox(NULL, TEXT("Failed to Get Screen Dimension"), TEXT("Error"), MB_OK);
	}
	else {
		iScreenWidth = rc.right - rc.left;
		iScreenHeight = rc.bottom - rc.top;

		iCenter_X = (iScreenWidth / 2) - (WIN_WIDTH / 2);
		iCenter_Y = (iScreenHeight / 2) - (WIN_HEIGHT / 2);
	}

	if (!RegisterClassEx(&WndClass)) {
		MessageBox(NULL, TEXT("Failed to register class"), TEXT("Error"), MB_OK);
		exit(0);
	}

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szWndClsName,
		TEXT("Snake Game"),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		iCenter_X, iCenter_Y, WIN_WIDTH, WIN_HEIGHT,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, TEXT("Failed to Create Window"), TEXT("Error"), MB_OK);
		exit(0);
	}

	ghwnd = hwnd;

	srand(time(0));

	Initialize();

	while (bDone == false) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				bDone = true;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			if (gbActiveWindow == true) {
				if (gbEscapeKeyIsPressed == true)
					bDone = true;

				Display();
			}
		}
	}

	Uninitialize();

	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	void Resize(int, int);
	void MakeFullscreen(void);
	void LeaveFullscreen(void);

	switch (iMsg) {
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			gbEscapeKeyIsPressed = true;
			break;

		case 0x46: // f or F
			if (gbFullScreen == false) {
				MakeFullscreen();
				gbFullScreen = true;
			}
			else {
				LeaveFullscreen();
				gbFullScreen = false;
			}
			break;

		case 0x57: // For w or W
		case VK_UP:
			if (!gbDownKeyIsPressed)
				gbUpKeyIsPressed = true;
			else
				gbDownKeyIsPressed = true;
			gbRightKeyIsPressed = false;
			gbLeftKeyIsPressed = false;
			break;

		case 0x44:
		case VK_RIGHT:
			if (!gbLeftKeyIsPressed)
				gbRightKeyIsPressed = true;
			else
				gbLeftKeyIsPressed = true;
			gbUpKeyIsPressed = false;
			gbDownKeyIsPressed = false;
			break;

		case 0x41:
		case VK_LEFT:
			if (!gbRightKeyIsPressed)
				gbLeftKeyIsPressed = true;
			else
				gbRightKeyIsPressed = true;
			gbUpKeyIsPressed = false;
			gbDownKeyIsPressed = false;
			break;

		case 0x53:
		case VK_DOWN:
			if (!gbUpKeyIsPressed)
				gbDownKeyIsPressed = true;
			else
				gbUpKeyIsPressed = true;
			gbRightKeyIsPressed = false;
			gbLeftKeyIsPressed = false;
			break;
		}
		break;

	case WM_ERASEBKGND:
		return 0;

	case WM_SIZE:
		igWidth = LOWORD(lParam);
		igHeight = HIWORD(lParam);
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void Initialize(void) {
	void GrowSnake(void);
	void Resize(int, int);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 24;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0) {
		MessageBox(ghwnd, TEXT("Failed to Choose Pixel Format"), TEXT("Error"), MB_OK);
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
		DestroyWindow(ghwnd);
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE) {
		MessageBox(ghwnd, TEXT("Failed to Set Pixel Format"), TEXT("Error"), MB_OK);
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
		DestroyWindow(ghwnd);
	}

	hglrc = wglCreateContext(ghdc);
	if (hglrc == NULL) {
		MessageBox(ghwnd, TEXT("Failed to Create Rendering Context"), TEXT("Error"), MB_ICONERROR);
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
		DestroyWindow(ghwnd);
	}

	if (wglMakeCurrent(ghdc, hglrc) == FALSE) {
		MessageBox(ghwnd, TEXT("Failed to Make Current Context"), TEXT("Error"), MB_ICONERROR);
		wglDeleteContext(hglrc);
		hglrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
		DestroyWindow(ghwnd);
	}

	for (int i = 0; i < snakeLength; i++)
		GrowSnake();

	randomPos = (GLfloat)(rand() % 80);
	randomPos *= 10;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Display(void) {
	void CheckBoundary(void);
	void DrawSnakeBody(void);
	void SnakeFood(void);

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	CheckBoundary();
	SnakeFood();
	DrawSnakeBody();

	SwapBuffers(ghdc);
}

void GrowSnake() {
	Node* temp;
	temp = (Node*)malloc(sizeof(Node));

	if (head == NULL) {
		head = temp;
		head->x1 = initX1;
		head->y1 = initY1;
		head->x2 = initX2;
		head->y2 = initY2;
		head->x3 = initX3;
		head->y3 = initY3;
		head->x4 = initX4;
		head->y4 = initY4;
		head->next = NULL;
		Iterate = head;
		return;
	}

	temp->x1 = Iterate->x1;
	temp->y1 = Iterate->y1 - 10.0f;
	temp->x2 = Iterate->x2;
	temp->y2 = Iterate->y2 - 10.0f;
	temp->x3 = Iterate->x3;
	temp->y3 = Iterate->y3 - 10.0f;
	temp->x4 = Iterate->x4;
	temp->y4 = Iterate->y4 - 10.0f;

	Iterate->next = temp;
	Iterate = temp;
	temp->next = NULL;
}

void CheckBoundary(void) {
	if (gbUpKeyIsPressed) {
		if (head->y4 >= 800.0f) {
			head->y1 = 0.0f;
			head->y2 = 0.0f;
			head->y3 = 10.0f;
			head->y4 = 10.0f;
		}
	}

	if (gbRightKeyIsPressed) {
		if (head->x4 >= 800.0f) {
			head->x1 = 0.0f;
			head->x2 = 10.0f;
			head->x3 = 10.0f;
			head->x4 = 0.0f;
		}
	}

	if (gbLeftKeyIsPressed) {
		if (head->x4 <= 0.0f) {
			head->x1 = 790.0f;
			head->x2 = 800.0f;
			head->x3 = 800.0f;
			head->x4 = 790.0f;
		}
	}

	if (gbDownKeyIsPressed) {
		if (head->y4 <= 0.0f) {
			head->y1 = 790.0f;
			head->y2 = 790.0f;
			head->y3 = 800.0f;
			head->y4 = 800.0f;
		}
	}
}

void UpdateNodes(Node* temp) {
	if (temp->next->next != NULL)
		UpdateNodes(temp->next);
	else
		grid[(GLint)(temp->next->x1) / 10][(GLint)(temp->next->y1) / 10] = 0;

	temp->next->x1 = temp->x1;
	temp->next->x2 = temp->x2;
	temp->next->x3 = temp->x3;
	temp->next->x4 = temp->x4;
	temp->next->y1 = temp->y1;
	temp->next->y2 = temp->y2;
	temp->next->y3 = temp->y3;
	temp->next->y4 = temp->y4;
	grid[(GLint)(temp->x1) / 10][(GLint)(temp->y1) / 10] = 2;
}

void DrawSnakeBody(void) {
	void UpdateNodes(Node*);
	Node* temp;

	temp = head;
	coloredHead = true;
	// Check if snake hit the food?
	if (grid[(GLint)(temp->x1) / 10][(GLint)(temp->y1) / 10] == 1) {
		grid[(GLint)(temp->x1) / 10][(GLint)(temp->y1) / 10] = 0;

		randomPos = (GLfloat)(rand() % 80);
		randomPos *= 10;

		if (randomPos == 0)
			randomPos = 10;

		frameIncreaseCnt += 0.025f;

		GrowSnake();

		PlaySound((LPCTSTR)SND_ALIAS_SYSTEMEXCLAMATION, NULL, SND_ALIAS_ID | SND_ASYNC);
	}
	// Self collision check
	else if (grid[(GLint)(temp->x1) / 10][(GLint)(temp->y1) / 10] == 2) {
		MessageBox(ghwnd, TEXT("Collision Occurs"), TEXT("Game Over"), MB_OK);
		exit(0);
	}

	while (temp != NULL) {
		if (coloredHead == true) {
			glBegin(GL_POLYGON);
			//glColor3f((rand() % 255)/255.0f, (rand() % 255)/255.0f, (rand() % 255)/255.0f); // For Random Color Head
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(temp->x1, temp->y1, 0.0f);
			glVertex3f(temp->x2, temp->y2, 0.0f);
			glVertex3f(temp->x3, temp->y3, 0.0f);
			glVertex3f(temp->x4, temp->y4, 0.0f);
			glEnd();
			coloredHead = false;
		}
		else {
			glBegin(GL_POLYGON);
			glColor3f(0.0f, 0.0f, 0.0f);
			glVertex3f(temp->x1, temp->y1, 0.0f);
			glColor3f(0.0f, 1.0f, 1.0f);
			glVertex3f(temp->x2, temp->y2, 0.0f);
			glColor3f(0.0f, 0.0f, 0.0f);
			glVertex3f(temp->x3, temp->y3, 0.0f);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(temp->x4, temp->y4, 0.0f);
			glEnd();
		}

		temp = temp->next;
	}

	if (frames >= 250.0f) {
		UpdateNodes(head);

		if (gbRightKeyIsPressed) {
			head->x1 += x;
			head->x2 += x;
			head->x3 += x;
			head->x4 += x;
		}
		if (gbUpKeyIsPressed) {
			head->y1 += y;
			head->y2 += y;
			head->y3 += y;
			head->y4 += y;
		}

		if (gbLeftKeyIsPressed) {
			head->x1 -= x;
			head->x2 -= x;
			head->x3 -= x;
			head->x4 -= x;
		}

		if (gbDownKeyIsPressed) {
			head->y1 -= y;
			head->y2 -= y;
			head->y3 -= y;
			head->y4 -= y;
		}
		frames = 0.0f;
	}
	frames += frameIncreaseCnt;
}

void SnakeFood(void) {
	grid[((int)(randomPos)) / 10][((int)(randomPos)) / 10] = 1;
	glBegin(GL_POLYGON);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(randomPos, randomPos + 10.0f, 0.0f);
	glVertex3f(randomPos + 10.0f, randomPos + 10.0f, 0.0f);
	glVertex3f(randomPos + 10.0f, randomPos, 0.0f);
	glVertex3f(randomPos, randomPos, 0.0f);
	glEnd();
}

void Resize(int iWidth, int iHeight) {
	if (iHeight == 0)
		iHeight = 1;

	glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (iWidth <= iHeight)
		glOrtho(0.1, 800.0, (0.1 * ((GLdouble)iHeight / (GLdouble)iWidth)), (800.0 * ((GLdouble)iHeight / (GLdouble)iWidth)), -1.0, 1.0);
	else
		glOrtho((0.1 * ((GLdouble)iWidth / (GLdouble)iHeight)), (800.0 * ((GLdouble)iWidth / (GLdouble)iHeight)), 0.1, 800.0, -1.0, 1.0);
}

void MakeFullscreen(void) {
	MONITORINFO mi = { sizeof(mi) };

	dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

	if (dwStyle & WS_OVERLAPPEDWINDOW) {
		if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi)) {
			SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
		}
	}
	ShowCursor(FALSE);
}

void LeaveFullscreen(void) {
	SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
	SetWindowPlacement(ghwnd, &wpPrev);
	SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	ShowCursor(TRUE);
}

void Uninitialize(void) {
	Node* freeNode;
	SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
	SetWindowPlacement(ghwnd, &wpPrev);
	SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	ShowCursor(TRUE);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrc);
	hglrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	while (head != NULL) {
		freeNode = head->next;
		free(head);
		head = freeNode;
	}

	fclose(gpFile);

	DestroyWindow(ghwnd);
}