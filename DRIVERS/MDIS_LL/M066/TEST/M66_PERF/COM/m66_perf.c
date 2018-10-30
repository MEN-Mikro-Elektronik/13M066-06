/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: m66_perf.c
 *      Project: MDIS 4.x
 *
 *       Author: ds
 *        $Date: 2010/03/26 14:55:30 $
 *    $Revision: 1.9 $
 *
 *  Description: performance test for m66 mdis driver
 *
 *     Required: -
 *     Switches: -
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000..2009 by MEN mikro elektronik GmbH, Nuernberg, Germany
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

static char *RCSid="$Id: m66_perf.c,v 1.9 2010/03/26 14:55:30 dpfeuffer Exp $\n";

#include <MEN/men_typs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <MEN/usr_oss.h>
#include <MEN/mdis_api.h>
#include <MEN/mdis_err.h>
#include <MEN/usr_utl.h>
#include <MEN/m66_drv.h>

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
#define	CALLS	10000


/******************************* errShow ************************************
 *
 *  Description:  Show MDIS or OS error message.
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  -
 *  Globals....:  -
 ****************************************************************************/
static void errShow( void )
{
   u_int32 error;

   error = UOS_ErrnoGet();

   printf("*** %s ***\n",M_errstring( error ) );
}

/********************************* usage ************************************
 *
 *  Description: Print program usage
 *			   
 *---------------------------------------------------------------------------
 *  Input......: -
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void usage(void)
{
	printf("Usage: m66_perf [<opts>] <device> [<opts>]\n");
	printf("Function: MDIS performance test with M66 hardware access\n");
	printf("Options:\n");
	printf("    device       device name (M66)     [none]\n");
	printf("    -c=<n>       nr of function calls  [%d]\n",CALLS);   
	printf("\n");
	printf("(c) 2000-2010 by MEN mikro elektronik GmbH\n%s\n\n",RCSid);
}

/******************************** main **************************************
 *
 *  Description:  main() - function
 *
 *                - opens the device
 *                - write and read channel 0
 *                - blockwrite and blockread all channels
 *                - close the device.
 *
 *                Shows the measured times for the calls.
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
	char *device,*str,*errstr,buf[40];
	int32  n, callcnt;
    MDIS_PATH path;
	int32  ch, val;
	int32  loop;
	int32  timeStart, timeStop, timeDiff;
	u_int8 dataBuf[M66_MAX_CH];
	
	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("c=?", buf))) {	/* check args */
		printf("*** %s\n", errstr);
		return(1);
	}

	if (UTL_TSTOPT("?")) {						/* help requested ? */
		usage();
		return(1);
	}

	/*--------------------+
    |  get arguments      |
    +--------------------*/
	for (device=NULL, n=1; n<argc; n++)
		if (*argv[n] != '-') {
			device = argv[n];
			break;
		}

	if (!device) {
		usage();
		return(1);
	}

	callcnt = ((str = UTL_TSTOPT("c=")) ? atoi(str) : CALLS);

    printf("m66_perf callcount=%d devicename=%s\n\n", (int)callcnt, device ); 

	/*------------------------------------+
	|  prepare buffer                     |
	+------------------------------------*/
	for( ch=0; ch<M66_MAX_CH; ch+=2 ) {
		dataBuf[ch] = 0;
		dataBuf[ch+1] = 1;
	}

	/*------------------------------------+
	|  open                               |
	+------------------------------------*/
    if( (path = M_open(device)) < 0 ) goto M66_TESTERR;

    /*------------------------------------+
    |  configuration                      |
    +------------------------------------*/
	if( M_setstat( path, M_BUF_RD_MODE, M_BUF_USRCTRL ) ) goto M66_TESTERR;

	/*------------------------------------+
	|  write                              |
	+------------------------------------*/
	printf("M_write:\n");

	timeStart = UOS_MsecTimerGet();
	for (loop=0; loop < callcnt; loop++) {
        if( M_write(path, 1 ) ) goto M66_TESTERR;
	}
	timeStop = UOS_MsecTimerGet();

	timeDiff = timeStop - timeStart;
	printf(" %d calls: %dmsec => %.1fusec / call\n",
		(int)callcnt, (int)timeDiff, (float)(timeDiff*1000)/(float)callcnt);

	/*------------------------------------+
	|  read                               |
	+------------------------------------*/
	printf("M_read:\n");

	timeStart = UOS_MsecTimerGet();
	for (loop=0; loop < callcnt; loop++) {
        if( M_read(path, &val ) ) goto M66_TESTERR;
	}
	timeStop = UOS_MsecTimerGet();

	timeDiff = timeStop - timeStart;
	printf(" %d calls: %dmsec => %.1fusec / call\n",
		(int)callcnt, (int)timeDiff, (float)(timeDiff*1000)/(float)callcnt);

	/*------------------------------------+
	|  blockwrite                         |
	+------------------------------------*/
	printf("M_setblock:\n");

	timeStart = UOS_MsecTimerGet();
	for (loop=0; loop < callcnt; loop++) {
		if( (M_setblock( path, dataBuf, M66_MAX_CH )) < 0 ) goto M66_TESTERR;
	}
	timeStop = UOS_MsecTimerGet();

	timeDiff = timeStop - timeStart;
	printf(" %d calls: %dmsec => %.1fusec / call\n",
		(int)callcnt, (int)timeDiff, (float)(timeDiff*1000)/(float)callcnt);

	/*------------------------------------+
	|  blockread                          |
	+------------------------------------*/
	printf("M_getblock\n");

	timeStart = UOS_MsecTimerGet();
	for (loop=0; loop < callcnt; loop++) {
		if( (M_getblock( path, dataBuf, M66_MAX_CH )) < 0 ) goto M66_TESTERR;
	}
	timeStop = UOS_MsecTimerGet();

	timeDiff = timeStop - timeStart;
	printf(" %d calls: %dmsec => %.1fusec / call\n",
		(int)callcnt, (int)timeDiff, (float)(timeDiff*1000)/(float)callcnt);

    /*------------------------------------+
    |  cleanup                            |
    +------------------------------------*/
    printf("M_close\n");
    if( M_close( path ) ) goto M66_TESTERR;

    printf("    => OK\n");
    return( 0 );


M66_TESTERR:
    errShow();
    printf("    => Error\n");
    M_close( path );
    return( 1 );
 }

