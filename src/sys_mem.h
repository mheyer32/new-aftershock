#ifndef SYS_MEM_H_INCLUDED
#define SYS_MEM_H_INCLUDED

/*
 memory allocation


Hunk_??? The hunk manages the entire memory block given to golk. It must be
contiguous. Memory can be allocated from either the low or high end in a
stack fashion. The only way memory is released is by resetting one of the
pointers.

Hunk allocations are guaranteed to be 32 byte aligned.

Z_??? Zone memory functions used for small, dynamic allocations like text
strings from command input.  There is only about 48K for it, allocated at
the very bottom of the hunk.


--- low memory ----
server vm
server clipmap
---mark---
renderer initialization (shaders, etc)
UI vm
cgame vm
renderer map
renderer models

---free---

temp file loading
--- high memory ---

*/

typedef enum memtag_e {
	TAG_FREE,
	TAG_GENERAL,
	TAG_BOTLIB,
	TAG_RENDERER,
	TAG_SMALL
} memtag_t;

void		Z_Init( void );

void		Z_Free( void *memory );
void		*Z_Malloc( int size );					/* returns 0 filled memory */
void		*Z_SmallMalloc( int size );
void		*Z_TagMalloc( int size, memtag_t tag );	/* NOT 0 filled memory */
int			Z_AvailableMemory( void );

void		Z_CheckHeap( void );

void		Hunk_Init( void );

void		Hunk_Clear( void );
void		Hunk_ClearToMark( void );
void		Hunk_SetMark( void );
aboolean	Hunk_CheckMark( void );
void		Hunk_ClearTempMemory( void );
aboolean	Hunk_MarksSet( void );

void		*Hunk_AllocateTempMemory( int size );
void		Hunk_FreeTempMemory( void *memory );

int			Hunk_MemoryRemaining( void );

#endif