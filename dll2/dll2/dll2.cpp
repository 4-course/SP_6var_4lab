#include "dll2.h"

int WINAPI DllMain(HINSTANCE hInstance, DWORD fdReason, PVOID pvReserved) {
    BOOL bAllWentWell = TRUE;
    switch (fdReason) {
        case DLL_PROCESS_ATTACH:     // ������������� ��������. 
            break;
        case DLL_THREAD_ATTACH:     // ������������� ������.
            break;
        case DLL_THREAD_DETACH:     // ������� �������� ������.
            break;
        case DLL_PROCESS_DETACH:     // ������� �������� ��������.
            break;
    }
    if (bAllWentWell)     return TRUE;
    else            return FALSE;
}

EXPORT int CALLBACK Dialog(double Xmin, double Xmax) {
    MessageBox(NULL, L"sfh", L"Function from DLL", MB_OK);
    return 1;
}