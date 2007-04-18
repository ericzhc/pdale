/**
 *********************************************************************
 *	@file       ppc.h
 *	@version    0.1
 *	@author     ELR
 *	@date       24 Avril 2006 
 *
 *	Project:    Sip Stack
 *
 *  @brief      CerfPDA: PPC (Peripheral Pin Controller) declarations
 *
 *  @todo 
 *
 *
 *	Revision History:
 * 
 * 
 *
 *	COPYRIGHT (C) 2006 Eduardo Luis Romero. Université de Sherbrooke.
 *  Québec, Canada.
 *
 *********************************************************************/
#ifndef _ERPDAPPC_H_
#define _ERPDAPPC_H_


/* //////////////////////////////////////////////////////////////////////////// */
/* define cosntants                        //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */

#define PPC_BASE_ADD    0x90060000
#define PPC_PPDR        *(volatile INT32U*)(PPC_BASE_ADD + 0x00000000)              /* PPC Pin Direction Register           */
#define PPC_PPSR        *(volatile INT32U*)(PPC_BASE_ADD + 0x00000004)              /* PPC Pin State Register               */
#define PPC_PPAR        *(volatile INT32U*)(PPC_BASE_ADD + 0x00000008)              /* PPC Pin Assignement Register         */
#define PPC_PSDR        *(volatile INT32U*)(PPC_BASE_ADD + 0x0000000C)              /* PPC Pin Sleep Mode Direction Register */
#define PPC_PPFR        *(volatile INT32U*)(PPC_BASE_ADD + 0x00000010)              /* PPC Pin Flag Register                */

#endif      /* end of _ERPDAPPC_H_ */


