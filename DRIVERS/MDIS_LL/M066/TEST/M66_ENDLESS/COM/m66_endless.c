/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: m66_endless.c
 *      Project: MDIS 4.x
 *
 *       Author: ds
 *        $Date: 2009/06/24 11:07:48 $
 *    $Revision: 1.8 $
 *
 *  Description: Endless test of the m66 mdis driver
 *
 *     Required: -
 *     Switches: -
 *
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 1999-2019, MEN Mikro Elektronik GmbH
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

static char *RCSid="$Id: m66_endless.c,v 1.8 2009/06/24 11:07:48 CRuff Exp $\n";

#include <MEN/men_typs.h>

#include <stdio.h>
#include <string.h>

#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>
#include <MEN/mdis_api.h>
#include <MEN/mdis_err.h>

#include <MEN/m66_drv.h>

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
static void errShow( void );

/******************************* errShow ************************************
 *
 *  Description:  Show MDIS or OS error message.
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  -
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
 *  Output.....:  return    0   if no error
 *                          1   if error
 *  Globals....:  -
 ****************************************************************************/
int main( int argc, char *argv[ ] )
{
	MDIS_PATH path;
	int32	n, count, set=1;
    int32	valWr=1, valRd;
	int32	delay, delayRand, delayRandOld=0;
	int32	ch, chRand, chRandOld=0;
	int32	chan[32];

    if( argc < 2 )
    {
        printf("usage: m66_endless <device name>\n");
        return 1;
    }
    
    printf("=========================\n");
    printf("%s", RCSid );
    printf("M_open(%s)\n", argv[1]);
    if( (path = M_open(argv[1])) < 0 ) goto M66_ERR;

	for( n=0; n<32; n++)
		chan[n] = 0;

    /* configuration */
    if( M_setstat( path, M_MK_IO_MODE, M_IO_EXEC ) ) goto M66_ERR;

	/* endless loop */
	do {

		/* select channel 0..31 */
       	chRand = UOS_Random(chRandOld);
		ch = UOS_RandomMap(chRand, 0, 31);
		chRandOld = chRand;

	    if( M_setstat(path, M_MK_CH_CURRENT, ch) ) goto M66_ERR;

		/* set channel (5x or 6x) */
		for (n=0; n<(5+set); n++) {

			valWr = n%2;
    		
			/* write value to curr ch */
       		if( M_write(path, valWr) ) goto M66_ERR;

			/* delays between 1ms and 500ms */
       		delayRand = UOS_Random(delayRandOld);
			delay = UOS_RandomMap(delayRand, 1, 500);
			delayRandOld = delayRand;
			UOS_Delay( delay );

			/* read value back */
       		if( M_read(path, &valRd) ) goto M66_ERR;
			printf("channel=%2d : write value=%d, read value=%d after %3d msec\n",
				(int)ch, (int)valWr, (int)valRd, (int)delay );

			if( valWr != valRd ) {
				printf("\007 *** m66 ch=%d wr/rd %ld/%ld differs\n", (int)ch, valWr, valRd );
				goto M66_ERR;
			}
		}

		chan[ch] = set;
		count = 0;

		for( n=0; n<32; n++)
			if( set == 1 )
				count += chan[n];
			else
				count -= chan[n];

		/* all channels set ? */
		if( count == 32 )
			set = 0;

		/* all channels cleared ? */
		if( count == 0 )
			set = 1;
	
	} while( UOS_KeyPressed()==-1 );

    printf("M_close\n");
    if( M_close( path ) ) goto M66_ERR;

    printf("    => OK\n");
    return( 0 );


M66_ERR:
    errShow();
    printf("    => Error\n");
    M_close( path );
    return( 1 );
}



