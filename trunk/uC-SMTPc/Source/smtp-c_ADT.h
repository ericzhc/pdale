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
*                                       AND FUNCTIONS DEFINITION
*
* Filename      : smtp-c_ADT.h
* Version       : V1.02
* Programmer(s) : SR
*                 JDH
*********************************************************************************************************
*
* This defines the various abstract data types needed in order to implement the SMTP client
* module, as well as helper functions to manipulate these data types.
*
*********************************************************************************************************
*/

#ifndef  __SMTP_C_ADT_H__
#define  __SMTP_C_ADT_H__


/*
*********************************************************************************************************
*                                              ERROR CODES
*********************************************************************************************************
*/

#define  SMTPc_ADT_ERR_BASE_NUMBER                                                     51100

#define  SMTPc_ADT_ERR_NONE                     (NET_ERR)  SMTPc_ERR_NONE
#define  SMTPc_ADT_ERR_NULL_ARG                 (NET_ERR) (SMTPc_ADT_ERR_BASE_NUMBER +     1)
#define  SMTPc_ADT_ERR_STR_TOO_LONG             (NET_ERR) (SMTPc_ADT_ERR_BASE_NUMBER +     2)


/*
*********************************************************************************************************
*                                              DEFINITIONS
*
* Note(s): (1) Maximum length of key-value strings in structure SMTPc_KEY_VAL.
*
*          (2) As mentionned in RFC #2821, Section 'The SMTP Specifications, Additionnal
*              Implementation Issues, Sizes and Timeouts, Size limits and minimums', "The maximum
*              total length of a user name or other local-part is 64 characters [and] the maximum
*              total length of a domain name or number is 255 characters.".  Adding 2 for
*              '@' (see SMTPc_MBOX structure) and '\0'.
*
*          (3) Maximum length of content-type.
*
*          (4) From RFC #2822, Section 'Syntax, Fields definitions, Identification fields', "The message
*              identifier (msg-id) [field] is similar in syntax to an angle-addr construct".
*
*          (5) Size of ParamArray in structure SMTPc_MIME_ENTITY_HDR.
*
*          (6) Maximum length of attachment's name and description.
*********************************************************************************************************
*/

                                                                /* See note #1                                          */
#define  SMTPc_KEY_VAL_KEY_LEN                            30
#define  SMTPc_KEY_VAL_VAL_LEN                            30

                                                                /* See note #2                                          */
#define  SMTPc_MBOX_DOMAIN_NAME_LEN                      255
#define  SMTPc_MBOX_LOCAL_PART_LEN                        64
#define  SMTPc_MBOX_ADDR_LEN                   (SMTPc_MBOX_DOMAIN_NAME_LEN + SMTPc_MBOX_LOCAL_PART_LEN + 2)

                                                                /* See note #3                                          */
#define  SMTPc_MIME_CONTENT_TYPE_LEN                      20
                                                                /* See note #4                                          */
#define  SMTPc_MIME_ID_LEN                      SMTPc_MBOX_ADDR_LEN
                                                                /* See note #5                                          */
#define  SMTPc_MIME_MAX_KEYVAL                             1

                                                                /* See note #6                                          */
#define  SMTPc_ATTACH_NAME_LEN                            50
#define  SMTPc_ATTACH_DESC_LEN                            50

                                                                /* See note #4                                          */
#define  SMTPc_MSG_MSGID_LEN                    SMTPc_MBOX_ADDR_LEN


/*
*********************************************************************************************************
*                                              DATA TYPES
*
* Note(s): (1) From RFC #2821 'The SMTP Model, Terminology, Mail Objects', "SMTP transports a mail
*              object.  A mail object contains an envelope and content.  The SMTP content is sent
*              in the SMTP DATA protocol unit and has two parts: the headers and the body":
*
*                       |----------------------|
*                       |                      |
*                       |                      |  Envelope
*                       |                      |
*                       |======================|
*                       |                      |
*                       |  Headers             |
*                       |                      |
*                       |                      |
*                       |                      |
*                       |----------------------|  Content
*                       |                      |
*                       |  Body                |
*                       |                      |
*                       |                      |
*                       |                      |
*                       |----------------------|
*
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                            KEY-VALUE STRUCTURE AND KEY-VAL LIST DATA TYPES
*
* Note(s): (1) This structure makes room for additional MIME header fields (see RFC #2045, Section
*              'Additional MIME Header Fields').
*********************************************************************************************************
*/

typedef struct smtpc_key_val
{
    CPU_CHAR  Key[SMTPc_KEY_VAL_KEY_LEN];                       /* Key (header field name)                              */
    CPU_CHAR  Val[SMTPc_KEY_VAL_VAL_LEN];                       /* Value associated with the preceding key              */
} SMTPc_KEY_VAL;


/*
*********************************************************************************************************
*                               SMTP MAILBOX AND MAILBOX LIST DATA TYPES
*
* Note(s): (1) Structure representing an email address, as well as the name of its owner.
*********************************************************************************************************
*/

typedef struct smtpc_mbox
{
    CPU_CHAR  NameDisp[SMTPc_CFG_MBOX_NAME_DISP_LEN];           /* Displayed name of address's owner                    */
    CPU_CHAR  Addr    [SMTPc_MBOX_ADDR_LEN];                    /* Address (local part '@' domain)                      */
} SMTPc_MBOX;


/*
*********************************************************************************************************
*                                        SMTP MIME ENTITY HEADER
*
* Note(s): (1) See RFC #2045 for details.
*
*          (2) Structure subject to change.  For instance, other data structures could be used to represent
*              "Encoding", etc.  The encoding could also be left to do by the application
*********************************************************************************************************
*/

typedef struct smtpc_mime_entity_hdr
{
    CPU_CHAR       *ContentType[SMTPc_MIME_CONTENT_TYPE_LEN];   /* Description of contained body data (IANA assigned)   */
    SMTPc_KEY_VAL  *ParamArray[SMTPc_MIME_MAX_KEYVAL];          /* Additional parameters for specified content-type     */
    void           *ContentEncoding;                            /* Content transfer encoding                            */
    void          (*EncodingFnctPtr)(CPU_CHAR *,                /* Pointer to function performing the encoding of the   */
                                     NET_ERR  *);               /* attachment                                           */
    CPU_CHAR        ID[SMTPc_MIME_ID_LEN];                      /* Unique attachment identifier                         */
} SMTPc_MIME_ENTITY_HDR;


/*
*********************************************************************************************************
*                        SMTP MESSAGE ATTACHMENT AND ATTACHMENT LIST DATA TYPES
*
* Note(s): (1) Attachments are not going to be supported in the first version of the uCSMTPc module.
*              Hence, the format of this structure will surely change to accommodate for possible
*              file system access.
*********************************************************************************************************
*/

typedef struct smtpc_attach
{
    SMTPc_MIME_ENTITY_HDR   MIMEPartHdrStruct;                  /* MIME content header for this attachment              */
    CPU_CHAR                Name[SMTPc_ATTACH_NAME_LEN];        /* Name of attachment inserted in the message           */
    CPU_CHAR                Desc[SMTPc_ATTACH_DESC_LEN];        /* Optional attachment description                      */
    void                   *AttachData;                         /* Pointer to beginning of body (data of the entity)    */
    CPU_INT32U              Size ;                              /* Size of data in octets                               */

} SMTPc_ATTACH;


/*
*********************************************************************************************************
*                                          SMTP MSG Structure
*
* Note(s): (1) A mail object is represented in this module by the structure SMTPc_MSG.  This
*              structure contains all the necessary information to generate the mail object
*              and to send it to the SMTP server.  More specifically, it encapsulates the various
*              addresses of the sender and recipients, MIME information, the message itself, and
*              finally the eventual attachments.
*********************************************************************************************************
*/

typedef struct smtpc_msg
{
    SMTPc_MBOX             *From;                               /* "From" field     (1:1)                               */
    SMTPc_MBOX             *ToArray[SMTPc_CFG_MSG_MAX_TO];      /* "To" field       (1:*)                               */
    SMTPc_MBOX             *ReplyTo;                            /* "Reply-to" field (0:1)                               */
    SMTPc_MBOX             *Sender;                             /* "Sender" field   (0:1)                               */
    SMTPc_MBOX             *CCArray[SMTPc_CFG_MSG_MAX_CC];      /* "CC" field       (0:*)                               */
    SMTPc_MBOX             *BCCArray[SMTPc_CFG_MSG_MAX_BCC];    /* "BCC" field      (0:*)                               */
    CPU_CHAR                MsgID[SMTPc_MSG_MSGID_LEN];         /* Unique message identifier.                           */
    SMTPc_MIME_ENTITY_HDR   MIMEMsgHdrStruct;                   /* Mail object MIME content headers.                    */
    CPU_CHAR                Subject[SMTPc_CFG_MSG_SUBJECT_LEN]; /* Subject of message.                                  */
    SMTPc_ATTACH           *AttachArray[SMTPc_CFG_MSG_MAX_ATTACH]; /* List of attachment(s), if any.                     */
    CPU_CHAR               *ContentBodyMsg;                     /* Data of the mail object content's body.              */
    CPU_INT32U              ContentBodyMsgLen;                  /* Size (in octets) of buffer pointed by ContentBodyMsg.*/
} SMTPc_MSG;


/*
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*/

#endif  /* __SMTP_C_ADT_H__ */
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
