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
File        : fs_lbl.h 
Purpose     : Header file for the file system's Logical Block Layer
---------------------------END-OF-HEADER------------------------------
*/

#ifndef _FS_LBL_H_
#define _FS_LBL_H_

#include "fs_api.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*             #define constants
*
**********************************************************************
*/

#define FS_LBL_MEDIA_UNCHANGED  0x00
#define FS_LBL_MEDIA_CHANGED    0x01

/*********************************************************************
*
*             Global function prototypes
*
**********************************************************************
*/

void FS_LB_DecBusyCnt (      FS_DEVICE *pDevice);
void FS_LB_IncBusyCnt (      FS_DEVICE *pDevice);
void FS_LB_InitMedium (const FS_DEVICE *pDevice);
int  FS_LB_ioctl      (      FS_DEVICE *pDevice, FS_I32 Cmd, FS_I32 Aux, void *pBuffer);
int  FS_LB_ReadBurst  (const FS_PARTITION *pPart, FS_U32 SectorNo, FS_U32 NumSectors,       void *pBuffer);
int  FS_LB_ReadDevice (const FS_DEVICE *pDevice, FS_U32 Sector, void *pBuffer);
int  FS_LB_ReadPart   (const FS_PARTITION *pPart, FS_U32 Sector, void *pBuffer);
int  FS_LB_WriteBurst (const FS_PARTITION *pPart, FS_U32 SectorNo, FS_U32 NumSectors, const void *pBuffer);
int  FS_LB_WriteDevice(const FS_DEVICE *pDevice, FS_U32 Sector,                       const void *pBuffer);
int  FS_LB_WritePart  (const FS_PARTITION *pPart, FS_U32 Sector,                      const void *pBuffer);
int  FS_LB_GetStatus  (const FS_DEVICE *pDevice);

#if defined(__cplusplus)
}                /* Make sure we have C-declarations in C++ programs */
#endif

#endif  /* _FS_LBL_H_  */

/*************************** End of file ****************************/

