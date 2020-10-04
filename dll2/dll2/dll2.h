#include <windows.h>

#define EXPORT extern "C" __declspec (dllexport)
EXPORT int CALLBACK Dialog(double Xmin, double Xmax);