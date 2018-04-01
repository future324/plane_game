#include"Planegame.hpp"
		
LRESULT CALLBACK	WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR m_pGdiToken;
	GdiplusStartup(&m_pGdiToken, &m_gdiplusStartupInput, NULL);
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"img//fly.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"ClassName";


	if (!RegisterClassEx(&wndClass))
		return -1;
	RECT rc;
	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);
	rc.left = (cxScreen-WINDOW_WIDTH) / 2;
	rc.top = (cyScreen - WINDOW_HEIGHT) / 2;
	rc.right = rc.left + WINDOW_WIDTH;
	rc.bottom = rc.top + WINDOW_HEIGHT;
	AdjustWindowRect(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

	HWND hwnd = CreateWindow(wndClass.lpszClassName, WINDOW_TITLE,
		WS_OVERLAPPED |WS_CAPTION | WS_SYSMENU| WS_MINIMIZEBOX, rc.left, rc.top, rc.right- rc.left,
		rc.bottom- rc.top, NULL, NULL, hInstance, NULL);


	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);


	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	Gdiplus::GdiplusShutdown(m_pGdiToken);
	UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;

	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CREATE:
		GameInit();
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GamePaint(hdc);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_TIMER:
		GameLogic();
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	case WM_LBUTTONDOWN:
		MouseClick(hwnd, LOWORD(lParam), HIWORD(lParam));
		return 0;
	case WM_MOUSEMOVE:
		if (WM_LBUTTONDOWN&wParam)
		{
			UpdatePlayerPos(LOWORD(lParam), HIWORD(lParam));
		}
		else
		{
			MenuCallback((int)LOWORD(lParam), (int)HIWORD(lParam));
		}
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);
		break;
	case WM_SETFOCUS:
		SetTimer(hwnd, TIMERID, 1000 / GAMEFPS, NULL);
		return 0;
	case WM_KILLFOCUS:
		KillTimer(hwnd, TIMERID);
		return 0;
	case WM_SYSCOMMAND:
		if (wParam==SC_CLOSE)
		{
			if (IDCANCEL == MessageBox(hwnd, L"您是否要退出程序", L"提示！！！", MB_OKCANCEL | MB_ICONQUESTION))
			{
				return 0;
			}
		}
		break;
	case WM_DESTROY:
		ReleaseRes();
		PostQuitMessage(0);
		break;
	}

	return  DefWindowProc(hwnd, message, wParam, lParam);
}

