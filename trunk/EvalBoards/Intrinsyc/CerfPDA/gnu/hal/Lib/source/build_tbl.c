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

#include "build_tbl.h"

struct {
    mmudata rule ;
    unsigned long base_address ;
    unsigned long table_entry ;
    long state ;
    long pagescount ;
    unsigned long *tbl_ptr ;
} global ;


//--function prototypes------------------------------------------------------

int  WriteTableEntries (void);
int  WriteLevel1(void);
int  WriteLevel2(void);

//--BUILD_TBL----------------------------------------------------------------


int build_tbl(unsigned long tbl_address,
		mmudata_ptr tbl1_array, int tbl1_size,
		mmudata_ptr tbl2_array, int tbl2_size)
{
    int i;

    global.state=0;
    global.tbl_ptr = (unsigned long *) tbl_address ;
    global.base_address = tbl_address ;

    global.state=1;
    for(i=0; i<tbl1_size; i++) {
	global.rule = *(tbl1_array++) ;
	if(WriteTableEntries()==-1) return -(10 + i);
    }

/* check table_entry is a sensible value */
	if(global.table_entry != 0 && global.table_entry != 0x1000) {
//		printf("Table data error - at end of Level 1, the table was %08lx bytes long (should be 0x4000)\n",global.table_entry<<2);
		return -3;
	}

    global.state=2;
    for(i=0; i<tbl2_size; i++) {
	global.rule = *(tbl2_array++) ;
	if(WriteTableEntries()==-1) return -(20 + i);
    }

//	printf("MMU table generation complete\n");

	return 0 ;
}

//--WRITETABLEENTRIES--------------------------------------------------------

int WriteTableEntries (void)
{
	switch( global.state )
	{
		case 1: return WriteLevel1();
		case 2: return WriteLevel2();
		default: return -1;
	}
}

//--WRITELEVEL1--------------------------------------------------------------

int WriteLevel1(void)
{
	unsigned long fields ;
	unsigned long physical ;
    unsigned long writevalue ;
	unsigned long i ;

	/* Check the table-entry number matches the virtual base address for this section */
	if( global.table_entry != (global.rule.v_base>>20) )
	{
//		printf("MMU Table synchronisation error!\n  table_entry = %08lx, v_base = %08lx\n",
//				global.table_entry, global.rule.v_base);
		return -1 ;
	}

	if (global.rule.entrytype == 'S' || global.rule.entrytype == 'F')
	{
		if(global.rule.entrytype == 'S')
		{
			fields = 0x12 | (global.rule.access<<10) | (global.rule.cb<<2) ;
		}
		else
		{
			fields = 0; //generate fault on access
		}
		physical = global.rule.p_base ;

		/* generate all the SECTION entries for this descriptor */
		for (i = global.rule.v_base; i<global.rule.v_end; )
		{
            writevalue = physical|fields ;
			* (global.tbl_ptr++) = writevalue ;
			global.table_entry++ ;
			physical += 0x100000 ;

			/* check for memory wrap-round */
			i+=0x100000 ;
			if (i==0x00000000) break;
		}

	}
	else
	{
		/* Single PAGE entry */
        writevalue = 0x11 + global.base_address + 0x4000 + 0x400*global.pagescount++ ;
		* (global.tbl_ptr++) = writevalue ;

		global.table_entry++ ;
	}

	return 0 ;

}


//--WRITELEVEL2--------------------------------------------------------------

int WriteLevel2(void)
{
	unsigned long fields ;
	unsigned long physical ;
    unsigned long writevalue ;
	unsigned long i ;
	
	/* work out repeated data fields - note: this line would change if per-page access levels were to be supported*/
	fields = (global.rule.access << 10)|(global.rule.access << 8)|(global.rule.access << 6)|(global.rule.access << 4)|(global.rule.cb <<2);

	if (global.rule.entrytype == 'L')
	{
		fields |= 1 ;
	}
	else
	{
		fields |= 2 ;
	}

	/* create entries for this descriptor */
	i = global.rule.v_base ;
	physical = global.rule.p_base ;

	while(i<global.rule.v_end)
	{
		if( global.rule.entrytype == 'L' )
		{
			int j ;
            writevalue = physical|fields ;

			for( j=16; j; j-- )
			{
				* (global.tbl_ptr++) = writevalue ;
			}
			physical += 0x10000;
			i += 0x10000;
		}
		else
		{
            writevalue = physical|fields ;
			* (global.tbl_ptr++) = writevalue ;
			physical += 0x1000;
			i += 0x1000;
		}

	}

    return 0 ;

}
