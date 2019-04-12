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
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m66_endless.c,v $
 * Revision 1.8  2009/06/24 11:07:48  CRuff
 * R: new MDIS path type MDIS_PATH
 * M: changed type of filedescriptor for M_open to MDIS_PATH
 *
 * Revision 1.7  2009/03/30 14:31:41  ufranke
 * cosmetics
 *
 * Revision 1.6  2008/09/17 18:15:07  CKauntz
 * R: No 64 bit support
 * M: Changed fd to INT32_OR_64 to support 64 bit OS
 *
 * Revision 1.5  2008/09/15 15:05:56  ufranke
 * R: diab compiler warnings
 * M: cosmetics
 *
 * Revision 1.4  2008/01/18 14:27:40  ufranke
 * cosmetics
 *
 * Revision 1.3  2000/08/09 14:09:23  kp
 * c++ comments converted to ANSI-C
 *
 * Revision 1.2  1999/08/30 14:21:33  Schmidt
 * cosmetics
 *
 * Revision 1.1  1999/08/30 13:57:18  Schmidt
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999..2009 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

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



