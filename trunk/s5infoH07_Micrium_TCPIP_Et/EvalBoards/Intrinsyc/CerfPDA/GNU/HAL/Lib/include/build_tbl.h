//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2003 Universite de Sherbrooke All rights reserved.
//
// Module name:
//
//      build_tbl.c
//
// Description:
//
//      Build the MMU translation tables.
//
// Author:
//
//      Philippe Mabilleau ing.
//	from mmugen by Peter Maloy, CodeSprite August 2000  www.codesprite.com
//
// Created:
//
//      October 2003
//
////////////////////////////////////////////////////////////////////////////////

#ifndef BUILD_TBL_H
#define BUILD_TBL_H

typedef struct
{
	unsigned long v_base;		/* first address in virtual section		*/
	unsigned long v_end;		/* last address in virtual section		*/
	unsigned long p_base;		/* corresponding physical base address		*/
	unsigned long entrytype; 	/* Entry type					*/
	unsigned long access;		/* Access types					*/
	unsigned long cb;		/* cacheable & bufferable status		*/
} mmudata, * mmudata_ptr;

/* Access types	*/
#define NO_ACCESS	0
#define	SVC_READWRITE	1
#define	NO_USR_WRITE	2
#define	FULL_ACCESS	3

/* Entry types level 1 table */
#define	PAGES		'P'
#define	SECTION		'S'
#define	FAULT		'F'
/* Entry types level 2 table */
#define	LARGEPAGES	'L'
#define	SMALLPAGES	'S'

#define	BUFFERABLE			1
#define	CACHEABLE			2
#define	BUFFERABLE_AND_CACHEABLE	3

int build_tbl(unsigned long tbl_address,
		mmudata_ptr tbl1_array, int tbl1_size,
		mmudata_ptr tbl2_array, int tbl2_size);

#endif //BUILD_TBL_H
