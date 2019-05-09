#***************************  M a k e f i l e  *******************************
#
#         Author: ds
#          $Date: 2004/08/30 15:43:53 $
#      $Revision: 1.2 $
#
#    Description: makefile descriptor file for M66_ENDLESS
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.2  2004/08/30 15:43:53  dpfeuffer
#   minor modifications for MDIS4/2004 conformity
#
#   Revision 1.1  1999/08/30 13:57:19  Schmidt
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=m66_endless

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/mdis_api$(LIB_SUFFIX)    \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)     \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_utl$(LIB_SUFFIX)     \


MAK_INCL=$(MEN_INC_DIR)/m66_drv.h     \
         $(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/mdis_api.h    \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_INC_DIR)/usr_oss.h     \
		 $(MEN_INC_DIR)/usr_utl.h     \

MAK_INP1=m66_endless$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)
