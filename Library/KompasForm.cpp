#include "StdAfx.h"
#include "KompasForm.h"
#include <windows.h>
#include <tchar.h>
#include <gdiplus.h>
#include "resource.h"

using namespace Gdiplus;
#pragma comment (lib, "Gdiplus.lib")

// Внешние функции из DLL
extern void CreateCylinder(int flag);

// Глобальные переменные
extern HINSTANCE g_hInstance;
HWND hWndButton1 = NULL;
HWND hWndButton2 = NULL;
Bitmap* pImage = NULL;

// Обработчик окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // изображение
        pImage = new Bitmap(L"C:/Users/User/Desktop/vsu/6sem/OAP/Library/Library/image.bmp");
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        if (pImage && pImage->GetLastStatus() == Ok)
        {
            Graphics graphics(hdc);
            graphics.DrawImage(pImage, 20, 20, 250, 170);
        }

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_COMMAND:
        if ((HWND)lParam == hWndButton1)
        {
            MessageBox(hWnd, _T("Построение опоры..."), _T("Инфо"), MB_OK);
            CreateCylinder(0);
        }
        else if ((HWND)lParam == hWndButton2)
        {
            MessageBox(hWnd, _T("Построение опоры с крепёжными элементами..."), _T("Инфо"), MB_OK);
            CreateCylinder(1);
        }
        break;

    case WM_DESTROY:
        if (pImage) { delete pImage; pImage = NULL; }
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Функция показа формы
void ShowKompasWindow()
{
    // Инициализация GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    const TCHAR* className = _T("KompasWinClass");

    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = g_hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = className;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
        return;

    HWND hWnd = CreateWindow(className, _T("Компас приложение"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 450, 350, NULL, NULL, g_hInstance, NULL); 

    if (!hWnd)
        return;

    // Первая кнопка
    hWndButton1 = CreateWindow(_T("BUTTON"), _T("Построить опору"),
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        100, 220, 200, 30,
        hWnd, NULL, g_hInstance, NULL);

    // Вторая кнопка
    hWndButton2 = CreateWindow(_T("BUTTON"), _T("Опора с крепёжными элементами"),
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        100, 260, 300, 30,
        hWnd, NULL, g_hInstance, NULL);

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    // Цикл сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);
}