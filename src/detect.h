/***************************************************************************
                          detect.h  -  CPU detection
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
#ifndef DETECT_H_INCLUDED
#define DETECT_H_INCLUDED

#define MAX_CPU	32

typedef struct processor_s {
	aboolean		MMX;
	aboolean		ISSE;
	aboolean		AMD3D;
	aboolean		serial_present;
	unsigned long	family;
	unsigned long	model;
	unsigned long	stepping;
	char			serial_number[30];
	char			vendor[13];
	char			cpu_type[48];
	char			fpu_type[30];
	unsigned int	speed;
} processor_t;

typedef struct processors_info_s {
	unsigned long	num_system_processors;
	unsigned long	num_avail_processors;
	processor_t		processor[MAX_CPU];
	int				OS_ISSE;
	int				OS_ISSE_EXCEPTIONS;
} processors_info_t;

void CPU_detect( processors_info_t *p_info );

#endif
