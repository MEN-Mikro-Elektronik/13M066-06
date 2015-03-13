/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: m66_simp.c
 *      Project: MDIS 4.x
 *
 *       Author: uf
 *        $Date: 2009/06/24 11:07:33 $
 *    $Revision: 1.12 $
 *
 *  Description: simple test of the m66 mdis driver
 *
 *     Required: -
 *     Switches: VXWORKS
 *
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m66_simp.c,v $
 * Revision 1.12  2009/06/24 11:07:33  CRuff
 * R: new MDIS path type MDIS_PATH
 * M: changed type of filedescriptor for M_open to MDIS_PATH
 *
 * Revision 1.11  2009/03/30 14:31:20  ufranke
 * cosmetics
 *
 * Revision 1.10  2008/09/17 17:59:18  CKauntz
 * R: No 64 bit support
 * M: Changed path to INT32_OR_64 to support 64 bit OS
 *
 * Revision 1.9  2006/07/20 15:07:51  ufranke
 * cosmetics
 *
 * Revision 1.8  2005/06/21 12:52:53  UFranke
 * cosmetics
 *
 * Revision 1.7  2003/07/25 15:09:26  UFranke
 * changed
 *   - toggles now channel 0..31
 *
 * Revision 1.6  2001/06/26 15:54:19  franke
 * cosmetics NO_MAIN_FUNC removed
 *
 * Revision 1.5  2000/08/09 14:09:19  kp
 * removed unused consts
 *
 * Revision 1.4  1998/07/29 15:13:11  see
 * M_close return value check was wrong
 *
 * Revision 1.3  1998/07/15 15:36:51  Franke
 * update to MDIS 4.1
 *
 * Revision 1.2  1998/02/19 15:22:24  franke
 * ch was uninitialized
 *
 * Revision 1.1  1998/02/19 14:39:52  Schmidt
 * Added by mcvs
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998..2009 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static char *RCSid="$Id: m66_simp.c,v 1.12 2009/06/24 11:07:33 CRuff Exp $\n";

#include <MEN/men_typs.h>

#include <stdio.h>
#include <string.h>

#include <MEN/usr_oss.h>
#include <MEN/mdis_api.h>
#include <MEN/mdis_err.h>

#include <MEN/m66_drv.h>

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
static int M66_Simple( char *DevName );

/******************************* errShow ************************************
 *
 *  Description:  Show MDIS or OS error message.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *
 *  Output.....:  -
 *
 *  Globals....:  errno
 *
 ****************************************************************************/
static void errShow( void )
{
   u_int32 error;

   error = UOS_ErrnoGet();

   printf("*** %s ***\n",M_errstring( error ) );
}


/******************************** main **************************************
 *
 *  Description:  main() - function
 *
 *---------------------------------------------------------------------------
 *  Input......:  argc      number of arguments
 *                *argv     pointer to arguments
 *                argv[1]   device name
 *
 *  Output.....:  return    0   if no error
 *                          1   if error
 *
 *  Globals....:  -
 ****************************************************************************/
 int main( int argc, char *argv[ ] )
 {
    if( argc < 2){
        printf("usage: m66_simp <device name>\n");
        return 1;
    }
    M66_Simple(argv[1]);
    return 0;
 }


/******************************* M66_Simple *********************************
 *
 *  Description:  Opens the device, write and read channel 0.
 *                Shows the read values.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  devName device name in the system e.g. "/m66/0"
 *
 *  Output.....:  return  T_OK or T_ERROR
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
static int M66_Simple( char *devName )
{
    MDIS_PATH path;
    int32  ch;
    int32  valL;
    int32  i;

    printf("=========================\n");
    printf("%s", RCSid );
    printf("M_open\n");
    if( (path = M_open(devName)) < 0 ) goto M66_TESTERR;

    /*------------------------------------+
    |  configuration                      |
    +-------------------------------------*/
    printf("configuration - M_setstat current channel 0\n");
    if( M_setstat( path, M_MK_IO_MODE, M_IO_EXEC ) ) goto M66_TESTERR;

    ch = 0; /* select the first channel */

    for( i=0; i<64; i++ )
    {
    	ch = i/2;
	   	M_setstat(path, M_MK_CH_CURRENT, ch);    /* set current channel */
       	valL = ((i+1)%2);
       	printf("M_write ch %d %d  || ", (int)ch, (int) valL );
       	if( M_write(path, valL ) ) goto M66_TESTERR;  /* write value to curr ch */

       	UOS_Delay( 300 );

       	printf("M_read ch %d", (int)ch );
       	if( M_read(path, &valL ) ) goto M66_TESTERR;  /* read value back */
       	printf(" %d\n", (int) valL);

       	UOS_Delay( 300 );
    }/*for*/


    printf("M_close\n");
    if( M_close( path ) < 0) goto M66_TESTERR;

    printf("    => OK\n");
    return( 0 );


M66_TESTERR:
    errShow();
    printf("    => Error\n");
    M_close( path );
    return( 1 );
}/*M66_Simple*/




