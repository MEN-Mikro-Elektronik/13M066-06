#***************************  M a k e f i l e  *******************************
#
#         Author: franke
#          $Date: 2004/08/30 15:43:26 $
#      $Revision: 1.3 $
#        $Header: /dd2/CVSR/COM/DRIVERS/MDIS_LL/M066/DRIVER/COM/driver.mak,v 1.3 2004/08/30 15:43:26 dpfeuffer Exp $
#
#    Description: makefile descriptor file for common
#                 modules MDIS 4.x   e.g. low level driver
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver.mak,v $
#   Revision 1.3  2004/08/30 15:43:26  dpfeuffer
#   minor modifications for MDIS4/2004 conformity
#
#   Revision 1.2  1998/07/15 15:36:18  Franke
#   added dbg lib for MDIS 4.1
#
#   Revision 1.1  1998/02/19 14:39:25  Schmidt
#   Added by mcvs
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1997 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=m66

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED
           
MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)     \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/id$(LIB_SUFFIX)       \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/mbuf$(LIB_SUFFIX)     \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)      \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)


MAK_INCL=$(MEN_MOD_DIR)/m66_flex.h    \
         $(MEN_INC_DIR)/m66_drv.h     \
         $(MEN_INC_DIR)/dbg.h    	  \
         $(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/oss.h         \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_INC_DIR)/mbuf.h    \
         $(MEN_INC_DIR)/maccess.h     \
         $(MEN_INC_DIR)/desc.h        \
         $(MEN_INC_DIR)/mdis_api.h    \
         $(MEN_INC_DIR)/mdis_com.h    \
         $(MEN_INC_DIR)/modcom.h      \
         $(MEN_INC_DIR)/ll_defs.h     \
         $(MEN_INC_DIR)/ll_entry.h    \

MAK_INP1=m66_drv$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

