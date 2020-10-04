#include "dll2.h"

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

EXPORT int CALLBACK Dialog(double Xmin, double Xmax) {
    MessageBox(NULL, L"sfh", L"Function from DLL", MB_OK);
    return 1;
}