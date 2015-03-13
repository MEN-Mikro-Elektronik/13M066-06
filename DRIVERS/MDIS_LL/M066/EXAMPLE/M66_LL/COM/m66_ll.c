/*********************  P r o g r a m  -  M o d u l e **********************/
/*!
 *         \file m66_ll.c
 *      Project: MDIS 4.x
 *
 *       \author uf
 *        $Date: 2008/09/17 17:59:20 $
 *    $Revision: 1.3 $
 *
 *		  \brief simple example for using M66 Low Level Driver direct
 *
 *     Required: -
 *     \switches: VXWORKS
 *
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: m66_ll.c,v $
 * Revision 1.3  2008/09/17 17:59:20  CKauntz
 * R: No doxygen documentation
 * M: Changed comments to doxygen documentations
 *
 * Revision 1.2  1999/03/19 11:50:09  Franke
 * changed -  MDIS 4.1 debug handling
 *
 * Revision 1.1  1998/03/03 11:59:50  franke
 * initial
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998..2008 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static char *RCSid="$Id: m66_ll.c,v 1.3 2008/09/17 17:59:20 CKauntz Exp $\n";

#include <stdio.h>

#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/mdis_api.h>
#include <MEN/mdis_err.h>
#include <MEN/mbuf.h>       /* buffer lib functions and types */
#include <MEN/maccess.h>    /* hw access macros and types     */

#include <MEN/oss.h>    /* operating system services */
#include <MEN/desc.h>   /* descriptor manager        */
#include <MEN/ll_defs.h>    /* low level driver definitions   */
#include <MEN/ll_entry.h>   /* low level driver entry struct  */

#define _LL_DRV_
#define M_ERRSTRING
#include <MEN/dbg.h>
#include <MEN/m66_drv.h>

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
static const int32 OFF = 0;
static const int32 ON  = 1;

static const int32 T_OK = 0;
static const int32 T_ERROR = 1;

static const int32 CH_DUMMY  = 0;
static const int32 CH_0      = 0;
static const int32 CH_31     = 31;

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

void m66_ll( void );

#ifdef VXWORKS
    #define  OS_HDL		       	NULL   /* OS Handle not used */
    #define  DEV_SEM_NULL_HDL   NULL   /* Device semaphore handle only used by MDIS Kernel */
    #define  IRQ_NULL_HDL       NULL   /* no irq's used in this example */
    extern   DESC_SPEC          M66_1; /* low level descriptor specifier  */
#else
    /*
    *  OS like OS9, HPRT, LYNX
    */
    void main( void )
    {
        m66_ll();
    }

#endif/*VXWORKS*/

static void delay()
{
volatile int i,j;
    for( i=0;i<1000;i++)
        for( j=0;j<1000;j++);
}/*delay*/

/******************************** m66_ll **********************************/
/** This function shows the use of:
 *                    M66_Init()
 *                    M66_Write()
 *                    M66_Read()
 *                    M66_BlockWrite()
 *                    M66_BlockRead()
 *                    M66_Exit()
 *
 *                It initialize the module, switches channel 0 on,
 *                reads channel 31, writes a pattern to all channels
 *                reads back the pattern and deinitialize the module.
 *
 *---------------------------------------------------------------------------
 *  Input......:  void
 *
 *  Output.....:  return  ---
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
void m66_ll( void )
{
int32      ret;
MACCESS    ma[1];                /* access handle */
LL_HANDLE  *llDrvHdl;            /* ll drv handle */
int32      value;                /* value to write/read */
u_int8     buffer[M66_MAX_CH];   /* block write/read buffer */
int32      nbrOfBytes;           /* number of written/read bytes */
int        loopy;

    printf("m66_ll - started ...\n");

#ifdef VXWORKS
    OSS_Init();
#endif/*VXWORKS*/

    /* set the base address of the device */
	ma[0] = (MACCESS) 0x88aaaa00; /* VME A08/D16 range */

	/* gets the ll-drv handle, decodes descriptor, init HW */
    ret = M66_Init( &M66_1,                   /* LL-Descriptor handle */
                    OS_HDL,                   /* operating system handle */
                    ma,				          /* access handle */
                    DEV_SEM_NULL_HDL,         /* device semaphore handle -  not used in this example */
                    IRQ_NULL_HDL,             /* irq handle - not used in this example */
                    &llDrvHdl );		      /* pointer to ll drv handle */
    if( ret != 0 )
    {
        printf("*** ERROR - M66_Init()");
        goto LL_EXAMPLE_ERROR;
    }/*if*/

    /* switch channel 0 on */
    value = 1;
    ret = M66_Write( llDrvHdl,
                     CH_0,
                     value );
    if( ret != 0 )
    {
        printf("*** ERROR - M66_Write()");
        goto LL_EXAMPLE_ERROR;
    }/*if*/

    delay();

    /* read channel 31 */
    ret = M66_Read( llDrvHdl,
                    CH_31,
                    (u_int32*) &value );
    if( ret != 0 )
    {
        printf("*** ERROR - M66_Read()");
        goto LL_EXAMPLE_ERROR;
    }/*if*/

    delay();

    /* switch output pattern */
    for( loopy=0; loopy<M66_MAX_CH; loopy++ ) buffer[loopy]= loopy & 1; /* 010101... pattern */
    ret = M66_BlockWrite( llDrvHdl,
                          CH_DUMMY,
                          buffer,
                          M66_MAX_CH,
                          &nbrOfBytes );
    if( ret != 0 )
    {
        printf("*** ERROR - M66_BlockWrite()");
        goto LL_EXAMPLE_ERROR;
    }/*if*/

    delay();

    /* read back pattern */
    ret = M66_BlockRead( llDrvHdl,
                         CH_DUMMY,
                         buffer,
                         M66_MAX_CH,
                         &nbrOfBytes );
    if( ret != 0 )
    {
        printf("*** ERROR - M66_BlockRead()");
        goto LL_EXAMPLE_ERROR;
    }/*if*/

    /* setstat enable debug - task level 1 */
    ret = M66_SetStat( llDrvHdl,
                       M_LL_DEBUG_LEVEL,
                       CH_DUMMY,
                       DBG_NORM |DBG_LEV1 );
    if( ret != 0 )
    {
        printf("*** ERROR - M66_SetStat()");
        goto LL_EXAMPLE_ERROR;
    }/*if*/

    /* getstat ID_CHECK at M66_Init() */
    ret = M66_GetStat( llDrvHdl,
                       M_LL_ID_CHECK,
                       CH_DUMMY,
                       &value );
    if( ret != 0 )
    {
        printf("*** ERROR - M66_GetStat()");
        goto LL_EXAMPLE_ERROR;
    }/*if*/

    /* deinitialize the M66 and free the low level handle */
    ret = M66_Exit( &llDrvHdl );
    if( ret != 0 )
    {
        printf("*** ERROR - M66_Exit()");
        goto LL_EXAMPLE_ERROR;
    }/*if*/


#ifdef VXWORKS
    OSS_Exit();
#endif/*VXWORKS*/
    printf("m66_ll - finished - OK\n");
    return;

    /* jump to if error occurred */
LL_EXAMPLE_ERROR:
    printf(" with error 0x%04x ***\n", (int) ret );

#ifdef M_ERRSTRING
    printf("*** %s ***\n",M_errstring( ret ) );
#endif
#ifdef VXWORKS
    OSS_Exit();
#endif/*VXWORKS*/

}/*m66_ll*/

