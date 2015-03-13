#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#          $Date: 2004/08/30 15:43:28 $
#      $Revision: 1.2 $
#            $Id: driver_sw.mak,v 1.2 2004/08/30 15:43:28 dpfeuffer Exp $
#
#    Description: makefile for M66 LL driver, swapped variant
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver_sw.mak,v $
#   Revision 1.2  2004/08/30 15:43:28  dpfeuffer
#   minor modifications for MDIS4/2004 conformity
#
#   Revision 1.1  2000/08/09 14:09:13  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=m66_sw

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED \
		   $(SW_PREFIX)MAC_BYTESWAP   \
		   $(SW_PREFIX)M66_VARIANT=M66_SW \
           $(SW_PREFIX)ID_SW
           
MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)     \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/id_sw$(LIB_SUFFIX)       \
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

