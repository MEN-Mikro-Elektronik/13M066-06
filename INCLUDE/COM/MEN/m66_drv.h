/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: m66_driver.h
 *
 *      $Author: CRuff $
 *        $Date: 2009/06/30 11:51:03 $
 *    $Revision: 1.5 $
 *
 *  Description: Header file for M66 driver modules
 *               - M66 specific status codes
 *               - M66 function prototypes
 *
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_
 *               _LL_DRV_
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m66_drv.h,v $
 * Revision 1.5  2009/06/30 11:51:03  CRuff
 * R: Porting to MDIS5
 * M: changed according to MDIS Porting Guide 0.5
 *
 * Revision 1.4  2000/08/09 14:09:27  kp
 * support swapped variant.
 * Removed all entry functions. Now static in m66_drv.c
 *
 * Revision 1.3  1998/09/23 15:32:11  Franke
 * changed define M66_LLDRV_H with _M66_LLDRV_H
 *
 * Revision 1.2  1998/02/26 10:34:39  franke
 * cosmetics
 *
 * Revision 1.1  1998/02/18 11:16:56  franke
 * Added by mcvs
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1995 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/
#ifndef _M66_LLDRV_H
#  define _M66_LLDRV_H

#  ifdef __cplusplus
      extern "C" {
#  endif

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
/* following macros can be used to control names of globally visible symbols */
#ifndef  M66_VARIANT
# define M66_VARIANT M66
#endif

# define _M66_GLOBNAME(var,name) var##_##name
#ifndef _ONE_NAMESPACE_PER_DRIVER_
# define M66_GLOBNAME(var,name) _M66_GLOBNAME(var,name)
#else
# define M66_GLOBNAME(var,name) _M66_GLOBNAME(M66,name)
#endif

#define __M66_GetEntry			M66_GLOBNAME(M66_VARIANT,GetEntry)

#define M66_MAX_CH     32

/*--------- M66 specific status codes (MCOD_OFFS...MCOD_OFFS+0xff) --------*/
                                          /* S,G: S=setstat, G=getstat code */
#define M66_IRQ_SOURCE            M_DEV_OF+0x01   /* G:   get interrupt source (last) */
#define M66_EDGE_MASK             M_DEV_OF+0x02   /* G,S: edge mask  */
#define M66_SIG_EDGE_OCCURRED     M_DEV_OF+0x03   /* G,S: signal */
#define M66_SIG_CLR_EDGE_OCCURRED M_DEV_OF+0x04   /*   S: signal */


/* channel option flags */
#define M66_IELH  0x1           /* irq on rising edge */
#define M66_IEHL  0x2           /* irq on falling edge */

/* channel status flags */
#define M66_OUTVAL  0x01        /* real value */
#define M66_OUTLH   0x02        /* rising edge occured */
#define M66_OUTHL   0x04        /* falling edge occured */


/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

#ifdef _LL_DRV_
#   ifndef _ONE_NAMESPACE_PER_DRIVER_
        extern void __M66_GetEntry( LL_ENTRY* drvP );
#   endif /* !_ONE_NAMESPACE_PER_DRIVER_ */
#endif /* _LL_DRV_ */

/*-----------------------------------------+
|  BACKWARD COMPATIBILITY TO MDIS4         |
+-----------------------------------------*/
#ifndef U_INT32_OR_64
 /* we have an MDIS4 men_types.h and mdis_api.h included */
 /* only 32bit compatibility needed!                     */
 #define INT32_OR_64  int32
 #define U_INT32_OR_64 u_int32
 typedef INT32_OR_64  MDIS_PATH;
#endif /* U_INT32_OR_64 */

#  ifdef __cplusplus
      }
#  endif

#endif/*_M66_LLDRV_H*/

