/*
*********************************************************************************************************
*                                               uC/HTTPs
*                                   HyperText Transfer Protocol Server
*
*                          (c) Copyright 2004-2005, Micrium, Inc., Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              HTTP SERVER
*                                      CONFIGURATION TEMPLATE FILE
*
* Filename      : http-s-cfg.h
* Programmer(s) : JDH
* Version       : V1.11
*********************************************************************************************************
*/

#define  HTTPS_OS_TASK_NAME                 "HTTP (Server)"
#define  HTTPs_OS_TASK_PRIO                   12
#define  HTTPs_OS_TASK_STK_SIZE             2048

#define  HTTPs_FS_ROOT                      "/HTTPRoot"
#define  HTTPs_DEFAULT_FILE                 "index.htm"

#define  HTTPs_IPPORT                         80

#define  HTTPs_MAX_VAR_SIZE                  255
#define  HTTPs_MAX_VAL_SIZE                  255

#define  HTTPs_HTML_NOT_FOUND_MSG           "<HTML>\r\n" \
                                            "<BODY>\r\n" \
                                            "<HEAD><TITLE>SYSTEM ERROR</TITLE></HEAD>\r\n" \
                                            "<H1>NOT FOUND</H1>\r\n" \
                                            "The requested object does not exist on this server.\r\n" \
                                            "</BODY>\r\n" \
                                            "</HTML>\r\n"
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	   		 		  	   		   	 			       	   	  		      		      		  	 	 		  				 	      		   	  		  				 	  	   		      		    		 	       	  	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		      	   	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
