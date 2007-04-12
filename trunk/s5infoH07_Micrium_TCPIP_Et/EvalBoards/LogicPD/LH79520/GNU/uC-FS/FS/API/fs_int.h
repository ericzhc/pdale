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
File        : fs_int.h 
Purpose     : Internals used accross different layers of the file system
---------------------------END-OF-HEADER------------------------------
*/

#ifndef _FS_INT_H_
#define _FS_INT_H_


#include "FS_Debug.h"
#include "FS_ConfDefaults.h"        /* FS Configuration */

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif



/*********************************************************************
*
*             Global data types
*
**********************************************************************
*/

#define FS_COUNTOF(a) (sizeof(a) / sizeof(a[0]))

/* BIOS parameter block (FAT12/FAT16) */
typedef struct {
  FS_U32          NumSectors;       /* RSVD + FAT + ROOT + FATA       */
  FS_U32          FATSize;          /* number of FAT sectors          */
  FS_U32          RootDirPos;       /* Position of root directory. FAT32: Cluster, FAT12/16: Sector */
  FS_U32          BytesPerCluster;
  FS_U16          BytesPerSec;      /* _512_,1024,2048,4096           */
  FS_U16          BytesPerSectorShift; /* 9, 10, 11, 12               */
  FS_U16          RootEntCnt;          /* number of root dir entries     */
  FS_U16          FSInfo;           /* Position of FSInfo structure. (FAT 32 only) */
  FS_U16          RsvdSecCnt;       /* 1 for FAT12 & FAT16            */
  FS_U8           SecPerClus;       /* sec in allocation unit         */
  FS_U8           NumFATs;          /* 2                              */
  FS_U8           FATType;
  FS_U32          NumClusters;
  FS_U32          FirstDataSector;
} FS_FAT_INFO;

typedef struct {
  FS_U32          NumSectors;       /* RSVD + FAT + ROOT + FATA       */
  FS_U32          FATSize;          /* number of FAT sectors          */
  FS_U32          RootClus;         /* root dir clus for FAT32        */
  FS_U16          BytesPerSec;      /* _512_,1024,2048,4096           */
  FS_U16          BytesPerSectorShift; /* 9, 10, 11, 12               */
  FS_U16          RootEntCnt;          /* number of root dir entries     */
  FS_U16          FSInfo;           /* position of FSInfo structure   */
  FS_U16          RsvdSecCnt;       /* 1 for FAT12 & FAT16            */
  FS_U8           SecPerClus;       /* sec in allocation unit         */
  FS_U32          NumClusters;
  FS_U32          FirstDataSector;
  FS_U32          BytesPerCluster;
} FS_EFS_INFO;

typedef struct {
  FS_DEVICE   Device;
  FS_U32      StartSector;
} FS_PARTITION;

typedef struct {
  FS_PARTITION Partition;
  FS_FAT_INFO  FATInfo;
  FS_U8        PartitionIndex;
  FS_U8        IsMounted;
/*FS_FS_TYPE ... To be added with multiple FS support */
} FS_VOLUME;


/* FS Sector Buffer */
typedef struct {
  FS_I32 SectorNo;
  FS_PARTITION * pPart;
  FS_U8 * pBuffer;
  char   IsDirty;
  char   HasError;
} FS_SB;

char FS_SB_Create   (FS_SB * pSB, FS_PARTITION *pPart);
void FS_SB_Delete   (FS_SB * pSB);
void FS_SB_Flush    (FS_SB * pSB);
void FS_SB_MarkDirty(FS_SB * pSB);
void FS_SB_MarkValid(FS_SB * pSB, FS_I32 SectorNo);
char FS_SB_Read     (FS_SB * pSB, FS_I32 SectorNo);
char FS_SB_WriteAt  (FS_SB * pSB, FS_I32 SectorNo);

FS_U8 * FS_AllocSectorBuffer(void);
void    FS_FreeSectorBuffer  (void * p);


#if defined(__cplusplus)
}                /* Make sure we have C-declarations in C++ programs */
#endif


#endif  /* _FS_INT_H_ */

/*************************** End of file ****************************/
