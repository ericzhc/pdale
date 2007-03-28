/*
**********************************************************************
*                          Micrium, Inc.
*                      949 Crestview Circle
*                     Weston,  FL 33327-1848
*
*                            uC/FS
*
*             (c) Copyright 2001 - 2003, Micrium, Inc.
*                      All rights reserved.
*
***********************************************************************

----------------------------------------------------------------------
File        : ide.h
Purpose     : Header file for the file system's IDE driver
----------------------------------------------------------------------
Known problems or limitations with current version
----------------------------------------------------------------------
None.
---------------------------END-OF-HEADER------------------------------
*/

#ifndef __IDE_H__
#define __IDE_H__

/*********************************************************************
*
*             Global function prototypes
*
**********************************************************************
*/

int FS__IDE_Init        (FS_U8 Unit);
int FS__IDE_ReadSectors (FS_U8 Unit, FS_U32 Sector, FS_U32 NumSectors, FS_U8 *pBuffer);
int FS__IDE_WriteSectors(FS_U8 Unit, FS_U32 Sector, FS_U32 NumSectors, FS_U8 *pBuffer);

#endif  /* __IDE_H__ */


/*************************** End of file ****************************/
