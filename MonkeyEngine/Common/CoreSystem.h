#ifndef _CORE_SYSTEM_H_
#define _CORE_SYSTEM_H_
#include <Windows.h>

class CoreSystem {
private:
	HWND mCoreHwnd;
public:
	int Run(HINSTANCE mHinstance, int cmdShow);
protected:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
#endif
