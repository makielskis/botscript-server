#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <thread>

#include "TrayIcon.h"

#include "boost/asio/io_service.hpp"
#include "boost/program_options.hpp"

#include "dust/storage/cached_db.h"

#include "bot.h"

#include "../botscript_server_version.h"
#include "../conf/bs_server_options.h"
#include "../conf/dust_server_options.h"
#include "../conf/options_parser.h"
#include "../conf/ws_server_options.h"
#include "../ws_server.h"

using namespace botscript_server;
using namespace dust;
namespace po = boost::program_options;

HWND g_hMainWnd = NULL;
CTrayIcon g_TrayIcon("Makielskis Bot", true,
                     LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(101)));

boost::asio::io_service io_service;
botscript_server::ws_server* s;
std::thread t;

void on_action(CTrayIcon* pTrayIcon, UINT uMsg) {
  if (uMsg != WM_RBUTTONUP) {
    return;
  }

  POINT pt;
  if (GetCursorPos(&pt)) {
    HMENU hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING | MF_GRAYED, 1,
               std::string(std::string("v") + short_version()).c_str());
    AppendMenu(hMenu, MF_STRING, 2, "Bot");
    AppendMenu(hMenu, MF_STRING, 3, "Quit");

    SetForegroundWindow(g_hMainWnd);
    UINT cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x,
                              pt.y, 0, g_hMainWnd, NULL);
    if (cmd == 2) {
      ShellExecute(NULL, "open", MOBILE_UI_URL, NULL, NULL, SW_SHOWNORMAL);
    } else if (cmd == 3) {
      PostMessage(g_hMainWnd, WM_CLOSE, 0, 0);
    }
    DestroyMenu(hMenu);
  }
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                             LPARAM lParam) {
  if (uMsg == WM_DESTROY) {
    s->stop();
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
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hInstance = (HINSTANCE)GetModuleHandle(NULL);
  wc.lpfnWndProc = &MainWndProc;
  wc.lpszClassName = CLASS_NAME;
  wc.lpszMenuName = NULL;
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

  if (!RegisterClass(&wc)) return false;

  g_hMainWnd = CreateWindowEx(
      0, CLASS_NAME, "Main Window",
      WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
      CW_USEDEFAULT, CW_USEDEFAULT, 300, 200, NULL, NULL, wc.hInstance, NULL);

  return true;
}

#if defined(_MSC_VER) && !defined(__MINGW32__) and !defined(__MINGW64__)
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance,
                     _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
  char* argv[] = {0};
  int argc = 0;
#else
int main(int argc, char* argv[]) {
#endif
  if (!CreateMainWnd()) return -1;

  ws_server_options wss_options("127.0.0.1", "9003");
  bs_server_options bss_options(false, true, "packages", false);

  options_parser parser({&wss_options, &bss_options});
  parser.read_configuration_file();
  parser.print_unrecognized(std::cout);
  parser.print_used(std::cout);

  // Start servers.
  try {
    auto store = std::make_shared<cached_db>("db");
    boost::asio::io_service ios;
    ws_server wss(std::move(wss_options), std::move(bss_options), &ios, store);
    s = &wss;
    s->start();
    t = std::thread(([&ios]() {
      try {
        ios.run();
      } catch (std::exception const& e) {
        MessageBox(NULL, e.what(), "A serious problem ...", MB_OK);
      } catch (...) {
        MessageBox(NULL, "unknown error", "A serious problem ...", MB_OK);
      }
    }));

    g_TrayIcon.SetListener(on_action);
    g_TrayIcon.SetVisible(true);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    return (int)msg.wParam;
  } catch (std::exception const& e) {
    MessageBox(NULL, e.what(), "A serious initialization problem ...", MB_OK);
  } catch (...) {
    MessageBox(NULL, "unknown initialization error", "A serious problem ...",
               MB_OK);
  }

  return 1;
}
