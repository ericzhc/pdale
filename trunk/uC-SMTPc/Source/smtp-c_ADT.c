/*
*********************************************************************************************************
*                                               uC/SMTPc
*                                Simple Mail Transfert Protocol (client)
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                              SMTP CLIENT
*
*                                  ABSTRACT DATA TYPES IMPLEMENTATION
*
* Filename      : smtp-c_ADT.c
* Version       : V1.02
* Programmer(s) : SR
*                 JDH
*********************************************************************************************************
*
* This code implements helper functions used to manipulate the abstract data types used in the SMTP
* client module.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define    SMTPc_ADT_MODULE
#include  <net.h>
#include  <smtp-c.h>


/*
*********************************************************************************************************
*                                        SMTPcADT_SetSMTPcMbox()
*
* Description : (1) Populates a SMTPc_MBOX structure with associated name and address.
*                   (a) Perform error checking (to preserve structure integrity).
*                   (b) Copy arguments in structure.
*
* Argument(s) : mbox            SMTPc_MBOX structure to be populated.
*               name            Name of the mailbox owner.
*               addr            Address associated with the mailbox.
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               SMTPc_ERR_NONE                      No error, structure ready.
*                               SMTPc_ADT_ERR_NULL_ARG              mbox and/or name argument(s) NULL.
*                               SMTPc_ADT_ERR_STR_TOO_LONG          Argument addr too long.
*
* Return(s)   : void.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) The name of the mailbox owner is not mandatory.  Passing NULL will result in an
*                   empty string being copied in the structure.
*********************************************************************************************************
*/

void  SMTPcADT_SetSMTPcMbox (SMTPc_MBOX  *mbox,
                             CPU_CHAR    *name,
                             CPU_CHAR    *addr,
                             NET_ERR     *perr)
{
    CPU_SIZE_T  len;

    if ((mbox == (SMTPc_MBOX *)0) ||
        (addr == (CPU_CHAR *)0)) {
        *perr = SMTPc_ADT_ERR_NULL_ARG;
         return;
    }

    len = Str_Len(addr);
    if ( len >= SMTPc_MBOX_ADDR_LEN) {
        *perr = SMTPc_ADT_ERR_STR_TOO_LONG;
         return;
    } else {
         Str_Copy(mbox->Addr, addr);
    }

    if (name == (CPU_CHAR *)0) {                                /* See note #1                                          */
        Str_Copy(mbox->Addr, "");
    } else {
        len = Str_Len(name);
        if ( len >= SMTPc_CFG_MBOX_NAME_DISP_LEN) {
            *perr = SMTPc_ADT_ERR_STR_TOO_LONG;
             return;
        } else {
             Str_Copy(mbox->NameDisp, name);
        }
    }

    *perr = SMTPc_ADT_ERR_NONE;
}


/*
*********************************************************************************************************
*                                        SMTPcADT_InitSMTPcMsg()
*
* Description : (1) Sets the various fields of a SMTPc_MSG structure so that it is valid and usable.
*
* Argument(s) : msg             SMTPc_MSG structure to be initialized.
*               from            SMTPc_MBOX structure representing the "From" field.
*               sender          SMTPc_MBOX structure representing the "Sender" field (optional).
*               perr            Pointer to variable that will hold the return error code from this
*                               function :
*
*                               SMTPc_ADT_ERR_NONE                  No error, structure ready.
*                               SMTPc_ADT_ERR_NULL_ARG              msg and/or from argument(s) NULL.
*
* Return(s)   : void.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function MUST be called after declaring a SMTPc_MSG structure and BEFORE beginning
*                   to manipulate it.  Failure to do so will likely produce run-time errors.
*
*               (2) The SMTPc_MSG structure member 'MIMEMsgHdrStruct' is left uninitialized for now,
*                   MIME extensions not being supported in this version.
*********************************************************************************************************
*/

void  SMTPcADT_InitSMTPcMsg (SMTPc_MSG   *msg,
                             SMTPc_MBOX  *from,
                             SMTPc_MBOX  *sender,
                             NET_ERR     *perr)
{
    CPU_INT08U  i;


    if ((msg  == (SMTPc_MSG *)0)  ||
        (from == (SMTPc_MBOX *)0)) {
        *perr  = SMTPc_ADT_ERR_NULL_ARG;
        return;
    }

    msg->From   = from;
    msg->Sender = sender;

                                                                /* Setting various pointer to NULL or 0                 */
    for (i = 0; i < SMTPc_CFG_MSG_MAX_TO; i++) {
        msg->ToArray[i]      = (SMTPc_MBOX *)0;
    }

    for (i = 0; i < SMTPc_CFG_MSG_MAX_CC; i++) {
        msg->CCArray[i]      = (SMTPc_MBOX *)0;
    }

    for (i = 0; i < SMTPc_CFG_MSG_MAX_BCC; i++) {
        msg->BCCArray[i]     = (SMTPc_MBOX *)0;
    }

    for (i = 0; i < SMTPc_CFG_MSG_MAX_ATTACH; i++) {
        msg->AttachArray[i]  = (SMTPc_ATTACH *)0;
    }

    msg->ReplyTo             = (SMTPc_MBOX *)0;
    msg->ContentBodyMsg      = (CPU_CHAR *)0;
    msg->ContentBodyMsgLen   = 0;

                                                                /* Emptying CPU_CHAR arrays                             */
    Mem_Set(msg->MsgID,   (CPU_CHAR)0, SMTPc_MIME_ID_LEN);
    Mem_Set(msg->Subject, (CPU_CHAR)0, SMTPc_CFG_MSG_SUBJECT_LEN);

    SMTPc_TRACE_DBG(("SMTPcADT_InitSMTPcMsg() success\n\r"));

   *perr = SMTPc_ADT_ERR_NONE;
}
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
