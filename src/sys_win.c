
#include <windows.h>
#include "engine.h"
#include "util.h"
#include "sys_win.h"

HINSTANCE hInst;
HDC dc;
HWND hWnd;
HGLRC hRC;
int nCmdShow;

int winX = 640;
int winY = 480;

int WIN_GetRemaining_Memory(void)
{
	MEMORYSTATUS memstat;
	unsigned long mem;

	GlobalMemoryStatus(&memstat);
	mem = memstat.dwAvailPhys + memstat.dwAvailVirtual;

	// Vic: a hack to UI_MEMORY_REMAINING work,
	// cause windows uses DWORD for rem. memory
	// and if the value is too high, int doesn't 
	// work anymore.
	if (mem > LOW_MEMORY)
		mem = LOW_MEMORY + 1;

	return (int)mem;
}

unsigned int WIN_Get_Time (void)
{
	static unsigned int starttime = 0;
	unsigned int now;

	now = timeGetTime();

	if (!starttime) {
		starttime = now;
	}

	return (now - starttime);
}


void WIN_SetPriority(int priority) {
	HANDLE    hProc;
	hProc = GetCurrentProcess();
	
	if (priority == 0) {
		SetPriorityClass(hProc, IDLE_PRIORITY_CLASS);
	} else if (priority == 1) {
		SetPriorityClass(hProc, NORMAL_PRIORITY_CLASS);
	} else if (priority == 3) {
		SetPriorityClass(hProc, REALTIME_PRIORITY_CLASS);
	} else {
		SetPriorityClass(hProc, HIGH_PRIORITY_CLASS);
	}
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     char	   *lpCmdLine,
                     int       CmdShow)
{
	MSG msg;

	hInst = hInstance;
	nCmdShow = CmdShow;

	if (!Engine_Init ()) {
		Error ("Could not Init Engine !!!");
		return 1;
	}
    
	while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	{
		Engine_Render_Next_Frame();
		DispatchMessage( &msg );
	}

	Engine_Shutdown();

	return msg.wParam;
}
