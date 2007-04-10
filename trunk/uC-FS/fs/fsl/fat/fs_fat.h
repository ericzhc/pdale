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
File        : fs_fat.h
Purpose     : FAT File System Layer header
---------------------------END-OF-HEADER------------------------------
*/

#ifndef _FS_FAT_H_
#define _FS_FAT_H_

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

#include "fs_int.h"
/*********************************************************************
*
*             #define constants
*
**********************************************************************
*/

#define FS_FAT_DENTRY_SIZE      0x20
#define FS_FAT_SEC_SIZE         FS_SEC_SIZE

#define FS_FAT_ATTR_READ_ONLY    FS_ATTR_READ_ONLY
#define FS_FAT_ATTR_HIDDEN       FS_ATTR_HIDDEN
#define FS_FAT_ATTR_SYSTEM       FS_ATTR_SYSTEM
#define FS_FAT_VOLUME_ID         FS_ATTR_VOLUME_ID
#define FS_FAT_ATTR_ARCHIVE      FS_ATTR_ARCHIVE
#define FS_FAT_ATTR_DIRECTORY    FS_ATTR_DIRECTORY
#define FS_FAT_ATTR_LONGNAME     FS_ATTR_READ_ONLY | \
                                 FS_ATTR_HIDDEN |    \
                                 FS_ATTR_SYSTEM |    \
                                 FS_ATTR_VOLUME_ID



#define FS_FAT_TYPE_FAT12       12
#define FS_FAT_TYPE_FAT16       16
#define FS_FAT_TYPE_FAT32       32

/*********************************************************************
*
*             Global data types
*
**********************************************************************
*/



/* FAT directory entry */
typedef struct {
  FS_U8   data[FS_FAT_DENTRY_SIZE];
} FS_FAT_DENTRY;

typedef struct {
  char ac[11];
} FS_83NAME;


/*********************************************************************
*
*             Global function prototypes
*
**********************************************************************
*/

/*********************************************************************
*
*             fat_misc
*/

int         FS__fat_checkunit   (FS_VOLUME * pVolume);

FS_U32      FS_FAT_ClusterId2SectorNo(FS_FAT_INFO * pFATInfo, FS_U32 ClusterId);
FS_U32      FS_FAT_FindFreeCluster   (FS_VOLUME * pVolume, FS_SB * pSB, FS_U32 FirstCluster);
void        FS_FAT_LinkCluster       (FS_VOLUME * pVolume, FS_SB * pSB, FS_U32 LastCluster, FS_U32 NewCluster);
char        FS_FAT_MarkClusterEOC    (FS_VOLUME * pVolume, FS_SB * pSB, FS_U32 Cluster);
FS_U32      FS_FAT_WalkCluster       (FS_VOLUME * pVolume, FS_SB * pSB, FS_U32 StartCluster, FS_U32 NumClusters);
FS_U32      FS_FAT_AllocCluster      (FS_VOLUME * pVolume, FS_SB * pSB, FS_U32 LastCluster);
FS_U32      FS_FAT_ReadFATEntry      (FS_VOLUME * pVolume, FS_SB * pSB, FS_U32 ClusterId);
FS_U32      FS_FAT_FindLastCluster   (FS_VOLUME * pVolume, FS_SB * pSB, FS_U32 ClusterId, FS_U32 * pNumClusters);
FS_U32      FS_FAT_FreeClusterChain  (FS_VOLUME * pVolume, FS_SB * pSB, FS_U32 ClusterId, FS_U32 NumClusters);

FS_FAT_DENTRY * FS_FAT_FindDirEntryShort  (FS_VOLUME * pVolume, FS_SB * pSB, char *pEntryName, int Len, FS_U32 DirStart);
FS_FAT_DENTRY * FS_FAT_FindEmptyDirEntry  (FS_VOLUME * pVolume, FS_SB * pSB, FS_U32 DirStart);
FS_FAT_DENTRY * FS_FAT_GetDirEntry        (FS_VOLUME * pVolume, FS_SB* pSB,  FS_DIR_POS * pDirPos);
void            FS_FAT_IncDirPos          (FS_DIR_POS * pDirPos);

char        FS_FAT_GetFATType          (FS_U32 NumClusters);
void        FS_FAT_WriteDirEntryCluster(FS_FAT_DENTRY* pDirEntry, FS_U32 Cluster);
void        FS_FAT_WriteDirEntry83     (FS_FAT_DENTRY * pDirEntry, const FS_83NAME * pFileName, FS_U16 Time, FS_U16 Date, FS_U32 ClusterId, FS_U8 Attrib);
void        FS_FAT_WriteDirEntryShort  (FS_FAT_DENTRY *pDirEntry, const char * pFileName, FS_U16 Time, FS_U16 Date, FS_U32 ClusterId, FS_U8 Attrib);


#if FS_FAT_SUPPORT_LFN
  #define FS_FAT_FIND_DIR_ENTRY(pVolume, pSB, sName, Len, DirStart, pLongEntryIndex)  FS_FAT_FindDirEntry(pVolume, pSB, sName, Len, DirStart, pLongEntryIndex)
#else
  #define FS_FAT_FIND_DIR_ENTRY(pVolume, pSB, sName, Len, DirStart, pLongEntryIndex)  FS_FAT_FindDirEntryShort(pVolume, pSB, sName, Len, DirStart)
#endif

/*********************************************************************
*
*             fat_in
*/

FS_SIZE_T   FS__fat_fread(void *pData, FS_U32 NumBytes, FS_FILE *pFile);


/*********************************************************************
*
*             fat_out
*/
FS_SIZE_T   FS__fat_fwrite(const void *pData, FS_U32 NumBytes, FS_FILE *pFile);
void        FS__fat_fclose(FS_FILE *pFile);


/*********************************************************************
*
*             fat_open
*/
char            FS__fat_fopen            (const char *pFileName, FS_FILE *pFile, char DoDel, char DoOpen, char DoCreate);
char            FS__fat_findpath         (FS_VOLUME * pVolume, FS_SB * pSB,  const char *pFullName, char * *ppFileName, FS_U32 *pDirStart);
char            FS__fat_DeleteFileOrDir  (FS_VOLUME * pVolume, FS_SB * pSB, FS_FAT_DENTRY  * pDirEntry, FS_U8 IsFile);
FS_U32          FS_FAT_GetFirstCluster   (FS_FAT_DENTRY* pDirEntry);
void            FS_FAT_InitDirEntryScan  (FS_FAT_INFO * pFATInfo, FS_DIR_POS * pDirPos, FS_U32 DirCluster);
char            FS_FAT_Make83Name        (FS_83NAME * pOutName, const char *pOrgName, int Len);
FS_FAT_DENTRY * FS_FAT_CreateDirEntry    (FS_VOLUME * pVolume, FS_SB * pSB, const char * pFileName, FS_U32 DirStart, FS_U8 Attribute);

/*********************************************************************
*
*       FAT Long file name support (LFN)
*/
FS_FAT_DENTRY * FS_FAT_FindDirEntry      (FS_VOLUME * pVolume, FS_SB * pSB, char        *pEntryName, int Len, FS_U32 DirStart, int * pLongDirEntryIndex);
FS_FAT_DENTRY * FS_FAT_FindDirEntryLong  (FS_VOLUME * pVolume, FS_SB * pSB, const char * sLongName,  int Len, FS_U32 DirStart, int * pLongDirEntryIndex);
void            FS_FAT_DelLongEntry      (FS_VOLUME * pVolume, FS_SB * pSB, FS_U32 DirStart, int LongDirEntryIndex);
FS_FAT_DENTRY * FS_FAT_LFN_CreateDirEntry(FS_VOLUME * pVolume, FS_SB * pSB, const char * pFileName, FS_U32 DirStart, FS_U8 Attribute);

int FS_FAT_CalcNumDirEntries(const char * sLongName, int Len);

/*********************************************************************
*
*             fat_ioctl
*/
int         FS__fat_ioctl(FS_VOLUME * pVolume, FS_I32 Cmd, FS_I32 Aux, void *pBuffer);


/*********************************************************************
*
*             fat_dir
*/

#if FS_POSIX_DIR_SUPPORT
  FS_DIR             *FS__fat_opendir (const char *pDirName, FS_DIR *pDir);
  int                 FS__fat_closedir(FS_DIR *pDir);
  FS_DIRENT          *FS__fat_readdir (FS_DIR *pDir);
  int                 FS_FAT_RemoveDir(FS_VOLUME * pVolume, const char *pDirName);
  int                 FS_FAT_CreateDir(FS_VOLUME * pVolume, const char *pDirName);
#endif /* FS_POSIX_DIR_SUPPORT */

#if defined(__cplusplus)
}                /* Make sure we have C-declarations in C++ programs */
#endif

#endif  /* _FS_FAT_H_ */

/*************************** End of file ****************************/
