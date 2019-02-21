#***************************  M a k e f i l e  *******************************
#
#         Author: uf
#          $Date: 1998/03/03 11:59:57 $
#      $Revision: 1.1 $
#        $Header: /dd2/CVSR/COM/DRIVERS/MDIS_LL/M066/EXAMPLE/M66_LL/COM/program.mak,v 1.1 1998/03/03 11:59:57 franke Exp $
#
#    Description: makefile descriptor file for common
#                 modules MDIS 4.x   e.g. low level driver
#
#-----------------------------------------------------------------------------
#   Copyright (c) 1998-2019, MEN Mikro Elektronik GmbH
#*****************************************************************************
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

MAK_NAME=m66_ll

MAK_LIBS= \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)     \


MAK_INCL=$(MEN_INC_DIR)/m66_drv.h     \
         $(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/mdis_api.h    \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_INC_DIR)/usr_oss.h     \


MAK_OPTIM=$(OPT_1)

MAK_INP1=m66_ll$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)
