#include "dll1.h"

int WINAPI DllMain(HINSTANCE hInstance, DWORD fdReason, PVOID pvReserved) {
    BOOL bAllWentWell = TRUE;
    switch (fdReason) {
        case DLL_PROCESS_ATTACH:     // Инициализация процесса. 
            break;
        case DLL_THREAD_ATTACH:     // Инициализация потока.
            break;
        case DLL_THREAD_DETACH:     // Очистка структур потока.
            break;
        case DLL_PROCESS_DETACH:     // Очистка структур процесса.
            break;
    }
    if (bAllWentWell)     return TRUE;
    else            return FALSE;
}

EXPORT int CALLBACK DrawGraph(HWND hWnd, HDC hdc, double Xmin, double Xmax) {
    HPEN pen, graphPen;
    HFONT font;
    POINT center;
    RECT rt;
    PAINTSTRUCT ps;

    double Ymin=0, Ymax=2,
        mainWindowWidth, mainWindowHeight,
        scaleX, scaleY,
        offsetWidth, offsetHeight,
        workWidth, workHeight,
        arrowLength,
        divisionStepX, divisionStepY, divisionLength;
    int divisionsCountX, divisionsCountY;
    double pointsX[1000];
    double pointsY[1000];

    RECT rect;
    GetWindowRect(hWnd, &rect);
    mainWindowWidth = (double)rect.right - rect.left;
    mainWindowHeight = (double)rect.bottom - rect.top;

    hdc = BeginPaint(hWnd, &ps);

    pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    font = CreateFont(mainWindowHeight * 0.035, 8, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET,
                      OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
    SelectObject(hdc, font);
    SelectObject(hdc, pen);

    GetClientRect(hWnd, &rt);
    FillRect(hdc, &rt, (HBRUSH)(COLOR_WINDOW + 1));

    //на данном этапе надо посчитать размер шрифта, размер интервала
    //определим рабочую область
    offsetWidth = 0.05 * mainWindowWidth;
    offsetHeight = 0.15 * mainWindowHeight;

    workWidth = mainWindowWidth - 2.0 * offsetWidth;
    workHeight = mainWindowHeight - 2.0 * offsetHeight;

    //и машстаб
    scaleX = workWidth / ((double)Xmax - Xmin);
    scaleY = workHeight / 2.0;

    //рисуем оси. всегда видна ось X, рисуем ее
    MoveToEx(hdc, offsetWidth, mainWindowHeight - offsetHeight, NULL);
    LineTo(hdc, mainWindowWidth - offsetWidth, mainWindowHeight - offsetHeight);

    //рисуем стрелочку у оси Х
    //определим длину стрелочки
    arrowLength = offsetWidth / 3.0;
    LineTo(hdc, mainWindowWidth - offsetWidth - arrowLength * sqrt(3.0) / 2.0, mainWindowHeight - offsetHeight - arrowLength / 2.0);
    MoveToEx(hdc, mainWindowWidth - offsetWidth, mainWindowHeight - offsetHeight, NULL);
    LineTo(hdc, mainWindowWidth - offsetWidth - arrowLength * sqrt(3.0) / 2.0, mainWindowHeight - offsetHeight + arrowLength / 2.0);

    double x = 0, y = 0;
    //рисуем ось Y
    //тут может быть несколько вариантов
    //1 - когда ось Y видна
    if (Xmin <= 0 && Xmax >= 0) {
        x = offsetWidth + scaleX * abs(Xmin);
        y = mainWindowHeight - offsetHeight;
        center.x = x;
        center.y = y;
    }
    //2 - когда ось Y не видна, рисуем ее просто слева/справа
    else if (Xmin < 0 && Xmax < 0) {
        x = mainWindowWidth - offsetWidth;
        y = mainWindowHeight - offsetHeight;
        center.x = x + scaleX * abs(Xmax);
        center.y = y;
    }
    else if (Xmin > 0 && Xmax > 0) {
        x = offsetWidth;
        y = mainWindowHeight - offsetHeight;
        center.x = x - Xmin * scaleX;
        center.y = y;
    }

    MoveToEx(hdc, x, y, NULL);
    LineTo(hdc, x, offsetHeight / 2.0);
    //и стрелочку 
    LineTo(hdc, x - arrowLength / 2.0, offsetHeight / 2.0 + arrowLength * sqrt(3.0) / 2.0);
    MoveToEx(hdc, x, offsetHeight / 2.0, NULL);
    LineTo(hdc, x + arrowLength / 2.0, offsetHeight / 2.0 + arrowLength * sqrt(3.0) / 2.0);

    SetViewportExtEx(hdc, workWidth, -workHeight, NULL);
    SetViewportOrgEx(hdc, center.x, center.y, NULL);

    //определимся с делением по оси Х
    divisionsCountX = 0.05 * workWidth / M_PI;
    divisionStepX = ((double)abs(Xmin) + abs(Xmax)) / divisionsCountX;

    divisionsCountY = workHeight / 50;
    divisionStepY = Ymax * scaleY / divisionsCountY;

    divisionLength = 0.025 * mainWindowHeight;

    int leftDivisionsCount = 0;
    int rightDivisionCount = divisionsCountX - 1;
    double centerX = 0;
    double divisionValue = ((double)abs(Xmax) + abs(Xmin)) / divisionsCountX;
    double step = divisionValue;

    if (Xmin <= 0 && Xmax >= 0) {
        leftDivisionsCount = abs(Xmin) / divisionStepX;
        rightDivisionCount = divisionsCountX - leftDivisionsCount - 1;
    }
    else if (Xmin < 0 && Xmax < 0) {
        leftDivisionsCount = divisionsCountX - 1;
        rightDivisionCount = 0;
    }

    for (int i = 1; i <= rightDivisionCount; i++) {
        MoveToEx(hdc, centerX + i * divisionStepX * scaleX, divisionLength / 2, NULL);
        LineTo(hdc, centerX + i * divisionStepX * scaleX, -divisionLength / 2);

        std::wstringstream wss;
        wss << round(divisionValue * 100) / 100;
        std::wstring converted = wss.str();
        LPCWSTR value = converted.c_str();

        TextOut(hdc, centerX + i * divisionStepX * scaleX - 8, (divisionLength / 2 + mainWindowHeight * 0.005), value, converted.size());
        divisionValue += step;
    }

    divisionValue = -step;

    for (int i = 1; i <= leftDivisionsCount; i++) {
        MoveToEx(hdc, centerX - i * divisionStepX * scaleX, divisionLength / 2, NULL);
        LineTo(hdc, centerX - i * divisionStepX * scaleX, -divisionLength / 2);

        std::wstringstream wss;
        wss << round(divisionValue * 100) / 100;
        std::wstring converted = wss.str();
        LPCWSTR value = converted.c_str();

        TextOut(hdc, centerX - i * divisionStepX * scaleX - 8, (divisionLength / 2 + mainWindowHeight * 0.005), value, converted.size());
        divisionValue -= step;
    }

    TextOut(hdc, -4, (divisionLength / 2 + mainWindowHeight * 0.005), L"0", 1);

    divisionValue = (double)Ymax / divisionsCountY;
    step = divisionValue;
    double centerY;
    double textLocation = 0.0;
    if (Xmin <= 0 && Xmax >= 0) {
        centerY = 0;
        textLocation = divisionLength / 2 + 1;
    }
    else if (Xmin > 0 && Xmax > 0) {
        centerY = Xmin * scaleX;
        textLocation = centerY + divisionLength / 2 + 1;
    }
    else {
        centerY = Xmax * scaleX;
        textLocation = centerY - divisionLength / 2 - 28;
    }
    for (int i = 1; i <= divisionsCountY; i++) {
        MoveToEx(hdc, centerY + divisionLength / 2, -i * divisionStepY, NULL);
        LineTo(hdc, centerY - divisionLength / 2, -i * divisionStepY);

        std::wstringstream wss;
        wss << round(divisionValue * 100) / 100;
        std::wstring converted = wss.str();
        LPCWSTR value = converted.c_str();

        TextOut(hdc, textLocation, -i * divisionStepY - 8, value, converted.size());
        divisionValue += step;
    }

    step = ((double)Xmax - Xmin) / 1000.0;
    double k = Xmin;
    for (int i = 0; i < 1000; i++) {
        pointsX[i] = scaleX * k;
        pointsY[i] = -scaleY * (1 - cos(k));
        k += step;
    }

    graphPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
    SelectObject(hdc, graphPen);
    MoveToEx(hdc, pointsX[0], pointsY[0], NULL);

    for (int i = 1; i < 1000; i++)
        LineTo(hdc, pointsX[i], pointsY[i]);

    EndPaint(hWnd, &ps);

    DeleteObject(pen);
    DeleteObject(graphPen);
    DeleteObject(font);

    return 1;
}