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
File    : FS_ConfDefault.h
Purpose : File system configuration defaults
--------  END-OF-HEADER  ---------------------------------------------
*/

#ifndef _FS_CONFDEFAULT_H_
#define _FS_CONFDEFAULT_H_

#include "fs_conf.h"

/*********************************************************************
*
*       define global data types
*
**********************************************************************
*/
#ifndef FS_SIZE_T
  #define FS_SIZE_T     unsigned long       /* 32 bit unsigned */
#endif
#ifndef FS_U32
  #define FS_U32        unsigned long       /* 32 bit unsigned */
#endif
#ifndef FS_I32
  #define FS_I32          signed long       /* 32 bit   signed */
#endif
#ifndef FS_U16
  #define FS_U16        unsigned short      /* 16 bit unsigned */
#endif
#ifndef FS_I16
  #define FS_I16          signed short      /* 16 bit   signed */
#endif
#ifndef FS_U8 
  #define FS_U8         unsigned char       /*  8 bit unsigned */
#endif

#ifndef FS_FARCHARPTR
  #define FS_FARCHARPTR char *
#endif

#ifndef FS_IS_LITTLE_ENDIAN
  #define FS_IS_LITTLE_ENDIAN   0
#endif

#if FS_IS_LITTLE_ENDIAN
  #define FS_WRITE_U16(pBuffer, Data)  *(FS_U16*)pBuffer = Data
  #define FS_READ_U16(pBuffer)         (*(FS_U16*)pBuffer)
#else
  #define FS_WRITE_U16(pBuffer, Data)             \
              * (FS_U8*)pBuffer      = Data;      \
              *((FS_U8*)pBuffer + 1) = Data >> 8;
  #define FS_READ_U16(pBuffer)     ((*(FS_U8*)pBuffer)  |  (*((FS_U8*)pBuffer + 1) << 8))
#endif

/*********************************************************************
*
*       Device Driver Support
*
**********************************************************************
*/

#ifndef   FS_USE_SMC_DRIVER
  #define FS_USE_SMC_DRIVER       0    /* SmartMedia card driver       */
#endif
#ifndef   FS_USE_IDE_DRIVER
  #define FS_USE_IDE_DRIVER       0    /* IDE driver                   */
#endif
#ifndef   FS_USE_WINDRIVE_DRIVER
  #define FS_USE_WINDRIVE_DRIVER  0    /* Windows Logical Drive driver */
#endif
#ifndef   FS_USE_RAMDISK_DRIVER
  #define FS_USE_RAMDISK_DRIVER   0    /* RAM Disk driver              */
#endif
#ifndef   FS_USE_MMC_DRIVER
  #define FS_USE_MMC_DRIVER       0    /* MMC/SD card driver           */
#endif
#ifndef   FS_USE_FLASH_DRIVER
  #define FS_USE_FLASH_DRIVER     0    /* Generic flash driver         */
#endif

#ifndef   FS_USE_GENDEV_DRIVER
  #define FS_USE_GENDEV_DRIVER    0   /* Generic device driver         */
#endif

#if (FS_USE_SMC_DRIVER     == 0) && (FS_USE_IDE_DRIVER == 0)        && \
    (FS_USE_RAMDISK_DRIVER == 0) && (FS_USE_MMC_DRIVER == 0)        && \
    (FS_USE_FLASH_DRIVER   == 0) && (FS_USE_WINDRIVE_DRIVER   == 0) && \
    (FS_USE_GENDEV_DRIVER  == 0)
  #error Please define which Medium you want to use
#endif

#if (!defined(_WIN32) && (FS_USE_WINDRIVE_DRIVER))
  #error Windows Logical Drive driver needs Windows API
#endif


/*********************************************************************
*
*       File system defaults
*
**********************************************************************
*/

#ifndef   FS_MAXOPEN
  #define FS_MAXOPEN              1     /* Maximum number of file handles */
#endif

#ifndef   FS_POSIX_DIR_SUPPORT
  #define FS_POSIX_DIR_SUPPORT    1     /* POSIX 1003.1 like directory support */
#endif

#ifndef   FS_DIR_MAXOPEN                /* Maximum number of directory handles */
  #define FS_DIR_MAXOPEN          FS_POSIX_DIR_SUPPORT
#endif

#ifndef   FS_SUPPORT_FAT
  #define FS_SUPPORT_FAT         1   /* Support the FAT file system if enabled */
#endif

#ifndef   FS_SUPPORT_EFS
  #define FS_SUPPORT_EFS         0   /* Support the EFS file system if enabled */
#endif

#ifndef   FS_SUPPORT_MULTIPLE_FS
  #define FS_SUPPORT_MULTIPLE_FS 0   /* Currently, only FAT is supported -> No need to switch it on */
#endif

#ifndef   FS_SUPPORT_BURST           /* Burst support, allows read/write of multiple sectors at a time (speed opt.) */
  #define FS_SUPPORT_BURST (FS_USE_IDE_DRIVER || FS_USE_SMC_DRIVER || FS_USE_WINDRIVE_DRIVER || FS_USE_RAMDISK_DRIVER)
#endif

#ifndef   FS_SUPPORT_VAR_SECTOR_SIZE
  #define FS_SUPPORT_VAR_SECTOR_SIZE 0    /* Support for variabled, non standard sector sizes (!= 512) */
#endif

#ifndef   FS_SUPPORT_FREE_SECTOR
  #define FS_SUPPORT_FREE_SECTOR  FS_USE_FLASH_DRIVER      /* Informs lower layer of unused sectors ... Makes sense only for drivers which use it */
#endif

#if (FS_SUPPORT_EFS==1) && (FS_SUPPORT_FAT==1)
  #error EFS and FAT can not be used simultaneously
#endif

/*********************************************************************
*
*       FAT File System Layer defines
*
*  For each media in your system using FAT, the file system reserves
*  memory to keep required information of the boot sector of that media.
*  FS_MAXDEV is the number of device drivers in your system used
*  with FAT, FS_FAT_MAXUNIT is the maximum number of logical units
*  supported by one of the activated drivers.
*/

/* For compatibility */
#ifdef  FS_FAT_NOFORMAT                 /* 1 disables code for formatting a media */
  #define FS_FAT_SUPPORT_FORMAT  (!FS_FAT_NOFORMAT)
#endif

/* For compatibility */
#ifdef    FS_FAT_NOFAT32                  /* 1 disables FAT32 support */
  #define FS_FAT_SUPPORT_FAT32      (!FS_FAT_NOFAT32)
#endif

#ifndef   FS_FAT_SUPPORT_FORMAT           /* 0 disables code for formatting a media */
  #define FS_FAT_SUPPORT_FORMAT     1
#endif

#ifndef   FS_FAT_SUPPORT_FAT32            /* 0  disables FAT32 support */
  #define FS_FAT_SUPPORT_FAT32      1
#endif
#ifndef   FS_FAT_DISKINFO                 /* 0 disables command FS_CMD_GET_DISKFREE */
  #define FS_FAT_DISKINFO           1
#endif

#ifndef FS_FAT_SUPPORT_LFN                /* 1 enables */
  #define FS_FAT_SUPPORT_LFN        0
#endif

#ifndef   FS_FAT_FWRITE_UPDATE_DIR       /* 1 FS_FWrite modifies directory (default), 0 directory is modified by FS_FClose */
  #define FS_FAT_FWRITE_UPDATE_DIR  0
#endif

#define FS_SEC_SIZE                 512  /* standard value for FAT system. */

#ifndef   FS_MAINTAIN_FAT_COPY
  #define FS_MAINTAIN_FAT_COPY      0
#endif

/*********************************************************************
*
*       EFS File System Layer defines
*
*/
#ifndef   FS_EFS_SUPPORT_FORMAT                 /* 0 disables code for formatting a media */
  #define FS_EFS_SUPPORT_FORMAT     1
#endif

#ifndef   FS_EFS_SUPPORT_DISKINFO                 /* 0 disables command FS_CMD_GET_DISKFREE */
  #define FS_EFS_SUPPORT_DISKINFO   1
#endif

#ifndef   FS_EFS_FWRITE_UPDATE_DIR        /* 1 FS_FWrite modifies directory (default), 0 directory is modified by FS_FClose */
  #define FS_EFS_FWRITE_UPDATE_DIR  0
#endif

/*********************************************************************
*
*       File system support sector access
*/

#ifndef FS_SUPPORT_SEC_ACCESS
  #define  FS_SUPPORT_SEC_ACCESS 1
#endif

/*********************************************************************
*
*       OS Layer 
*
**********************************************************************
*/

#ifndef   FS_OS_UCOS_II
  #define FS_OS_UCOS_II           0    /* 1 = use uC/OS-II */
#endif
#ifndef   FS_OS_EMBOS
  #define FS_OS_EMBOS             0    /* 1 = use embOS */
#endif
#ifndef   FS_OS_WINDOWS
  #define FS_OS_WINDOWS           0    /* 1 = use WINDOWS */
#endif
#if ((FS_OS_WINDOWS==1) && ((FS_OS_UCOS_II==1) || (FS_OS_EMBOS==1)))
  #error You must not use Windows at the same time as uC/OS-II or embOS!
#endif

#ifndef   FS_NO_CLIB
  #define FS_NO_CLIB  0
#endif

/*********************************************************************
*
*       Device driver defaults
*
**********************************************************************
*/



/*********************************************************************
*
*       RAMDISK defines
*
*/
#if FS_USE_RAMDISK_DRIVER
  #ifndef   FS_RAMDISK_MAXUNIT
    #define FS_RAMDISK_MAXUNIT 1
  #endif
#else
  #ifndef   FS_RAMDISK_MAXUNIT
    #define FS_RAMDISK_MAXUNIT 0
  #endif
#endif

/*********************************************************************
*
*       SMC_DRIVER defines
*
*/
#if FS_USE_SMC_DRIVER
  #ifndef FS_SMC_MAXUNIT
    #define FS_SMC_MAXUNIT                    1
  #endif
  #ifndef FS_SMC_HW_SUPPORT_BSYLINE_CHECK
    #define FS_SMC_HW_SUPPORT_BSYLINE_CHECK   1
  #endif
  #ifndef FS_SMC_HW_NEEDS_POLL
    #define FS_SMC_HW_NEEDS_POLL              1
  #endif
#else
  #ifdef FS_SMC_MAXUNIT
    #undef FS_SMC_MAXUNIT
  #endif
  #define FS_SMC_MAXUNIT                      0
#endif /* FS_USE_SMC_DRIVER */


/*********************************************************************
*
*       MMC_DRIVER defines
*
*/
#if FS_USE_MMC_DRIVER
  #ifndef   FS_MMC_MAXUNIT
    #define FS_MMC_MAXUNIT          1
  #endif
  #ifndef   FS_MMC_HW_NEEDS_POLL
    #define FS_MMC_HW_NEEDS_POLL    0
  #endif
  #ifndef   FS__MMC_BLOCKSIZE
    #define FS__MMC_BLOCKSIZE       0x200
  #endif
#else 
  #ifdef   FS_MMC_MAXUNIT
    #undef FS_MMC_MAXUNIT
  #endif
  #define FS_MMC_MAXUNIT            0
#endif /* FS_USE_MMC_DRIVER */


/*********************************************************************
*
*       IDE_DRIVER defines
*
*/
#if FS_USE_IDE_DRIVER
  #ifndef   FS_IDE_MAXUNIT
    #define FS_IDE_MAXUNIT           1
  #endif
  #ifndef   FS_IDE_HW_NEEDS_POLL
    #define FS_IDE_HW_NEEDS_POLL     1  
  #endif
#else 
  #ifdef   FS_IDE_MAXUNIT
    #undef FS_IDE_MAXUNIT
  #endif
  #define FS_IDE_MAXUNIT             0
#endif /* FS_USE_IDE_DRIVER */

/*********************************************************************
*
*       FLASH_DRIVER defines
*/

#if FS_USE_FLASH_DRIVER
  #ifndef FS_FLASH_MAXUNIT
    #define FS_FLASH_MAXUNIT             1
  #endif
  #ifndef FS_FLASH_RAMBUFFER
    #define FS_FLASH_RAMBUFFER      0x4000
  #endif
  #ifndef FS_FLASHWEARLEVELING
    #define FS_FLASHWEARLEVELING         1
  #endif
#else 
  #ifdef FS_FLASH_MAXUNIT
    #undef FS_FLASH_MAXUNIT
  #endif
  #define FS_FLASH_MAXUNIT               0
#endif

/*********************************************************************
*
*       WINDRIVE_DRIVER defines
*
* This driver does allow to use any Windows logical driver on a
*  Windows NT system with the file system. Please be aware, that Win9X
*  is not supported, because it cannot access logical drives with
*  "CreateFile".
*/

#if FS_USE_WINDRIVE_DRIVER
  #ifndef FS_WD_MAXDEV
    #define FS_WD_MAXDEV              2        /* number of windows drives */
  #endif
  #ifndef FS_WD_DEV0NAME
    #define FS_WD_DEV0NAME   "\\\\.\\A:"       /* Windows drive name for "windrv:0:" */
  #endif
  #ifndef FS_WD_DEV1NAME
    #define FS_WD_DEV1NAME   "\\\\.\\B:"       /* Windows drive name for "windrv:1:" */
  #endif
#else 
  #ifdef FS_WD_MAXDEV
    #undef FS_WD_MAXDEV
  #endif
  #define FS_WD_MAXDEV                0

#endif  /* FS_USE_WINDRIVE_DRIVER */


/* In order to avoid warnings for undefined parameters */
#ifndef FS_USE_PARA
  #if defined (__BORLANDC__) || defined(NC30) || defined(NC308)
    #define FS_USE_PARA(para)
  #else
    #define FS_USE_PARA(para) para=para;
  #endif
#endif


/*********************************************************************
*
*       Hardware specific defaults
*/

#ifndef FS_SMC_HW_NEEDS_POLL
  #define FS_SMC_HW_NEEDS_POLL  0
#endif
#ifndef FS_MMC_HW_NEEDS_POLL
  #define FS_MMC_HW_NEEDS_POLL  0
#endif
#ifndef FS_IDE_HW_NEEDS_POLL
  #define FS_IDE_HW_NEEDS_POLL  0  
#endif

#define FS_HW_NEEDS_POLL (FS_SMC_HW_NEEDS_POLL || FS_MMC_HW_NEEDS_POLL || FS_IDE_HW_NEEDS_POLL)


/*********************************************************************
*
*       Number of 
*/
#ifndef   FS_NUM_VOLUMES
  #define FS_NUM_VOLUMES           (FS_USE_SMC_DRIVER      * FS_SMC_MAXUNIT   + \
                                    FS_USE_IDE_DRIVER      * FS_IDE_MAXUNIT   + \
                                    FS_USE_MMC_DRIVER      * FS_MMC_MAXUNIT   + \
                                    FS_USE_IDE_DRIVER      * FS_IDE_MAXUNIT   + \
                                    FS_USE_FLASH_DRIVER    * FS_FLASH_MAXUNIT + \
                                    FS_USE_WINDRIVE_DRIVER * FS_WD_MAXDEV     + \
                                    FS_USE_RAMDISK_DRIVER  * FS_RAMDISK_MAXUNIT  )   /* Note: Can be reduced using the individual defines */
#endif

/********************************************************************/

#endif  /* Avoid multiple inclusion */

/****** End of file *************************************************/
