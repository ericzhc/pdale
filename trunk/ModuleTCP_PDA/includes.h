#ifndef INCLUDES_H
#define INCLUDES_H
/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2005, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                           MASTER INCLUDE FILE
*********************************************************************************************************
*/

#include  <stdio.h>
#include  <string.h>
#include  <ctype.h>
#include  <stdlib.h>

#include  <app_cfg.h>

#include  <cpu.h>
#include  <lib_def.h>
#include  <lib_mem.h>
#include  <lib_str.h>

#include  <ucos_ii.h>
#include  <LH79520_map.h>
#include  <serial.h>
#include  <bsp.h>

#include  <net.h>
#include  <dhcp-c.h>
#include  <fs_api.h>
#include  <http-s.h>
#include  <tftp.h>

#include "sockettcp.h"



#define   DBG_PRINT  printf

#endif
