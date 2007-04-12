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
File        : FS_Debug.h
Purpose     : Debug macros
----------------------------------------------------------------------
   Debug macros for logging

 In the FS Simulation, all output is transferred into the log window.
*/

#ifndef FS_DEBUG_H
#define FS_DEBUG_H


#define FS_DEBUG_LEVEL_NOCHECK       0  /* No run time checks are performed */
#define FS_DEBUG_LEVEL_CHECK_PARA    1  /* Parameter checks are performed to avoid crashes */
#define FS_DEBUG_LEVEL_CHECK_ALL     2  /* Parameter checks and consistency checks are performed */
#define FS_DEBUG_LEVEL_LOG_ERRORS    3  /* Errors are recorded */
#define FS_DEBUG_LEVEL_LOG_WARNINGS  4  /* Errors & Warnings are recorded */
#define FS_DEBUG_LEVEL_LOG_ALL       5  /* Errors, Warnings and Messages are recorded. */

#ifndef FS_DEBUG_LEVEL
  #ifdef WIN32
    #define FS_DEBUG_LEVEL FS_DEBUG_LEVEL_LOG_WARNINGS  /* Simulation should log all warnings */
  #else
    #define FS_DEBUG_LEVEL FS_DEBUG_LEVEL_CHECK_PARA  /* For most targets, min. size is important */
  #endif
#endif

/*********************************************************************
*
*       Logging (for debugging primarily)
*
**********************************************************************
*/
void FS_Log      (const char *s);
void FS_Log1     (const char *s, int p0);
void FS_Log2     (const char *s, int p0, int p1);
void FS_Log3     (const char *s, int p0, int p1, int p2);
void FS_Log4     (const char *s, int p0, int p1, int p2,int p3);
void FS_Warn     (const char *s);
void FS_Warn1    (const char *s, int p0);
void FS_Warn2    (const char *s, int p0, int p1);
void FS_Warn3    (const char *s, int p0, int p1, int p2);
void FS_Warn4    (const char *s, int p0, int p1, int p2, int p3);
void FS_ErrorOut (const char *s);
void FS_ErrorOut1(const char *s, int p0);
void FS_ErrorOut2(const char *s, int p0, int p1);
void FS_ErrorOut3(const char *s, int p0, int p1, int p2);
void FS_ErrorOut4(const char *s, int p0, int p1, int p2, int p3);

/*******************************************************************
*
*               Commandline
*
********************************************************************
*/

#ifdef WIN32
  #define FS_DEBUG_GETCMDLINE() SIM_GetCmdLine()
#else
  #define FS_DEBUG_GETCMDLINE() 0
#endif

/*******************************************************************
*
*               Error macros
*
********************************************************************
*/

/* Make sure the macros are actually defined */

#if FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ERRORS
  #define FS_DEBUG_ERROROUT(s)              FS_ErrorOut(s)
  #define FS_DEBUG_ERROROUT1(s,p0)          FS_ErrorOut1(s,p0)
  #define FS_DEBUG_ERROROUT2(s,p0,p1)       FS_ErrorOut2(s,p0,p1)
  #define FS_DEBUG_ERROROUT3(s,p0,p1,p2)    FS_ErrorOut3(s,p0,p1,p2)
  #define FS_DEBUG_ERROROUT4(s,p0,p1,p2,p3) FS_ErrorOut4(s,p0,p1,p2,p3)
  #define FS_DEBUG_ERROROUT_IF(exp,s)              { if (exp) FS_DEBUG_ERROROUT(s); }
  #define FS_DEBUG_ERROROUT1_IF(exp,s,p0)          { if (exp) FS_DEBUG_ERROROUT1(s,p0); }
  #define FS_DEBUG_ERROROUT2_IF(exp,s,p0,p1)       { if (exp) FS_DEBUG_ERROROUT2(s,p0,p1); }
  #define FS_DEBUG_ERROROUT3_IF(exp,s,p0,p1,p2)    { if (exp) FS_DEBUG_ERROROUT3(s,p0,p1,p2); }
  #define FS_DEBUG_ERROROUT4_IF(exp,s,p0,p1,p2,p3) { if (exp) FS_DEBUG_ERROROUT4(s,p0,p1,p2,p3); }
#else
  #define FS_DEBUG_ERROROUT(s)
  #define FS_DEBUG_ERROROUT1(s,p0)
  #define FS_DEBUG_ERROROUT2(s,p0,p1)
  #define FS_DEBUG_ERROROUT3(s,p0,p1,p2)
  #define FS_DEBUG_ERROROUT4(s,p0,p1,p2,p3)
  #define FS_DEBUG_ERROROUT_IF(exp,s)
  #define FS_DEBUG_ERROROUT1_IF(exp,s,p0)
  #define FS_DEBUG_ERROROUT2_IF(exp,s,p0,p1)
  #define FS_DEBUG_ERROROUT3_IF(exp,s,p0,p1,p2)
  #define FS_DEBUG_ERROROUT4_IF(exp,s,p0,p1,p2,p3)
#endif

/*******************************************************************
*
*               Warning macros
*
********************************************************************
*/

/* Make sure the macros are actually defined */

#if FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_WARNINGS
  #define FS_DEBUG_WARN(s)              FS_Warn(s)
  #define FS_DEBUG_WARN1(s,p0)          FS_Warn1(s,p0)
  #define FS_DEBUG_WARN2(s,p0,p1)       FS_Warn2(s,p0,p1)
  #define FS_DEBUG_WARN3(s,p0,p1,p2)    FS_Warn3(s,p0,p1,p2)
  #define FS_DEBUG_WARN4(s,p0,p1,p2,p3) FS_Warn4(s,p0,p1,p2,p3)
  #define FS_DEBUG_WARN_IF(exp,s)                  { if (exp) FS_DEBUG_WARN(s); }
  #define FS_DEBUG_WARN1_IF(exp,s,p0)              { if (exp) FS_DEBUG_WARN1(s,p0); }
  #define FS_DEBUG_WARN2_IF(exp,s,p0,p1)           { if (exp) FS_DEBUG_WARN2(s,p0,p1); }
  #define FS_DEBUG_WARN3_IF(exp,s,p0,p1,p2)        { if (exp) FS_DEBUG_WARN3(s,p0,p1,p2); }
  #define FS_DEBUG_WARN4_IF(exp,s,p0,p1,p2,p3)     { if (exp) FS_DEBUG_WARN4(s,p0,p1,p2,p3); }
#else
  #define FS_DEBUG_WARN(s)
  #define FS_DEBUG_WARN1(s,p0)
  #define FS_DEBUG_WARN2(s,p0,p1)
  #define FS_DEBUG_WARN3(s,p0,p1,p2)
  #define FS_DEBUG_WARN4(s,p0,p1,p2,p3)
  #define FS_DEBUG_WARN_IF(exp,s)
  #define FS_DEBUG_WARN1_IF(exp,s,p0)
  #define FS_DEBUG_WARN2_IF(exp,s,p0,p1)
  #define FS_DEBUG_WARN3_IF(exp,s,p0,p1,p2)
  #define FS_DEBUG_WARN4_IF(exp,s,p0,p1,p2,p3)
#endif

/*******************************************************************
*
*               Logging macros
*
********************************************************************
*/
/* Make sure the macros are actually defined */

#if FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ALL
  #define FS_DEBUG_LOG(s)              FS_Log(s)
  #define FS_DEBUG_LOG1(s,p0)          FS_Warn1(s,p0)
  #define FS_DEBUG_LOG2(s,p0,p1)       FS_Warn2(s,p0,p1)
  #define FS_DEBUG_LOG3(s,p0,p1,p2)    FS_Warn3(s,p0,p1,p2)
  #define FS_DEBUG_LOG4(s,p0,p1,p2,p3) FS_Warn4(s,p0,p1,p2,p3)
  #define FS_DEBUG_LOG_IF(exp,s)                   { if (exp) FS_DEBUG_LOG(s); }
  #define FS_DEBUG_LOG1_IF(exp,s,p0)               { if (exp) FS_DEBUG_LOG1(s,p0); }
  #define FS_DEBUG_LOG2_IF(exp,s,p0,p1)            { if (exp) FS_DEBUG_LOG2(s,p0,p1); }
  #define FS_DEBUG_LOG3_IF(exp,s,p0,p1,p2)         { if (exp) FS_DEBUG_LOG3(s,p0,p1,p2); }
  #define FS_DEBUG_LOG4_IF(exp,s,p0,p1,p2,p3)      { if (exp) FS_DEBUG_LOG4(s,p0,p1,p2,p3); }
#else
  #define FS_DEBUG_LOG(s)
  #define FS_DEBUG_LOG1(s,p0)
  #define FS_DEBUG_LOG2(s,p0,p1)
  #define FS_DEBUG_LOG3(s,p0,p1,p2)
  #define FS_DEBUG_LOG4(s,p0,p1,p2,p3)
  #define FS_DEBUG_LOG_IF(exp,s)
  #define FS_DEBUG_LOG1_IF(exp,s,p0)
  #define FS_DEBUG_LOG2_IF(exp,s,p0,p1)
  #define FS_DEBUG_LOG3_IF(exp,s,p0,p1,p2)
  #define FS_DEBUG_LOG4_IF(exp,s,p0,p1,p2,p3)
#endif

/*******************************************************************
*
*               Asserts
*
********************************************************************
*/
#if FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ERRORS
  #define FS_DEBUG_ASSERT(exp)                     { if (!exp) FS_DEBUG_ERROROUT(#exp); }
#else
  #define FS_DEBUG_ASSERT(exp)
#endif

#endif /* LCD_H */




/*************************** End of file ****************************/
