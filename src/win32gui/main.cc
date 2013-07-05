#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "TrayIcon.h"

#include <thread>

#include "../service.h"

HWND g_hMainWnd = NULL;
CTrayIcon g_TrayIcon("Makielskis Bot", true, LoadIcon (GetModuleHandle(NULL), MAKEINTRESOURCE(101)));
botscript_server::service s;
std::thread t;

void on_action(CTrayIcon* pTrayIcon, UINT uMsg) {
	if (uMsg != WM_RBUTTONUP) {
    return;
  }

	POINT pt;
	if (GetCursorPos(&pt)) {
		HMENU menu = CreatePopupMenu();
		AppendMenu(menu, MF_STRING, 1, "Bot");
		AppendMenu(menu, MF_STRING, 2, "Quit");
		UINT cmd = TrackPopupMenu(menu, TPM_RETURNCMD|TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_hMainWnd, NULL);
		if (cmd == 1) {
			ShellExecute(NULL, "open", "http://makielski.net/mobileui", NULL, NULL, SW_SHOWNORMAL);
    } else if (cmd == 2) {
			PostMessage(g_hMainWnd, WM_CLOSE, 0, 0);
    }
	}
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (uMsg == WM_DESTROY) {
    s.stop();
    t.join();
		PostQuitMessage(0);
		return 0;
  }
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool CreateMainWnd()
{
	static const char CLASS_NAME[] = "Main Class";

	WNDCLASS wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = (HINSTANCE) GetModuleHandle(NULL);
	wc.lpfnWndProc = &MainWndProc;
	wc.lpszClassName = CLASS_NAME;
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

	if (!RegisterClass(&wc))
		return false;

	g_hMainWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		"Main Window",
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
		NULL,
		NULL,
		wc.hInstance,
		NULL
	);

	return true;
}

int CALLBACK WinMain(
  _In_  HINSTANCE hInstance,
  _In_  HINSTANCE hPrevInstance,
  _In_  LPSTR lpCmdLine,
  _In_  int nCmdShow
) {
	if (!CreateMainWnd())
		return -1;

  t = std::thread(([]() { s.start(9003); }));

	g_TrayIcon.SetListener(on_action);
  g_TrayIcon.SetVisible(true);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}