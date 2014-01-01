#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "TrayIcon.h"

#include "boost/thread.hpp"
#include "boost/asio/io_service.hpp"

#include "dust/storage/cached_db.h"

#include "bot.h"

#include "../ws_server.h"
#include "../make_unique.h"
#include "../botscript_server_version.h"

using namespace botscript;
using namespace botscript_server;

HWND g_hMainWnd = NULL;
CTrayIcon g_TrayIcon("Makielskis Bot", true,
                     LoadIcon(GetModuleHandle(NULL),
                              MAKEINTRESOURCE(101)));

boost::asio::io_service io_service;
botscript_server::ws_server s(false, "packages", &io_service,
                              std::make_shared<dust::cached_db>("db"));
boost::thread t;

void on_action(CTrayIcon* pTrayIcon, UINT uMsg) {
  if (uMsg != WM_RBUTTONUP) {
    return;
  }

  POINT pt;
  if (GetCursorPos(&pt)) {
    HMENU hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING | MF_GRAYED, 1,
               std::string(std::string("v") + version()).c_str());
    AppendMenu(hMenu, MF_STRING, 2, "Bot");
    AppendMenu(hMenu, MF_STRING, 3, "Quit");

    SetForegroundWindow(g_hMainWnd);
    UINT cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD|TPM_RIGHTBUTTON,
                              pt.x, pt.y, 0, g_hMainWnd, NULL);
    if (cmd == 2) {
      ShellExecute(NULL, "open", MOBILE_UI_URL, NULL, NULL, SW_SHOWNORMAL);
    } else if (cmd == 3) {
      PostMessage(g_hMainWnd, WM_CLOSE, 0, 0);
    }
    DestroyMenu(hMenu);
  }
}

LRESULT CALLBACK MainWndProc(
  HWND hWnd,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam) {
  if (uMsg == WM_DESTROY) {
    s.stop();
    t.join();
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool CreateMainWnd() {
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

int main() {
  if (!CreateMainWnd())
    return -1;

  t = boost::thread(([]() { s.start("127.0.0.1", "9003"); }));

  g_TrayIcon.SetListener(on_action);
  g_TrayIcon.SetVisible(true);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return (int) msg.wParam;
}
