

#ifndef SYS_FILE_H__
#define SYS_FILE_H__


void * File_Open (const char * path ,int mode );
int File_Close ( void * file );
int File_GetLen ( void * file );
size_t File_Read(void *buf, size_t size, void* stream);
size_t File_Write(const void *buf, size_t size,void * stream);
void* File_FindFirst (const char *path, char *filename, unsigned char musthave, unsigned char canthave);
unsigned char File_FindNext(void *handle, char *filename, unsigned char musthave, unsigned char canthave);
void File_FindClose(void *handle);





#endif 



