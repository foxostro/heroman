#ifdef _WIN32

#include <Windows.h>

int mymain(); // from Application.cpp

int WINAPI WinMain(HINSTANCE ,  // hInstance
                   HINSTANCE ,  // hPrevInstance
                   LPSTR     ,  // lpCmdLine
                   int       )  // nCmdShow
{
	return mymain();
}

#endif
