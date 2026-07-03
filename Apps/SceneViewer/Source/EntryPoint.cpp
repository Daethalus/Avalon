#include <iostream>

#include "SceneViewer.hpp"
#include "Avalon/Common.hpp"

i32 Main()
{
	Avalon::SceneViewer sceneViewer;
	return sceneViewer.Run();
}

#ifdef AV_WIN
#define NOMINMAX
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return Main();
}
#else
int main(int argc, char** argv)
{
	return Main();
}
#endif
