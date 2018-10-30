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
 *---------------------------------------------------------------------------
 * (c) Copyright 1998..2009 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/
/*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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




