#include "Common/CoreSystem.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	CoreSystem NuclearCore;
	return NuclearCore.Run(hInstance, nCmdShow);
}
