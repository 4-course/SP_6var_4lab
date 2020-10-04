#include <windows.h>
#include <stdio.h>
#include "resource.h"

double xvalues[2];

typedef int(*Draw)(HWND, HDC, double, double);
typedef double* (*DialogBoxDll)(double[2]);

HMODULE draw, dialog;
Draw graph;
DialogBoxDll dialogBox;
DialogBoxDll xmin, xmax;

void checkIsCLassRegistered(WNDCLASSEX windowClass) {
    if (!RegisterClassEx(&windowClass)) {
        MessageBox(NULL, L"Не удалось создать класс окна. Ошибка " + GetLastError(), L"Создание класса окна", MB_ICONERROR);
        exit(0);
    }
}

void checkIsWindowCreated(HWND window) {
    if (!window) {
        MessageBox(NULL, L"Не удалось создать окно. Ошибка " + GetLastError(), L"Создание окна", MB_ICONERROR);
        exit(0);
    }
}

void getDialog(HWND hWnd) {
    double* returned;
    if (dialog == NULL) {
        dialog = LoadLibrary(L"dll2");
        if (dialog == NULL) {
            MessageBox(hWnd, L"Не удалось загрузить библиотеку диалогового блока", L"DLL ERROR", NULL);
            EnableMenuItem(GetMenu(hWnd), ID_40001, MF_DISABLED | MF_GRAYED);
            return;
        }
    }
    EnableMenuItem(GetMenu(hWnd), ID_40001, MF_ENABLED);
    if (dialogBox == NULL) {
        dialogBox = (DialogBoxDll)GetProcAddress(dialog, "Dialog");
        if (dialogBox == NULL) {
            MessageBox(hWnd, L"Не удалось загрузить библиотеку диалогового блока", L"DLL ERROR", NULL);
            EnableMenuItem(GetMenu(hWnd), ID_40001, MF_DISABLED | MF_GRAYED);
            return;
        }
    }
    returned = dialogBox(xvalues);
    xvalues[0] = returned[0];
    xvalues[1] = returned[1];
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    RECT rt;
    HDC hdc = NULL;
    PAINTSTRUCT ps;

    switch (uMsg) {
        case WM_CREATE:
            draw = LoadLibrary(L"dll1");

            if (draw == NULL) {
                MessageBox(hWnd, L"Не удалось загрузить библиотеку отрисовки графика", L"DLL ERROR", NULL);
                SendMessage(hWnd, WM_CLOSE, wParam, lParam);
            }
            else {
                graph = (Draw)GetProcAddress(draw, "DrawGraph");
                if (graph == NULL) {
                    MessageBox(hWnd, L"Не удалось загрузить библиотеку отрисовки графика", L"DLL ERROR", NULL);
                    SendMessage(hWnd, WM_CLOSE, wParam, lParam);
                }
            }
            break;
        case WM_PAINT:

            graph(hWnd, hdc, xvalues[0], xvalues[1]);

            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_40001:
                    getDialog(hWnd);
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
            }
            break;
        case WM_LBUTTONDOWN:
            getDialog(hWnd);
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        case WM_SIZE:
            GetClientRect(hWnd, &rt);
            InvalidateRect(hWnd, &rt, FALSE);
            break;
        case WM_CLOSE:
            if (draw!=NULL)
                FreeLibrary(draw);
            if (dialog != NULL)
                FreeLibrary(dialog);
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst,
                   HINSTANCE hPreviousInst,
                   LPSTR lpCommandLine,
                   int nCommandShow) {
    double screenWidth = GetSystemMetrics(SM_CXSCREEN),
        screenHeight = GetSystemMetrics(SM_CYSCREEN);
    MSG uMsg;
    WNDCLASSEX mainWindowClass;
    HWND mainWindow;
    /**Xmin = -100;
    *Xmax = 50;*/
    xvalues[0] = -100;
    xvalues[1] = 50;
    memset(&mainWindowClass, 0, sizeof(WNDCLASSEX));
    mainWindowClass.cbSize = sizeof(WNDCLASSEX);
    mainWindowClass.hbrBackground = (HBRUSH)WHITE_BRUSH;
    mainWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    mainWindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    mainWindowClass.hInstance = hInst;
    mainWindowClass.lpfnWndProc = WindowProc;
    mainWindowClass.lpszClassName = L"mainWindow";
    mainWindowClass.lpszMenuName = (LPCWSTR)IDR_MENU1;


    checkIsCLassRegistered(mainWindowClass);
    mainWindow = CreateWindow(mainWindowClass.lpszClassName, L"График функции", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_THICKFRAME,
                              (screenWidth - 850) / 2, (screenHeight - 600) / 2,
                              850, 600, NULL, NULL, hInst, NULL);

    checkIsWindowCreated(mainWindow);

    ShowWindow(mainWindow, nCommandShow);

    while (GetMessage(&uMsg, NULL, NULL, NULL)) {
        TranslateMessage(&uMsg);
        DispatchMessage(&uMsg);
    }
    return uMsg.wParam;
    return 0;
}