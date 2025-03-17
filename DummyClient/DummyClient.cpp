#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "SendBuffer.h"

//#include <asio.hpp>
#include <boost/asio.hpp>
//#include <boost/asio/ssl.hpp>
#include "BufferWriter.h"
#include "ServerPacketHandler.h"

#include "Game.h"
#include "DummyClient.h"
#include <tchar.h>

using namespace std;


#define GWinSizeX 800
#define GWinSizeY 600

HINSTANCE           GhInst;
HWND                GhWnd;      // main window handle

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    ULONG_PTR gpToken;

    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    shared_ptr<Game> game = make_shared<Game>();
    game->Init(GhWnd);

    //Game game;
    //game.Init(GhWnd);

    MSG msg = {};

    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else
        {
            if (msg.message == WM_QUIT)
                break;

            //game.Update();
            //game.Render();

            game->Update();
            game->Render();
        }
    }
    return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"DummyClient";

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    GhInst = hInstance;

    RECT windowRect = { 0, 0, GWinSizeX, GWinSizeY };
    ::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

    HWND hWnd = CreateWindowW(L"DummyClient", L"Client", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance, nullptr);

    GhWnd = hWnd;

    if (!hWnd)
    {
        return FALSE;
    }

    ::ShowWindow(hWnd, nCmdShow);
    ::UpdateWindow(hWnd);

    return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
