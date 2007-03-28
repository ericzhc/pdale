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
File        : fs_os.h 
Purpose     : File system's OS Layer header file
----------------------------------------------------------------------
Known problems or limitations with current version
----------------------------------------------------------------------
None.
---------------------------END-OF-HEADER------------------------------
*/

#ifndef _FS_OS_H_

#define _FS_OS_H_
#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

void     FS_X_OS_LockFileHandle  (void);
void     FS_X_OS_UnlockFileHandle(void);
void     FS_X_OS_LockFileOp      (FS_FILE *pFile);
void     FS_X_OS_UnlockFileOp    (FS_FILE *pFile);
void     FS_X_OS_LockMem         (void);
void     FS_X_OS_UnlockMem       (void);
void     FS_X_OS_LockDeviceOp    (const FS_DEVICE * pDevice);
void     FS_X_OS_UnlockDeviceOp  (const FS_DEVICE * pDevice);
FS_U16   FS_X_OS_GetDate         (void);
FS_U16   FS_X_OS_GetTime         (void);
int      FS_X_OS_Init            (void);
int      FS_X_OS_Exit            (void);

void     FS_X_Warn     (const char *s);
void     FS_X_ErrorOut (const char *s);
void     FS_X_Log      (const char *s);

#if FS_POSIX_DIR_SUPPORT
void     FS_X_OS_LockDirHandle(void);
void     FS_X_OS_UnlockDirHandle(void);
void     FS_X_OS_LockDirOp(FS_DIR *dirp);
void     FS_X_OS_UnlockDirOp(FS_DIR *dirp);

#endif  /* FS_POSIX_DIR_SUPPORT */

#if defined(__cplusplus)
}                /* Make sure we have C-declarations in C++ programs */
#endif

#endif    /*  _FS_OS_H_  */

/*************************** End of file ****************************/
