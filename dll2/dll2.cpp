#include "dll2.h"

int WINAPI DllMain(HINSTANCE hInstance, DWORD fdReason, PVOID pvReserved) {
    BOOL bAllWentWell = TRUE;
    switch (fdReason) {
        case DLL_PROCESS_ATTACH:     // Инициализация процесса. 
            hInst = hInstance;
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
    return TRUE;
}

LRESULT CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    wchar_t xmin[100000], xmax[100000];
    std::wstringstream wss;
    std::wstring converted;
    std::wstring converted2;
    LPCWSTR value1, value2;
    bool hasMinus1 = false;
    bool hasMinus2 = false;
    bool hasDot1 = false;
    bool hasDot2 = false;
    int dot1 = -1;
    int dot2 = -1;
    bool isNumber = true;
    switch (message) {
        case WM_INITDIALOG:

            wss << xValues[1];
            converted = wss.str();
            value1 = converted.c_str();

            wss.str(L"");

            wss << xValues[0];
            converted2 = wss.str();
            value2 = converted2.c_str();

            wss.str(L"");

            SetDlgItemText(hDlg, IDC_EDIT1, value2);
            SetDlgItemText(hDlg, IDC_EDIT2, value1);
            return TRUE;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    GetDlgItemText(hDlg, IDC_EDIT1, xmin, sizeof(xmin));
                    GetDlgItemText(hDlg, IDC_EDIT2, xmax, sizeof(xmax));

                    for (int i = 0; i < 100000; i++) {
                        if (i == 0 && xmin[i] == '-')
                            hasMinus1 = true;
                        else if (i != 0 && xmin[i] == '.') {
                            hasDot1 = true;
                            dot1 = i;
                        }
                        else if (xmin[i] == '\0')
                            break;
                        else if ((hasMinus1 && xmin[i] == '-' && i != 0) || (hasDot1 && xmin[i] == '.' && dot1 != i)
                                 || ((xmin[i] < '0' || xmin[i]>'9') && xmin[i] != '.' && xmin[i] != '-')) {
                            isNumber = false;
                            break;
                        }
                    }

                    for (int i = 0; i < 100000; i++) {
                        if (i == 0 && xmax[i] == '-')
                            hasMinus2 = true;
                        else if (i != 0 && xmax[i] == '.') {
                            hasDot2 = true;
                            dot2 = i;
                        }
                        else if (xmax[i] == '\0')
                            break;
                        else if ((hasMinus2 && xmax[i] == '-' && i != 0) || (hasDot2 && xmax[i] == '.' && dot2 != i)
                                 || ((xmax[i] < '0' || xmax[i]>'9') && xmax[i] != '.' && xmax[i] != '-')) {
                            isNumber = false;
                            break;
                        }
                    }

                    if (!isNumber) {
                        MessageBox(NULL, L"Введите число пожалуйста" + GetLastError(), L"Неверное число", MB_ICONERROR);
                        break;
                    }


                    xValues[0] = _wtof(xmin);
                    xValues[1] = _wtof(xmax);
                    if (xValues[0] >= xValues[1])
                        MessageBox(hDlg, L"Значение Xmin должно быть меньше Xmax", L"Значения границ", MB_ICONINFORMATION);
                    else
                        EndDialog(hDlg, LOWORD(wParam));
                    return TRUE;
                    break;
                case IDCANCEL:
                    EndDialog(hDlg, LOWORD(wParam));
                    return TRUE;
                    break;
            }
            break;
        case WM_CLOSE:
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
    }
    return FALSE;
}

EXPORT double* CALLBACK Dialog(double xvalues[2]) {
    xValues[0] = xvalues[0];
    xValues[1] = xvalues[1];

    DialogBox(hInst, (LPCTSTR)IDD_DIALOG1, NULL, (DLGPROC)DlgProc);

    return xValues;
}