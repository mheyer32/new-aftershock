
#include <windows.h>

HINSTANCE hInst;
HDC dc;
HWND hWnd;
HGLRC hRC;
int nCmdShow;


int winX=800;
int winY=600;


int WIN_GetRemaining_Memory(void)
{

	MEMORYSTATUS memstat;
	 GlobalMemoryStatus(&memstat);
 
	return memstat.dwAvailPhys+memstat.dwAvailVirtual;

}




unsigned int WIN_Get_Time (void )
{

	static  unsigned int starttime = 0;
	unsigned int now;

	now = timeGetTime();

	if(!starttime) {
		starttime = now;
	}
	return (now-starttime);
}



void WIN_SetPriority(int priority) {
  HANDLE    hProc;
  hProc = GetCurrentProcess();

  if (priority==0) {
    SetPriorityClass(hProc, IDLE_PRIORITY_CLASS);
  } else if (priority==1) {
    SetPriorityClass(hProc, NORMAL_PRIORITY_CLASS);
  } else if (priority==3) {
    SetPriorityClass(hProc, REALTIME_PRIORITY_CLASS);
  } else {
    SetPriorityClass(hProc, HIGH_PRIORITY_CLASS);
  }
}


static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);








int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     char *    lpCmdLine,
                     int       CmdShow )
{
	MSG msg;
	hInst=hInstance;
	nCmdShow=CmdShow;
	if (!Engine_Init ())
		Error ("Could not Init Engine !!!");
    


	while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ))//GetMessage(&msg, hWnd, 0, 0) ) 
	{
		
			Engine_Render_Next_Frame();
			DispatchMessage( &msg );
		
	}


 

	Engine_Shutdown();

	return msg.wParam;
}




 


