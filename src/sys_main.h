

#ifndef SYS_MAIN_H__
#define SYS_MAIN_H__





#ifdef _WIN32 

#include "sys_win.h" 

#define Sys_Get_Time  WIN_Get_Time 

#define Sys_GetRemaining_Memory WIN_GetRemaining_Memory
#define Sys_SetPriority (p) WIN_SetPriority(p)


#else 

#error "NOT RUNNING UNDER WIN32 : NO OTHER PLATFORMS SUPPORTED !"

#endif 


















#endif 