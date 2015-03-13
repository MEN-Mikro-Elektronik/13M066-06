#***************************  M a k e f i l e  *******************************
#
#         Author: ds
#          $Date: 2004/08/30 15:43:46 $
#      $Revision: 1.4 $
#
#    Description: Makefile definitions for the m66_demo example program
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.4  2004/08/30 15:43:46  dpfeuffer
#   minor modifications for MDIS4/2004 conformity
#
#   Revision 1.3  2003/07/25 15:10:12  UFranke
#   cosmetics
#
#   Revision 1.2  1998/07/20 12:04:25  Schmidt
#   m66_drv.h removed, cosmetics
#
#   Revision 1.1  1998/02/20 15:07:26  Schmidt
#   Added by mcvs
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1998 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=m66_demo

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/mdis_api$(LIB_SUFFIX)    \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)     \


MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/mdis_api.h    \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_INC_DIR)/usr_oss.h     \

MAK_INP1=m66_demo$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)
