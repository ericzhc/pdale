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
File        : fs_api.h 
Purpose     : Define global functions and types to be used by an
              application using the file system.

              This file needs to be included by any modul using the
              file system.
---------------------------END-OF-HEADER------------------------------
*/

#ifndef _FS_API_H_
#define _FS_API_H_

/*********************************************************************
*
*             #include Section
*
**********************************************************************
*/

#include "FS_ConfDefaults.h"        /* FS Configuration */
#include "fs_dev.h"
#include "fs_int.h"


#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif


/*********************************************************************
*
*             #define constants
*
**********************************************************************
*/

/* File system version */
#define FS_VERSION                0x00023000UL

/* Maximum size of a directory name */
#define FS_DIRNAME_MAX            255

/* Global error codes */
#define FS_ERR_OK                 0
#define FS_ERR_EOF                -1
#define FS_ERR_DISKFULL           -32
#define FS_ERR_INVALIDPAR         -48
#define FS_ERR_CMDNOTSUPPORTED    -40
#define FS_ERR_WRITEONLY          -64
#define FS_ERR_READONLY           -80
#define FS_ERR_READERROR          -96
#define FS_ERR_WRITEERROR         -112
#define FS_ERR_DISKCHANGED        -128
#define FS_ERR_CLOSE              -144
#define FS_ERR_INVALIDDESC        -160 /* Invalid media descriptor, media corrupt */
#define FS_ERR_DEVICETIMEOUT      -176 /* Device did not respond or timed out */
#define FS_ERR_NOMEDIAPRESENT     -192 /* No media in device present */

/* Global constants*/
#define FS_SEEK_CUR               1
#define FS_SEEK_END               2
#define FS_SEEK_SET               0

/* I/O commands */
#define FS_CMD_FLUSH_CACHE        1000L
#define FS_CMD_CHK_DSKCHANGE      1010L
#define FS_CMD_READ_SECTOR        1100L
#define FS_CMD_WRITE_SECTOR       1110L
#define FS_CMD_FORMAT_MEDIA       2222L
#define FS_CMD_FORMAT_AUTO        2333L
#define FS_CMD_FORMAT_EX          2334L
#define FS_CMD_FORMAT_LOW_LEVEL   9001L
#define FS_CMD_REQUIRES_FORMAT    9002L
#define FS_CMD_INC_BUSYCNT        3001L
#define FS_CMD_DEC_BUSYCNT        3002L
#define FS_CMD_GET_DISKFREE       4000L
#define FS_CMD_GET_TOTAL_SPACE    4001L
#define FS_CMD_GET_DEVINFO        4011L
#define FS_CMD_FREE_SECTORS       4012L

#define FS_CMD_FLASH_ERASE_CHIP   FS_CMD_FORMAT_LOW_LEVEL  /* Obsolete, do no use any more (name is confusing) */

/* known media types */
#define FS_MEDIA_SMC_1MB          1010L
#define FS_MEDIA_SMC_2MB          1020L
#define FS_MEDIA_SMC_4MB          1030L
#define FS_MEDIA_SMC_8MB          1040L
#define FS_MEDIA_SMC_16MB         1050L
#define FS_MEDIA_SMC_32MB         1060L
#define FS_MEDIA_SMC_64MB         1070L
#define FS_MEDIA_SMC_128MB        1080L
#define FS_MEDIA_SMC_256MB        1090L

#define FS_MEDIA_RAM_16KB         2010L
#define FS_MEDIA_RAM_64KB         2020L
#define FS_MEDIA_RAM_128KB        2030L
#define FS_MEDIA_RAM_256KB        2040L
#define FS_MEDIA_RAM_512KB        2050L

#define FS_MEDIA_FD_144MB         3040L

#define FS_MEDIA_MMC_32MB         4060L
#define FS_MEDIA_MMC_64MB         4070L
#define FS_MEDIA_MMC_128MB        4080L

#define FS_MEDIA_SD_16MB          5050L
#define FS_MEDIA_SD_64MB          5070L
#define FS_MEDIA_SD_128MB         5080L
#define FS_MEDIA_SD_256MB         5090L

#define FS_MEDIA_CF_32MB          6060L
#define FS_MEDIA_CF_64MB          6070L

/* device states */
#define FS_MEDIA_ISNOTPRESENT     0 
#define FS_MEDIA_ISPRESENT        1
#define FS_MEDIA_STATEUNKNOWN     2

/* Field information for BPB */
#define _FS_BPB_TOTALSEC16      0x013
#define _FS_BPB_SECPERTRK       0x018
#define _FS_BPB_HIDDENSEC       0x01C
#define _FS_BPB_HEADNUM         0x01A
#define _FS_BPB_TOTALSEC32      0x020

/* partition table definitions */
#define FS__DEV_PART_0_TABLE       0x01BE /* start of first entry */

/* attribute definitions */
#define FS_ATTR_READ_ONLY    0x01
#define FS_ATTR_HIDDEN       0x02
#define FS_ATTR_SYSTEM       0x04
#define FS_ATTR_VOLUME_ID    0x08
#define FS_ATTR_ARCHIVE      0x20
#define FS_ATTR_DIRECTORY    0x10

/*********************************************************************
*
*             Global data types
*
**********************************************************************
*/

typedef struct { /* Describes the file handle structure */
  FS_U32        FirstCluster;   /* First cluster used for file  */
  FS_U32        CurClusterFile; /* Current cluster within the file. First cluster is 0, next cluster is 1 ... */
  FS_U32        CurClusterAbs;  /* Current cluster on the medium. This needs to be computed from file cluster, consulting the FAT */
  FS_I32        FilePos;        /* current position in file     */
  FS_I32        size;           /* size of file                 */
  FS_U32        DirEntrySector; /* Sector of directory */
  FS_U16        DirEntryIndex;  /* Index of directory entry */
  FS_VOLUME *   pVolume;
  FS_I16        Error;          /* error code                   */
  FS_U8         InUse;          /* handle in use mark           */
  FS_U8         AccessFlags;
} FS_FILE;

#define FS_FILE_ACCESS_FLAG_A (1 << 0)
#define FS_FILE_ACCESS_FLAG_B (1 << 1)
#define FS_FILE_ACCESS_FLAG_C (1 << 2)
#define FS_FILE_ACCESS_FLAG_R (1 << 3)
#define FS_FILE_ACCESS_FLAG_W (1 << 4)

/* Combined access modes which are frequently used */
#define FS_FILE_ACCESS_FLAGS_AW     (FS_FILE_ACCESS_FLAG_A | FS_FILE_ACCESS_FLAG_W)
#define FS_FILE_ACCESS_FLAGS_BR     (FS_FILE_ACCESS_FLAG_B | FS_FILE_ACCESS_FLAG_R)
#define FS_FILE_ACCESS_FLAGS_CW     (FS_FILE_ACCESS_FLAG_C | FS_FILE_ACCESS_FLAG_W)
#define FS_FILE_ACCESS_FLAGS_RW     (FS_FILE_ACCESS_FLAG_R | FS_FILE_ACCESS_FLAG_W)

#define FS_FILE_ACCESS_FLAGS_ACW    (FS_FILE_ACCESS_FLAG_C | FS_FILE_ACCESS_FLAGS_AW)
#define FS_FILE_ACCESS_FLAGS_ARW    (FS_FILE_ACCESS_FLAG_A | FS_FILE_ACCESS_FLAGS_RW)
#define FS_FILE_ACCESS_FLAGS_BCW    (FS_FILE_ACCESS_FLAG_B | FS_FILE_ACCESS_FLAGS_CW)
#define FS_FILE_ACCESS_FLAGS_BRW    (FS_FILE_ACCESS_FLAG_W | FS_FILE_ACCESS_FLAGS_BR)
#define FS_FILE_ACCESS_FLAGS_CRW    (FS_FILE_ACCESS_FLAG_C | FS_FILE_ACCESS_FLAG_R | FS_FILE_ACCESS_FLAG_W)

#define FS_FILE_ACCESS_FLAGS_ABCW   (FS_FILE_ACCESS_FLAG_B | FS_FILE_ACCESS_FLAGS_ACW)
#define FS_FILE_ACCESS_FLAGS_ACRW   (FS_FILE_ACCESS_FLAG_A | FS_FILE_ACCESS_FLAGS_CRW)
#define FS_FILE_ACCESS_FLAGS_BCRW   (FS_FILE_ACCESS_FLAG_B | FS_FILE_ACCESS_FLAGS_CRW)

#define FS_FILE_ACCESS_FLAGS_ABCRW  (FS_FILE_ACCESS_FLAGS_ACRW | FS_FILE_ACCESS_FLAG_A)


typedef struct {
  FS_U32 total_clusters;
  FS_U32 avail_clusters;
  FS_U16 sectors_per_cluster;
  FS_U16 bytes_per_sector;
} FS_DISKFREE_T;

typedef struct {
  FS_U16 NumHeads;          /* Relevant only for mechanical drives   */
  FS_U16 SectorsPerTrack;   /* Relevant only for mechanical drives   */
  FS_U32 NumSectors;        /* Total number of sectors on the medium */
} FS_DEV_INFO;

typedef struct {
  FS_U16 SectorsPerCluster;
  FS_U16 NumRootDirEntries;    /* Proposed, actual value depends on FATType */
  FS_DEV_INFO * pDevInfo;
} FS_FORMAT_INFO;

typedef struct {
  FS_U32 Cluster;                    /* Cluster of current sector */
  FS_U32 DirEntryIndex;              /* Directory entry index (first directory entry has index 0 */
} FS_DIR_POS;

    
/*********************************************************************
*
*             Non blocking (backgrounded) file I/O functions
*/

typedef struct FS_BG_DATA {
  struct FS_BG_DATA* pNext;
  FS_U32    NumBytes;
  FS_U32    NumBytesRem;     /* Remaining bytes to transfer */
  void    * pData;
  FS_FILE * pFile;
  FS_U8     Operation;  
  void (*pfOnCompletion) (void * pData);   /* Optional completion routine */
  void * pCompletionData;                  /* Optional data for completion routine */
  char IsCompleted;
} FS_BG_DATA;  /* Information for background data */

void FS_Daemon(void);
void FS_FReadNonBlock (void       *pData,
                        FS_U32 NumBytes,
                        FS_FILE *pFile,
                        FS_BG_DATA * pBGData,                    /* User supplied management block */
                        void (*pfOnCompletion) (void * pData),   /* Optional completion routine */
                        void * pCompletionData                   /* Optional data for completion routine */
                        );
void FS_FWriteNonBlock(const void *pData,
                       FS_U32 NumBytes,
                       FS_FILE *pFile,
                       FS_BG_DATA * pBGData,                    /* User supplied management block */
                       void (*pfOnCompletion) (void * pData),   /* Optional completion routine */
                       void * pCompletionData                   /* Optional data for completion routine */
                       );
char FS_IsCompleted(FS_BG_DATA * pBGData);

/*********************************************************************
*
*             directory types
*/

#if FS_POSIX_DIR_SUPPORT

#define FS_ino_t  int

typedef struct FS_DIRENT {
  FS_ino_t d_ino;                      /* to be POSIX conform */
  char     d_name[FS_DIRNAME_MAX];
  FS_U8    FAT_DirAttr;                /* FAT only. Contains the "DIR_Attr" field of an entry. */
  FS_U16   FAT_Time;
  FS_U16   FAT_Date;
  FS_U32   FAT_Size;
} FS_DIRENT;



typedef struct FS_DIR {
  FS_DIRENT      dirent;          /* current directory entry      */
  FS_DIR_POS     DirPos;             /* current position in file     */
  FS_U16         DirEntryIndex;
  FS_U32         FirstCluster;
  FS_VOLUME *    pVolume;
  FS_I16         error;              /* error code                   */
  FS_U8          InUse;              /* handle in use mark           */
} FS_DIR;

#endif  /* FS_POSIX_DIR_SUPPORT */


/*********************************************************************
*
*             Global function prototypes
*
**********************************************************************
*/

/*********************************************************************
*
*             STD file I/O functions
*/

FS_FILE          *FS_FOpen(const char *pFileName, const char *pMode);
void             FS_FClose(FS_FILE *pFile);
FS_SIZE_T        FS_FRead(void *pData, FS_SIZE_T Size, FS_SIZE_T N, FS_FILE *pFile);
FS_SIZE_T        FS_FWrite(const void *pData, FS_SIZE_T Size, FS_SIZE_T N, FS_FILE *pFile);


/*********************************************************************
*
*             file pointer handling
*/

int              FS_FSeek(FS_FILE *pFile, FS_I32 Offset, int Origin);
FS_I32           FS_FTell(FS_FILE *pFile);


/*********************************************************************
*
*             I/O error handling
*/

FS_I16           FS_FError(FS_FILE *pFile);
void             FS_ClearErr(FS_FILE *pFile);
const char *     FS_ErrorNo2Text(int ErrCode);

/*********************************************************************
*
*             file functions
*/

int              FS_Remove(const char *pFileName);
FS_U32           FS_GetFileSize(FS_FILE *pFile);


/*********************************************************************
*
*             IOCTL
*/

int              FS_IoCtl(const char *pDevName, FS_I32 Cmd, FS_I32 Aux, void *pBuffer);



/*********************************************************************
*
*             directory functions
*/

#if FS_POSIX_DIR_SUPPORT

void             FS_DirEnt2Name(FS_DIRENT *pDirEnt, char* pBuffer);
void             FS_DirEnt2Attr(FS_DIRENT *pDirEnt, FS_U8* pAttr);
FS_U32           FS_DirEnt2Size(FS_DIRENT *pDirEnt);
FS_U32           FS_GetNumFiles(FS_DIR *pDir);
FS_DIR           *FS_OpenDir(const char *pDirName);
int              FS_CloseDir(FS_DIR *pDir);
FS_DIRENT        *FS_ReadDir(FS_DIR *pDir);
void             FS_RewindDir(FS_DIR *pDir);
int              FS_MkDir(const char *pDirName);
int              FS_RmDir(const char *pDirName);

#endif  /* FS_POSIX_DIR_SUPPORT */


/*********************************************************************
*
*             file system control functions
*/
FS_VOLUME *      FS_AddDevice(const FS_DEVICE_TYPE * pDevType);
void             FS_UnmountDevice(const FS_DEVICE * pDevice);
int              FS_Init(void);
int              FS_Exit(void);

/*********************************************************************
*
*       Formatting
*/
int              FS_Format   (const char *pDevice, FS_FORMAT_INFO * pFormatInfo);
int              FS_FormatLow(const char *pDevice);

/*********************************************************************
*
*       Partition related
*/
FS_U32           FS_GetMediaStartSec(const FS_U8* pBuffer);
signed char      FS_LocatePartition(FS_VOLUME * pVolume);

/*********************************************************************
*
*             Space information functions
*/
#if (FS_FAT_DISKINFO)
FS_U32           FS_GetFreeSpace (const char *pDevName, FS_U8 DevIndex);
FS_U32           FS_GetTotalSpace(const char *pDevName, FS_U8 DevIndex);
#endif
/*********************************************************************
*
*             endian translation functions
*/
FS_U32           FS_ReadU32(const FS_U8 *pBuffer);
FS_U16           FS_ReadU16(const FS_U8 *pBuffer);
void             FS_WriteU32(     FS_U8 *pBuffer, FS_U32 Data);
void             FS_WriteU16(     FS_U8 *pBuffer, unsigned Data);

/*********************************************************************
*
*             Debug support
*/
void FS_X_Log     (const char *s);
void FS_X_Warn    (const char *s);
void FS_X_ErrorOut(const char *s);

/*********************************************************************
*
*             Timing
*/
void FS_X_Delay(int Ticks);

/*********************************************************************
*
*             FLASH driver specific
*/
typedef struct {
  FS_U32 Off;
  FS_U32 Size;
  FS_U32 EraseCnt;
  FS_U16 NumUsedSectors;
  FS_U16 NumFreeSectors;
  FS_U16 NumEraseableSectors;
} FS_FLASH_SECTOR_INFO;

typedef struct {
  FS_U32 NumPhysSectors;
  FS_U32 NumLogSectors;
  FS_U32 NumUsedSectors;   /* Number of used logical sectors */
} FS_FLASH_DISK_INFO;

void FS_FLASH_GetDiskInfo  (FS_U8 Unit, FS_FLASH_DISK_INFO * pDiskInfo);
void FS_FLASH_GetSectorInfo(FS_U8 Unit, FS_U32 PhysSectorIndex, FS_FLASH_SECTOR_INFO * pSectorInfo);


#if defined(__cplusplus)
  }              /* Make sure we have C-declarations in C++ programs */
#endif

#endif  /* _FS_API_H_ */

/*************************** End of file ****************************/
