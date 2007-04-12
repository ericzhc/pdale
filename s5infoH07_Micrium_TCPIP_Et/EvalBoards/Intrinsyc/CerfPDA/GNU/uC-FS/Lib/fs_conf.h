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
File        : fs_conf.h
Purpose     : File system configuration
---------------------------END-OF-HEADER------------------------------
*/

#ifndef _FS_CONF_H_
#define _FS_CONF_H_


#ifdef WIN32
  #define FS_OS_WINDOWS               1
#endif

#define FS_OS_UCOS_II                 1
#define FS_FAT_SUPPORT_LFN            0


/*********************************************************************
*
*       Specify which driver do you want
*/
#define FS_USE_RAMDISK_DRIVER         1

#define FS_USE_FLASH_DRIVER           0 /* 1 = use NOR flash device       */
#define FS_USE_GENDEV_DRIVER          0 /* 1 = use generic dummy device   */
#define FS_USE_IDE_DRIVER             0 /* 1 = use IDE/CF devices         */
#define FS_USE_MMC_DRIVER             0 /* 1 = use MMC/SD devices         */
#define FS_USE_SMC_DRIVER             0 /* 1 = use SMC/NAND flash devices */


/*********************************************************************
*
*        Setup Debug Level
*/
#define FS_DEBUG_LEVEL                1

/*********************************************************************
*
*             RAMDISK_DRIVER defines
*/

#if FS_USE_RAMDISK_DRIVER
  /*
   * Define size of your RAM disk here.
   * You specify the number of sectors (512 bytes) here.
   */
  #define FS_RR_BLOCKNUM           32768       /* 16MB RAM */
#endif

#endif  /* Avoid multiple inclusion */
