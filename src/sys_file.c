
#include "a_shared.h"
#include <windows.h>
#include "console.h"


# define OPEN_READONLY			0
# define OPEN_WRITEONLY			1
# define OPEN_READWRITE			2
# define OPEN_APPEND			3





void * File_Open (const char * path ,int mode )
{
	int access ,creation_mode,flags;
	void * file;

	switch (mode) {
		case OPEN_READONLY:
			access = GENERIC_READ;
			creation_mode = OPEN_EXISTING;
			flags = FILE_FLAG_RANDOM_ACCESS|FILE_ATTRIBUTE_READONLY;
			break;
		case OPEN_WRITEONLY:
			access = GENERIC_WRITE;
			creation_mode = CREATE_ALWAYS;
			flags = FILE_FLAG_RANDOM_ACCESS|FILE_ATTRIBUTE_NORMAL;
			break;
		case OPEN_READWRITE:
			access = GENERIC_READ|GENERIC_WRITE;
			creation_mode = CREATE_ALWAYS;
			flags = FILE_FLAG_RANDOM_ACCESS|FILE_ATTRIBUTE_NORMAL;
			break;
		case OPEN_APPEND:
			access = GENERIC_WRITE;
			creation_mode = OPEN_ALWAYS;
			flags = FILE_FLAG_RANDOM_ACCESS|FILE_ATTRIBUTE_NORMAL;
			break;
		default:
			Con_Printf ("File_Open: Bad mode \n");
			return(0);
	}

	file =CreateFile(path, access, FILE_SHARE_READ, NULL, creation_mode, flags, NULL);

	if (file==INVALID_HANDLE_VALUE)
			return NULL;
	else 
		return file ;

}

int File_Close ( void * file )
{
	if (!file )
	{
		Con_Printf ("File_Close : NULL \n");
		return 0;
	}


	return (CloseHandle(file));


}

int File_GetLen ( void * file )
{

	if (!file  )
	{
		Con_DPrintf ("File_GetLen : NULL \n");
		return 0;
	}

	return GetFileSize(file,NULL);
}


size_t File_Read(void *buf, size_t size, void* stream)
{
	int dwSizeRead;
	if (!ReadFile(stream, buf, size, &dwSizeRead, NULL))
		return(0);
	return ((size_t) dwSizeRead);
}

size_t File_Write(const void *buf, size_t size,void * stream)
{
	int dwSizeWritten;
	if (!WriteFile(stream, buf, size, &dwSizeWritten, NULL))
		return(0);
	return ((size_t) dwSizeWritten);
}



void* File_FindFirst (const char *path, char *filename, unsigned char musthave, unsigned char canthave)
{

	HANDLE hSearch;
	WIN32_FIND_DATA finddata;
	char *pattern = (char *)malloc(strlen(path)+3);

	strcpy (pattern,path);
	//sprintf(pattern, "%s\\*", path);
//	ConvertPath(pattern, SYSTEM_PATH);
	hSearch = FindFirstFile(pattern, &finddata);
	if (hSearch != INVALID_HANDLE_VALUE) {
		do {
			if ((finddata.dwFileAttributes & musthave) != musthave) continue;
			if (finddata.dwFileAttributes & canthave) continue;

			strcpy(filename, finddata.cFileName);
			free(pattern);
			return (hSearch);
		} while (FindNextFile(hSearch, &finddata));
		FindClose(hSearch);
	}
	free(pattern);

	return (0);
}


unsigned char File_FindNext(void *handle, char *filename, unsigned char musthave, unsigned char canthave)
{
	WIN32_FIND_DATA finddata;

	if (handle != INVALID_HANDLE_VALUE) {
		while (FindNextFile(handle, &finddata)) {
			if ((finddata.dwFileAttributes & musthave) != musthave) continue;
			if (finddata.dwFileAttributes & canthave) continue;

			strcpy(filename, finddata.cFileName);
			return (1);
		}
	}

	return (0);
}

void File_FindClose(void *handle)
{
	if (handle != INVALID_HANDLE_VALUE) {
		FindClose(handle);
	}
}