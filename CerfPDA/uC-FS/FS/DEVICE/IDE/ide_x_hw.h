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
File        : ide_x_hw.h
Purpose     : IDE hardware layer for Segger SED137XE board accessing
              a CF card in true IDE mode with P7,P8,P9,P10.
----------------------------------------------------------------------
Known problems or limitations with current version
----------------------------------------------------------------------
None.
---------------------------END-OF-HEADER------------------------------
*/

#ifndef __IDE_X_HW_H__
#define __IDE_X_HW_H__

/*********************************************************************
*
*             Global function prototypes
*
**********************************************************************
*/

/* Control line functions */
void FS_IDE_HW_X_BusyLedOff(FS_U8 Unit);
void FS_IDE_HW_X_BusyLedOn (FS_U8 Unit);
void FS_IDE_HW_X_HWReset   (FS_U8 Unit);

/* ATA I/O register access functions */
FS_U8  FS_IDE_HW_X_GetAltStatus  (FS_U8 Unit);
FS_U8  FS_IDE_HW_X_GetCylHigh    (FS_U8 Unit);
FS_U8  FS_IDE_HW_X_GetCylLow     (FS_U8 Unit);
FS_U16 FS_IDE_HW_X_GetData       (FS_U8 Unit);
FS_U8  FS_IDE_HW_X_GetDevice     (FS_U8 Unit);
FS_U8  FS_IDE_HW_X_GetError      (FS_U8 Unit);
FS_U8  FS_IDE_HW_X_GetSectorCount(FS_U8 Unit);
FS_U8  FS_IDE_HW_X_GetSectorNo   (FS_U8 Unit);
FS_U8  FS_IDE_HW_X_GetStatus     (FS_U8 Unit);

void FS_IDE_HW_X_SetCommand    (FS_U8 Unit, FS_U8  Data);
void FS_IDE_HW_X_SetCylHigh    (FS_U8 Unit, FS_U8  Data);
void FS_IDE_HW_X_SetCylLow     (FS_U8 Unit, FS_U8  Data);
void FS_IDE_HW_X_SetData       (FS_U8 Unit, FS_U16 Data);
void FS_IDE_HW_X_SetDevControl (FS_U8 Unit, FS_U8  Data);
void FS_IDE_HW_X_SetDevice     (FS_U8 Unit, FS_U8  Data);
void FS_IDE_HW_X_SetFeatures   (FS_U8 Unit, FS_U8  Data);
void FS_IDE_HW_X_SetSectorCount(FS_U8 Unit, FS_U8  Data);
void FS_IDE_HW_X_SetSectorNo   (FS_U8 Unit, FS_U8  Data);

/* Status detection functions */
char FS_IDE_HW_X_DetectStatus(FS_U8 Unit);

#endif  /* __IDE_X_HW_H__ */

/*************************** End of file ****************************/
