/***************************************************************************
                     zone.c  -  Zone Memory Allocation
                             -------------------
    begin                : Sun Oct 17 1999
    copyright            : (C) 1999-2000 by Ingmar Stein
    email                : IngmarStein@Digital-Phenomenon.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "a_shared.h"
#include "sys_mem.h"
#include "command.h"
#include "c_var.h"
#include "io.h"

static cvar_t *com_hunkmegs;

//extern cvar_t *dedicated;

#define	DYNAMIC_SIZE	0x80000		/* == 0,5 MB */

#define	ZONEID			0x1d4a11
#define MINFRAGMENT		64

typedef struct memblock_s {
	int					size;			/* including the header and possibly tiny fragments */
	memtag_t			tag;			/* a tag of 0 is a free block */
	struct memblock_s	*next, *prev;
	int					id;        		/* should be ZONEID */
} memblock_t;

typedef struct memzone_s {
	int			size;		/* total bytes malloced, including header */
	int			datasize;
	memblock_t	blocklist;	/* start / end cap for linked list */
	memblock_t	*rover;
} memzone_t;

/*
==============================================================================

						ZONE MEMORY ALLOCATION

There is never any space between memblocks, and there will never be two
contiguous free memblocks.

The rover can be left pointing at a non-empty block

The zone calls are pretty much only used for small strings and structures,
all big things are allocated on the hunk.
==============================================================================
*/

static memzone_t	*mainzone;
static memzone_t	*smallzone;
static int			zone_size;
static int			smallzone_size;

/*
========================
Z_ClearZone
========================
*/
static void Z_ClearZone( memzone_t *zone, int size )
{
	memblock_t	*block;

	/* set the entire zone to one free block */

	zone->blocklist.next = zone->blocklist.prev = block =
		(memblock_t *)( (byte *)zone + sizeof(memzone_t) );
	zone->blocklist.tag = 1;	/* in use block */
	zone->blocklist.id = 0;
	zone->blocklist.size = 0;
	zone->rover = block;
	zone->datasize = 0;
	
	block->prev = block->next = &zone->blocklist;
	block->tag = 0;			/* free block */
	block->id = ZONEID;
	block->size = size - sizeof(memzone_t);
}

/*
========================
Z_AvailableMemory
========================
*/
int Z_AvailableMemory( void )
{
	return( mainzone->size - mainzone->datasize );
}

/*
========================
Z_Init
========================
*/
void Z_Init( void )
{
	cvar_t *com_zoneMegs;

	/* small zone */
	smallzone_size = DYNAMIC_SIZE;

	smallzone = malloc( smallzone_size );
	if( !smallzone ) {
		Com_Error( ERR_FATAL, "Small zone data failed to allocate %1.1f megs", smallzone_size / 1048576.f );
	}

	Z_ClearZone( smallzone, smallzone_size );

	/* main zone */
	/* TODO: documentation */
	com_zoneMegs = Cvar_Get( "com_zoneMegs", "16", CVAR_ARCHIVE|CVAR_LATCH );
	zone_size = com_zoneMegs->integer;
	if( zone_size < 16 ) {
		zone_size = 0x1000000;	/* 16 MB */
	} else {
		zone_size <<= 20;
	}

	mainzone = malloc( zone_size );

	if( !mainzone ) {
		Com_Error( ERR_FATAL, "Zone data failed to allocate %i megs", zone_size / 1048576 );
	}

	Z_ClearZone( mainzone, zone_size );
}

/*
========================
Z_Free
========================
*/
void Z_Free( void *ptr )
{
	memzone_t	*zone;
	memblock_t	*block, *other;

	if( !ptr )
		Com_Error( ERR_FATAL, "Z_Free: NULL pointer" );
	block = (memblock_t *) ( (byte *)ptr - sizeof(memblock_t));
	if( block->id != ZONEID )
		Com_Error( ERR_FATAL, "Z_Free: freed a pointer without ZONEID" );
	if( block->tag == 0 )
		Com_Error( ERR_FATAL, "Z_Free: freed a freed pointer" );
	if( block->tag == 5 )
		return;
	if( ((memblock_t *)((byte *)block + block->size - sizeof(memblock_t)))->id != ZONEID )
		Com_Error( ERR_FATAL, "Z_Free: memory block wrote past end" );
	if( block->tag == TAG_SMALL ) {
		zone = smallzone;
	} else {
		zone = mainzone;
	}

	zone->datasize -= block->size;
	memset( ptr, 0xAA, block->size - sizeof(memblock_t) );

	block->tag = 0;		/* mark as free */

	other = block->prev;
	if( !other->tag ) {	/* merge with previous free block */
		other->size += block->size;
		other->next = block->next;
		other->next->prev = other;
		if( block == mainzone->rover )
			mainzone->rover = other;
		block = other;
	}
	
	other = block->next;
	if( !other->tag ) {	/* merge the next free block onto the end */
		block->size += other->size;
		block->next = other->next;
		block->next->prev = block;
		if( other == mainzone->rover )
			mainzone->rover = block;
	}
}

/*
========================
Z_Malloc
========================
*/
void *Z_Malloc( int size )
{
	void	*buf;

#ifdef _DEBUG
	Z_CheckHeap();
#endif

	buf = Z_TagMalloc( size, TAG_GENERAL );
	memset( buf, 0, size );

	return( buf );
}

void *Z_SmallMalloc( int size )
{
	void *buf;

	buf = Z_TagMalloc( size, TAG_SMALL );

	return( buf );
}

/*
========================
Z_TagMalloc
========================
*/
void *Z_TagMalloc( int size, memtag_t tag )
{
	int			extra;
	char		*zonename;
	memzone_t	*zone;
	memblock_t	*start, *rover, *newmem, *base;

	if( !tag ) {
		Com_Error( ERR_FATAL, "Z_TagMalloc: tried to use a 0 tag" );
	}

	if( tag == TAG_SMALL ) {
		zone = smallzone;
	} else {
		zone = mainzone;
	}

	/*
	 * scan through the block list looking for the first free block
	 * of sufficient size
	 */
	size += sizeof(memblock_t);	/* account for size of block header */
	size += 4;					/* space for memory trash tester */
	size = (size + 3) & ~3;		/* align to 4-byte boundary */
	
	base = rover = zone->rover;
	start = base->prev;
	
	do {
		if( rover == start ) {	/* scanned all the way around the list */
			if( zone == smallzone ) {
				zonename = "small";
			} else {
				zonename = "main";
			}
			Com_Error( ERR_FATAL, "Z_Malloc: failed on allocation of %i bytes from the %s zone", size, zonename );
		}
		if( rover->tag )
			base = rover = rover->next;
		else
			rover = rover->next;
	} while( base->tag || base->size < size );
	
	/* found a block big enough */
	extra = base->size - size;
	if( extra > MINFRAGMENT ) {		/* there will be a free fragment after the allocated block */
		newmem = (memblock_t *) ((byte *)base + size );
		newmem->size = extra;
		newmem->tag = 0;			/* free block */
		newmem->prev = base;
		newmem->id = ZONEID;
		newmem->next = base->next;
		newmem->next->prev = newmem;
		base->next = newmem;
		base->size = size;
	}
	
	base->tag = tag;		/* no longer a free block */
	
	zone->rover = base->next;	/* next allocation will start looking here */
	zone->datasize += base->size;

	base->id = ZONEID;

	/* marker for memory trash testing */
	*(int *)((byte *)base + base->size - 4) = ZONEID;

	return (void *) ((byte *)base + sizeof(memblock_t));
}

/*
========================
Z_CheckHeap
========================
*/
void Z_CheckHeap( void )
{
	memblock_t	*block;

	for( block = mainzone->blocklist.next ; ; block = block->next ) {
		if( block->next == &mainzone->blocklist )
			break;			/* all blocks have been hit */
		if( (byte *)block + block->size != (byte *)block->next )
			Com_Error( ERR_FATAL, "Z_CheckHeap: block size does not touch the next block\n" );
		if( block->next->prev != block )
			Com_Error( ERR_FATAL, "Z_CheckHeap: next block doesn't have proper back link\n" );
		if( !block->tag && !block->next->tag )
			Com_Error( ERR_FATAL, "Z_CheckHeap: two consecutive free blocks\n" );
	}
}

//============================================================================

#define TEMP_MAGIC		0x89537892

typedef struct hunk_s {
	int	mark;
	int	permanent;
	int temp;
	int tempHighwater;
} hunk_t;

static byte		*hunk_base = NULL;
static int		hunk_size;

static hunk_t	hunk_low;
static hunk_t	hunk_high;
static hunk_t	*hunk_low_mark;
static hunk_t	*hunk_high_mark;

typedef struct temphunk_s {
	int magic;
	int size;
} temphunk_t;

unsigned int WIN_Get_Time (void);

/*
========================
Com_TouchMemory
========================
*/
void Com_TouchMemory( void )
{
	unsigned int	start;
	unsigned int	end;
	unsigned int	val;
	unsigned int	*mem;
	int				count;
	int				i;
	memblock_t		*block;

	Z_CheckHeap();

	start = WIN_Get_Time();

	/* touch low hunk */
	mem = (unsigned int *)hunk_base;
	val = 0;
	count = hunk_low.permanent / sizeof(unsigned int);
	for( i = count; i; i-- ) {
		val += *mem++;
	}

	/* touch high hunk */
	mem = (unsigned int *)(hunk_base + hunk_size - hunk_high.permanent);
	count = hunk_high.permanent / sizeof(unsigned int);
	for( i = count; i; i-- ) {
		val += *mem++;
	}

	/* touch zone */
	for( block = mainzone->blocklist.next ; ; block = block->next ) {
		if( block->tag ) {
			count = block->size / sizeof(unsigned int);
			mem = (unsigned int *)block;
			for( i=count; i; i-- ) {
				val += *mem++;
			}
		}
	}

	end = WIN_Get_Time();

	Com_Printf( "Com_TouchMemory: %i msec\n", end-start );
}

static void meminfo( void )
{
	memblock_t	*block;
	int			zone_used_blocks;
	int			zone_used_size;
	int			zone_botlib;
	int			zone_renderer;
	int			smallzone_used;
	int			hunk_unused_highwater;
	int			argc;

	zone_used_size = 0;
	zone_used_blocks = 0;
	smallzone_used = 0;
	zone_botlib = 0;
	zone_renderer = 0;

	argc = Cmd_Argc();
	for( block = mainzone->blocklist.next; ; block = block->next ) {
		if( argc != 1 ) {
			Com_Printf( "block:%p    size:%7i    tag:%3i\n", block, block->size, block->tag );
		}

		if( block->tag ) {
			zone_used_size += block->size;
			zone_used_blocks++;
			if( block->tag == TAG_BOTLIB ) {
				zone_botlib += block->size;
			} else if( block->tag == TAG_RENDERER ) {
				zone_renderer += block->size;
			}
		}

		if( block->next == &mainzone->blocklist )
			break;			/* all blocks have been hit */
		if( (byte *)block + block->size != (byte *)block->next )
			Com_Printf( "ERROR: block size does not touch the next block\n" );
		if( block->next->prev != block )
			Com_Printf( "ERROR: next block doesn't have proper back link\n" );
		if( !block->tag && !block->next->tag )
			Com_Printf( "ERROR: two consecutive free blocks\n" );
	}

	for( block = smallzone->blocklist.next; ; block = block->next ) {
		if( block->tag ) {
			smallzone_used += block->size;
		}

		if( block->next == &smallzone->blocklist )
			break;			/* all blocks have been hit */
	}

	Com_Printf( "%8i bytes total hunk\n", hunk_size );
	Com_Printf( "%8i bytes total zone\n", zone_size );
	Com_Printf( "\n" );

	/* low hunk */
	Com_Printf( "%8i low mark\n", hunk_low.mark );
	Com_Printf( "%8i low permanent\n", hunk_low.permanent );

	if( hunk_low.temp != hunk_low.permanent ) {
		Com_Printf( "%8i low temp\n", hunk_low.temp );
	}

	Com_Printf( "%8i low tempHighwater\n", hunk_low.tempHighwater );
	Com_Printf( "\n" );

	/* high hunk */
	Com_Printf( "%8i high mark\n", hunk_high.mark );
	Com_Printf( "%8i high permanent\n", hunk_high.permanent );

	if( hunk_high.temp != hunk_high.permanent ) {
		Com_Printf( "%8i high temp\n", hunk_high.temp );
	}

	Com_Printf( "%8i high tempHighwater\n", hunk_high.tempHighwater );
	Com_Printf( "\n" );

	Com_Printf( "%8i total hunk in use\n", hunk_low.permanent + hunk_high.permanent );

	hunk_unused_highwater = 0;
	if( hunk_low.tempHighwater > hunk_low.permanent ) {
		hunk_unused_highwater = hunk_low.tempHighwater - hunk_low.permanent;
	}
	if( hunk_high.tempHighwater > hunk_high.permanent ) {
		hunk_unused_highwater += hunk_high.tempHighwater - hunk_high.permanent;
	}

	Com_Printf( "%8i unused highwater\n", hunk_unused_highwater );
	Com_Printf( "\n" );
	Com_Printf( "%8i bytes in %i zone blocks\n", zone_used_size, zone_used_blocks );
	Com_Printf( "        %8i bytes in dynamic botlib\n", zone_botlib );
	Com_Printf( "        %8i bytes in dynamic renderer\n", zone_renderer );
	Com_Printf( "        %8i bytes in dynamic other\n", zone_used_size - zone_renderer - zone_botlib );
	Com_Printf( "        %8i bytes in small Zone memory\n", smallzone_used );
}

/*
========================
Hunk_Clear
========================
*/
void Hunk_Clear( void )
{
	Com_Printf( "Hunk_Clear: reset the hunk ok\n" );

	hunk_low.mark = 0;
	hunk_low.permanent = 0;
	hunk_low.temp = 0;
	hunk_low.tempHighwater = 0;

	hunk_high.mark = 0;
	hunk_high.permanent = 0;
	hunk_high.temp = 0;
	hunk_high.tempHighwater = 0;

	hunk_low_mark = &hunk_low;
	hunk_high_mark = &hunk_high;
}

/*
========================
Hunk_Init
========================
*/
void Hunk_Init( void )
{
	int		min_hunkmegs;
	char	*min_message;

	if( FS_LoadStack() ) {
		Com_Error( ERR_FATAL, "Hunk initialization failed. File system load stack not zero" );
	}

	com_hunkmegs = Cvar_Get( "com_hunkmegs", "64", CVAR_ARCHIVE|CVAR_LATCH );

//	if( dedicated && dedicated->integer ) {
	if (0) {
		min_hunkmegs = 1;
		min_message = "Minimum com_hunkmegs for a dedicated server is %i, allocating %i megs.\n";
	} else {
		min_hunkmegs = 64;
		min_message = "Minimum com_hunkmegs is %i, allocating %i megs.\n";
	}

	if( com_hunkmegs->integer < min_hunkmegs ) {
		hunk_size = min_hunkmegs << 20;
		Com_Printf( min_message, min_hunkmegs, hunk_size / 1048576 );
	} else {
		hunk_size = com_hunkmegs->integer << 20;
	}

	hunk_base = malloc( hunk_size + 31 );

	if( !hunk_base ) {
		Com_Error( ERR_FATAL, "Hunk data failed to allocate %i megs", hunk_size / 1048576 );
	}

	hunk_base = (byte *)((int)(hunk_base+31) & ~31);

	Hunk_Clear();

	Cmd_AddCommand( "meminfo", meminfo );
}

/*
===================
Hunk_Alloc
===================
*/
#ifdef HUNK_DEBUG
void *Hunk_AllocDebug( int size, ha_pref preference, char *label, char *file, int line )
#else
void *Hunk_Alloc( int size, ha_pref preference )
#endif
{
	byte	*mem;
	hunk_t	*h;

	if( !hunk_base ) {
		Com_Error( ERR_FATAL, "Hunk_Alloc: Hunk memory system not initialized" );
	}

	if( preference == h_dontcare
		|| (preference == h_high && hunk_low_mark != &hunk_high)
		|| (preference == h_low && hunk_low_mark != &hunk_low) ) {
		if( hunk_high_mark->temp == hunk_high_mark->permanent ) {
			if( hunk_high_mark->tempHighwater - hunk_high_mark->permanent > hunk_low_mark->tempHighwater - hunk_low_mark->permanent ) {
				h = hunk_high_mark;
				hunk_high_mark = hunk_low_mark;
				hunk_low_mark = h;
			}
		}
	}

	size = ((size+31) & ~31);

	if( hunk_low.temp + hunk_high.temp + size > hunk_size ) {
		Com_Error( ERR_DROP, "Hunk_Alloc: failed on %i", size );
	}

	if( hunk_low_mark == &hunk_low ) {
		mem = hunk_base + hunk_low_mark->permanent;
		hunk_low_mark->permanent += size;
	} else {
		hunk_low_mark->permanent += size;
		mem = hunk_base + hunk_size - hunk_low_mark->permanent;
	}

	hunk_low_mark->temp = hunk_low_mark->permanent;

	memset( mem, 0, size );

	return( mem );
}

/*
=================
Hunk_AllocateTempMemory

Return space from the top of the hunk
=================
*/
void	*Hunk_AllocateTempMemory( int size )
{
	byte		*mem;
	hunk_t		*h;
	temphunk_t	*t;

	if( !hunk_base ) {
		mem = Z_Malloc( size );
		return( mem );
	}

	if( hunk_high_mark->temp == hunk_high_mark->permanent ) {
		if( hunk_high_mark->tempHighwater - hunk_high_mark->permanent > hunk_low_mark->tempHighwater - hunk_low_mark->permanent ) {
			h = hunk_high_mark;
			hunk_high_mark = hunk_low_mark;
			hunk_low_mark = h;
		}
	}

	size += sizeof(temphunk_t);

	size = (size+3)&~3;

	if( hunk_low_mark->permanent + hunk_high_mark->temp + size > hunk_size ) {
		Com_Error( ERR_FATAL, "Hunk_AllocateTempMemory: failed on %i", size );
	}

	if( hunk_high_mark == &hunk_low ) {
		mem = hunk_base + hunk_high_mark->temp;
		hunk_high_mark->temp += size;
	} else {
		hunk_high_mark->temp += size;
		mem = hunk_base + hunk_size - hunk_high_mark->temp;
	}

	if( hunk_high_mark->temp > hunk_high_mark->tempHighwater ) {
		hunk_high_mark->tempHighwater = hunk_high_mark->temp;
	}

	t = (temphunk_t *)mem;
	t->magic = TEMP_MAGIC;
	t->size = size;

	return( (void *)(mem+sizeof(*t)) );
}

/*
========================
Hunk_FreeTempMemory
========================
*/
void Hunk_FreeTempMemory( void *memory )
{
	temphunk_t *t;

	if( !hunk_base ) {
		Z_Free( memory );
		return;
	}

	t = (temphunk_t *)((byte *)memory - sizeof(temphunk_t) );

	if( t->magic != TEMP_MAGIC ) {
		Com_Error( ERR_FATAL, "Hunk_FreeTempMemory: bad magic" );
	}

	t->magic = TEMP_MAGIC+1;

	if( hunk_high_mark == &hunk_low ) {
		if( (byte *)t == hunk_base + hunk_high_mark->temp - t->size ) {
			hunk_high_mark->temp -= t->size;
		} else {
			Com_Error( ERR_FATAL, "Hunk_FreeTempMemory: not the final block" );
		}
	} else {
		if( (byte *)t == hunk_base + hunk_size - hunk_high_mark->temp ) {
			hunk_high_mark->temp -= t->size;
		} else {
			Com_Error( ERR_FATAL, "Hunk_FreeTempMemory: not the final block" );
		}
	}
}

/*
========================
Hunk_ClearTempMemory
========================
*/
void Hunk_ClearTempMemory( void )
{
	if( !hunk_base )
		return;

	hunk_high_mark->temp = hunk_high_mark->permanent;
}

/*
========================
Hunk_CheckMark
========================
*/
aboolean Hunk_CheckMark( void )
{
	if( hunk_low.mark || hunk_high.mark )
		return atrue;

	return afalse;
}

/*
========================
Hunk_SetMark
========================
*/
void Hunk_SetMark( void )
{
	hunk_high.mark = hunk_high.permanent;
	hunk_low.mark = hunk_low.permanent;
}

/*
========================
Hunk_ClearToMark
========================
*/
void Hunk_ClearToMark( void )
{
	hunk_low.temp = hunk_low.permanent = hunk_low.mark;
	hunk_high.temp = hunk_high.permanent = hunk_high.mark;
}

/*
========================
Hunk_MemoryRemaining
========================
*/
int Hunk_MemoryRemaining( void )
{
	int lo_mem;
	int hi_mem;

	lo_mem = hunk_low.permanent;
	if( hunk_low.permanent <= hunk_low.temp )
		lo_mem = hunk_low.temp;

	hi_mem = hunk_high.temp;
	if( hunk_high.permanent > hunk_high.temp )
		hi_mem = hunk_high.permanent;
		
	return( hunk_size - hi_mem - lo_mem );
}

/*
========================
Hunk_MarksSet
========================
*/
aboolean Hunk_MarksSet( void )
{
	if( hunk_low.mark || hunk_high.mark )
		return atrue;

	return afalse;
}
