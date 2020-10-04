#include <windows.h>
#include <sstream>
#include <iostream>
#include "resource.h"

#define EXPORT extern "C" __declspec (dllexport)
EXPORT double* CALLBACK Dialog(double xvalues[2]);
double xValues[2];
HINSTANCE hInst;