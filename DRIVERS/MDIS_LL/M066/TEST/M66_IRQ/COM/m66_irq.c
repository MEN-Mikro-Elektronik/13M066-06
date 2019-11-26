/****************************************************************************
 ************                                                    ************
 ************                    M66_IRQ                         ************
 ************                                                    ************
 ****************************************************************************/
/*!  
 *         \file m66_irq.c
 *       \author dieter.pfeuffer@men.de
 *
 *        \brief Test program for interrupt handling of the M66 driver 
 *
 *     Required: libraries: mdis_api, usr_oss, usr_utl
 *     \switches WINNT
 *
 *
 *---------------------------------------------------------------------------
 * Copyright 2005-2019, MEN Mikro Elektronik GmbH
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

#include <MEN/men_typs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>
#include <MEN/mdis_api.h>
#include <MEN/mdis_err.h>
#include <MEN/m66_drv.h>

static const char IdentString[]=MENT_XSTR(MAK_REVISION);

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define CH_NOT 0xff00
#define BUF_LH	0	/* index for G_BufEdge */
#define BUF_HL	1	/* index for G_BufEdge */

/*
 * Specify if the OS specific MDIS implementation
 * allows driver calls from the signal handler.
 */
#ifdef WINNT
#define SIGHDLR_CAN_CALL
#endif

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
static u_int32 G_SigCountEdges;		/* count of edge signals */
static u_int32 G_SigCountHighwater;	/* count of high water signals */
static u_int32 G_SigCountOthers;	/* count of other signals */
static u_int32	G_BufEdge[2][M66_MAX_CH];

#ifdef SIGHDLR_CAN_CALL
	static U_INT32_OR_64	G_Path;
	static u_int32	G_AllReportedEdges;
	static u_int32	G_MultipleChEdges;
	static u_int8	G_BufRead[2][M66_MAX_CH];
#endif

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static void usage(void);
static void __MAPILIB SigHandler(u_int32 sigCode);
static void PrintMdisError(char *info);
static void PrintUosError(char *info);


/********************************* usage ***********************************/
/**  Print program usage
 */
static void usage(void)
{
	printf("Usage: m66_irq [<opts>] <device> [<opts>]\n");
	printf("Function: Test program for interrupt handling of the M66 driver\n");
	printf("          Note: <chan> = 0..31\n");
	printf("Options:\n");
	printf("  device       device name........................... [none]   \n");
	printf("  -d=<ms>      delay for loops......................... [no]   \n");
	printf("  ===output==\n");
	printf("  -t=<chan>    toggle specified channel..........(loop) [no]   \n");
	printf("  -T           toggle all channels...............(loop) [no]   \n");
	printf("  -h=<chan>    set specified channel high.............  [no]   \n");
	printf("  -l=<chan>    set specified channel low..............  [no]   \n");
	printf("  ===input===\n");
	printf("  -g=<chan>    get state of specified channel... (loop) [no]   \n");
	printf("  -G           get all channel states.................. [no]   \n");
	printf("  ===interrupt===\n");
	printf("  -i=<chan>    handle IRQs of specified channel..(loop) [no]   \n");
	printf("  -I           handle IRQs of all channels.......(loop) [no]   \n");
	printf("  -r           observe rising edge (for -i,-I)......... [no]   \n");
	printf("  -f           observe falling edge (for -i,-I)........ [no]   \n");
	printf("  -b=<mode>    read-block i/o mode (for -i,-I).......... [0]   \n");
	printf("                 0 = M_BUF_USRCTRL\n");
	printf("                 1 = M_BUF_CURRBUF\n");
	printf("                 2 = M_BUF_RINGBUF\n");
	printf("                 3 = M_BUF_RINGBUF_OVERWR\n");
	printf("\n");
	printf("Copyright 2005-2019, MEN Mikro Elektronik GmbH\n\n");
	printf("%s\n", IdentString );
}

/********************************* main ************************************/
/** Program main function
 *
 *  \param argc       \IN  argument counter
 *  \param argv       \IN  argument vector
 *
 *  \return	          success (0) or error (1)
 */
int main(int argc, char *argv[])
{
	MDIS_PATH path;
	int32	n, delay, blkMode;
	int32	toggleCh, toggleAll;
	int32	getCh, getAll;
	int32	highCh, lowCh;
	int32	irqCh, irqAll, rEdge, fEdge;
	char	*device,*str,*errstr,buf[40];

	/*--------------------+
    |  check arguments    |
    +--------------------*/
	if ((errstr = UTL_ILLIOPT("t=Tg=Gh=l=i=Irfb=d=?", buf))) {	
		printf("\a*** %s\n", errstr);
		return(1);
	}

	/* help requested ? */
	if( UTL_TSTOPT("?") || argc<3 ){						
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

	toggleAll	= (UTL_TSTOPT("T") ? 1 : 0);
	toggleCh	= ((str = UTL_TSTOPT("t=")) ? atoi(str) : CH_NOT);
	highCh		= ((str = UTL_TSTOPT("h=")) ? atoi(str) : CH_NOT);
	lowCh		= ((str = UTL_TSTOPT("l=")) ? atoi(str) : CH_NOT);
	getAll		= (UTL_TSTOPT("G") ? 1 : 0);
	getCh		= ((str = UTL_TSTOPT("g=")) ? atoi(str) : CH_NOT);
	irqCh		= ((str = UTL_TSTOPT("i=")) ? atoi(str) : CH_NOT);
	irqAll		= (UTL_TSTOPT("I") ? 1 : 0);
	rEdge		= (UTL_TSTOPT("r") ? 1 : 0);
	fEdge		= (UTL_TSTOPT("f") ? 1 : 0);
	blkMode		= ((str = UTL_TSTOPT("b=")) ? atoi(str) : M_BUF_USRCTRL);
	delay		= ((str = UTL_TSTOPT("d=")) ? atoi(str) : 0);

	/*--------------------+
    |  open path          |
    +--------------------*/
	if ((path = M_open(device)) < 0) {
		PrintMdisError("M_open");
		return(1);
	}
#ifdef SIGHDLR_CAN_CALL
	G_Path = path;
#endif

	/*--------------------+
    |  toggle             |
    +--------------------*/
	/* toggle all channels */
	if( toggleAll ){
	
		u_int8 dataBuf[M66_MAX_CH+1] =
			{0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};
		u_int8 *dataBufP = dataBuf;
		u_int32 count=0;

		printf("Toggling all channels\n");
		
		/* set initial values */
		if ((M_setblock( path, dataBufP, M66_MAX_CH )) < 0) {
			PrintMdisError("M_setblock");
			goto abort;
		}

		printf("  Initial values set to ch0=0, ch1=1, ch2=0, ...\n");
		printf("  >> Press any key to start toggling!\n");
		
		/* === wait here === */
		UOS_KeyWait();

		printf("  Toggling (0/1/0/.. -#1-> 1/0/1/.. -#2-> 0/1/0/.. -#n-> ..)\n");
		printf("  >> Press any key to abort!\n");

		/* endless loop */
		do {
			if( dataBufP == dataBuf )
				dataBufP++;
			else
				dataBufP--;

			if ((M_setblock( path, dataBufP, M66_MAX_CH )) < 0) {
				PrintMdisError("M_setblock");
				goto abort;
			}
			count++;

			printf("\r  State changes (0/1/0/.. <--> 1/0/1/..): #%d, current state: %d/%d/%d/..",
				(int)count, dataBufP[0], dataBufP[1], dataBufP[2] );

			if( delay )
				UOS_Delay( delay );

		} while( UOS_KeyPressed()==-1 );

		/* === wait here === */
		printf("\n  >> Done, press any key to close path and exit!\n");
		UOS_KeyWait();

		goto abort;
	}
	/* toggle specified channel */
	if ( toggleCh != CH_NOT ){

		int32	val=0;
		u_int32 count=0;

		printf("Toggling channel %d\n", (int)toggleCh);

		/* select channel */
		if ((M_setstat(path, M_MK_CH_CURRENT, toggleCh)) < 0) {
			PrintMdisError("M_setstat M_MK_CH_CURRENT");
			goto abort;
		}

		/* set initial value */
		if ((M_write(path,val)) < 0) {
			PrintMdisError("M_write");
			goto abort;
		}
		printf("  Initial value set to %d\n", (int)val);
		printf("  >> Press any key to start toggling!\n");
		
		/* === wait here === */
		UOS_KeyWait();

		printf("  Toggling (%d -#1-> %d -#2-> %d -#n-> ..)\n",
			(int)val, !val, (int)val);
		printf("  >> Press any key to abort!\n");


		/* endless loop */
		do {
			val = !val;

			if ((M_write(path,val)) < 0) {
				PrintMdisError("M_write");
				goto abort;
			}
			count++;

			printf("\r  State changes (0 <--> 1): #%d, current state: %d",
				(int)count, (int)val);

			if( delay )
				UOS_Delay( delay );

		} while( UOS_KeyPressed()==-1 );

		/* === wait here === */
		printf("\n  >> Done, press any key to close path and exit!\n");
		UOS_KeyWait();

		goto abort;
	}/* toggle */

	/*--------------------+
    |  set states         |
    +--------------------*/
	/*  set specified channel state */
	if( (highCh != CH_NOT) ||
		(lowCh  != CH_NOT)){

		int32 setCh, setVal;

		if( highCh != CH_NOT ){
			setCh = highCh;
			setVal = 1;
		}
		else{
			setCh = lowCh;
			setVal = 0;
		}
		
		printf("Set state of channel %d to %d\n", (int)setCh, (int)setVal);
		
		if ((M_setstat(path, M_MK_CH_CURRENT, setCh)) < 0) {
			PrintMdisError("M_setstat M_MK_CH_CURRENT");
			goto abort;
		}

		if ((M_write(path,setVal)) < 0) {
			PrintMdisError("M_write");
			goto abort;
		}

		/* === wait here === */
		printf("\n  >> Done, press any key to close path and exit!\n");
		UOS_KeyWait();

		goto abort;
	}/* set */

	/*--------------------+
    |  get states         |
    +--------------------*/
	/* get all channel states */
	if( getAll ){
	
		u_int8 dataBuf[M66_MAX_CH];
		u_int32 ch;
		
		printf("Get all channel states\n");

		if ((M_getblock( path, dataBuf, M66_MAX_CH )) < 0) {
			PrintMdisError("M_getblock");
			goto abort;
		}

		printf("  ch | state | rising edge | falling edge\n");
		printf("  ---+-------+-------------+-------------\n");
		for( ch=0; ch<M66_MAX_CH; ch++ ){
			printf(" %2d  |   %d   |      %d      |       %d      \n",
				(int)ch,
				dataBuf[ch]    & 0x01,
				dataBuf[ch]>>1 & 0x01,
				dataBuf[ch]>>2 & 0x01);
		}

		goto abort;
	}
	/*  get specified channel state */
	if ( getCh != CH_NOT ){

		u_int8 dataBuf[M66_MAX_CH];
		u_int32 count=0;
		u_int32 stateChanges = 0, risingChanges = 0, fallingChanges = 0;
		u_int8 stateCurr = 0, risingCurr = 0, fallingCurr = 0;
		
		printf("Get state of channel %d\n", (int)getCh);
		printf("  >> Press any key to abort!\n");
		
		printf("  count | state | rising edge | falling edge\n");
		printf("  ------+-------+-------------+-------------\n");

		/* endless loop */
		do {
			if ((M_getblock( path, dataBuf, M66_MAX_CH )) < 0) {
				PrintMdisError("M_getblock");
				goto abort;
			}

			/* count state and edge changes */
			if( count == 0 ){
				stateCurr   = dataBuf[getCh]    & 0x01;
				risingCurr  = (u_int8)(dataBuf[getCh]>>1) & 0x01;
				fallingCurr = (u_int8)(dataBuf[getCh]>>2) & 0x01;
			}
			else{
				if( stateCurr   != (dataBuf[getCh]  & 0x01) ){
					stateCurr    = (dataBuf[getCh]  & 0x01);
					stateChanges++;
				}
				if( risingCurr  != ((dataBuf[getCh]>>1) & 0x01) ){
					risingCurr   = ((u_int8)(dataBuf[getCh]>>1) & 0x01);
					risingChanges++;
				}
				if( fallingCurr != ((dataBuf[getCh]>>2) & 0x01 ) ){
					fallingCurr  = ((u_int8)(dataBuf[getCh]>>2) & 0x01);
					fallingChanges++;
				}
			}

			count++;

			printf("\r  %4d  |    %d  |       %d     |        %d",
				(int)count,
				dataBuf[getCh]    & 0x01,
				dataBuf[getCh]>>1 & 0x01,
				dataBuf[getCh]>>2 & 0x01);

			if( delay )
				UOS_Delay( delay );

		} while( UOS_KeyPressed()==-1 );

		printf("\n  changes:   %d  |       %d     |        %d\n",
		(int)stateChanges, (int)risingChanges, (int)fallingChanges);

		goto abort;
	}/* get */

	/*--------------------+
    |  IRQ                |
    +--------------------*/
	if( (irqCh != CH_NOT) || irqAll ){
#ifdef SIGHDLR_CAN_CALL
		int32 nbrOfReadBlks = 0;
#endif
		u_int32	ch, chStart, chEnd, remainingEdges;
		int32	irqCount, readBufCounter, remainingBlks, n;
		u_int8	dataBuf[M66_MAX_CH];
		
		if( (irqCh != CH_NOT) && irqAll ){
			printf("\a*** option -i=<chan> and -I cannot be used together\n");
			goto abort;
		}

		if( !rEdge && !fEdge ){
			printf("\a*** option -i=<chan>/-I requires option -r and/or -f\n");
			goto abort;
		}

		if( irqAll )
			printf("Handle IRQs of all channels\n");
		else
			printf("Handle IRQs of channel %d\n", (int)irqCh);

		/* install signal handler */
		if( UOS_SigInit(SigHandler) ){
			PrintUosError("UOS_SigInit");
			goto abort;
		}
	
		/* install signal for edges */
		if( UOS_SigInstall(UOS_SIG_USR1) ){
			PrintUosError("UOS_SigInstall UOS_SIG_USR1");
			goto abort;
		}

		/* install signal for buffer highwater */
		if( UOS_SigInstall(UOS_SIG_USR2) ){
			PrintUosError("UOS_SigInstall UOS_SIG_USR2");
			goto abort;
		}

		/* configure channels(s) */
		if( irqAll ){
			chStart = 0;
			chEnd = M66_MAX_CH;
		}
		else{
			chStart = irqCh;
			chEnd = irqCh+1;
		}

		for( ch=chStart; ch<chEnd; ch++ ){

			if ((M_setstat(path, M_MK_CH_CURRENT, ch)) < 0) {
				PrintMdisError("M_setstat M_MK_CH_CURRENT");
				goto abort;
			}

			if ((M_setstat(path, M66_EDGE_MASK, (fEdge<<1) | rEdge)) < 0) {
				PrintMdisError("M_setstat M66_EDGE_MASK");
				goto abort;
			}
		}

		/* reset interrupt counter */
		if ((M_setstat(path, M_LL_IRQ_COUNT, 0)) < 0) {
			PrintMdisError("M_setstat M_LL_IRQ_COUNT");
			goto abort;
		}

		/* activate edge signal */
		if ((M_setstat(path, M66_SIG_EDGE_OCCURRED, UOS_SIG_USR1)) < 0) {
			PrintMdisError("M_setstat M66_SIG_EDGE_OCCURRED");
			goto abort;
		}

		/* activate buffer highwater signal */
		if( (M_setstat(path, M_BUF_RD_SIGSET_HIGH, UOS_SIG_USR2)) < 0) {
			PrintMdisError("setstat M_BUF_RD_SIGSET_HIGH");
			goto abort;
		}

		/* set read-buffer timeout to minimum */
		if( (M_setstat(path, M_BUF_RD_TIMEOUT, 1)) < 0) {
			PrintMdisError("setstat M_BUF_RD_TIMEOUT");
			goto abort;
		}

#ifdef SIGHDLR_CAN_CALL
		/* our starting point: no edges occured */
		for( ch=chStart; ch<M66_MAX_CH; ch++ ){
			G_BufRead[0][ch] = 0x00;
		}
#endif
		/* set mode for M_getblock */
		if ((M_setstat( path, M_BUF_RD_MODE, blkMode )) < 0) {
			PrintMdisError("M_setstat M_BUF_RD_MODE");
			goto abort;
		}

		/* enable interrupt */
		if ((M_setstat(path, M_MK_IRQ_ENABLE, 1)) < 0) {
			PrintMdisError("M_setstat M_MK_IRQ_ENABLE 1");
			goto abort;
		}

		/* === wait here === */
		printf("  >> Press any key to abort!\n");

	        do{
	            UOS_Delay(100);
	        } while( UOS_KeyPressed() == (-1) );

		/* disable interrupt */
		if ((M_setstat(path, M_MK_IRQ_ENABLE, 0)) < 0) {
			PrintMdisError("M_setstat M_MK_IRQ_ENABLE 0");
			goto abort;
		}

		/* deactivate signal sending */
		if ((M_setstat(path, M66_SIG_CLR_EDGE_OCCURRED, 0)) < 0) {
			PrintMdisError("M_setstat M66_SIG_EDGE_OCCURRED");
			goto abort;
		}

		/* deinstall signals */
		if( UOS_SigRemove(UOS_SIG_USR1) ){
			PrintUosError("UOS_SigRemove");
			goto abort;
		}
		if( UOS_SigRemove(UOS_SIG_USR2) ){
			PrintUosError("UOS_SigRemove");
			goto abort;
		}

		/* deinstall signal handler */
		if( UOS_SigExit()){
			PrintUosError("UOS_SigExit");
			goto abort;
		}

		/* read interrupt counter */
		if ((M_getstat(path, M_LL_IRQ_COUNT, &irqCount)) < 0) {
			PrintMdisError("M_getstat M_LL_IRQ_COUNT");
			goto abort;
		}

		printf("\n\n");

		/*
		 * get any remaining buffer entries
		 */

		/* read buffer counter */
		if ((M_getstat(path, M_BUF_RD_COUNT, &readBufCounter)) < 0) {
			PrintMdisError("M_getstat M_BUF_RD_COUNT");
			goto abort;
		}

		printf("  Remaining bytes in read buffer: %5d\n", (int)readBufCounter);

		remainingEdges = 0;
		remainingBlks = readBufCounter/M66_MAX_CH;

		for( n=0; n<remainingBlks; n++ ){

			if ((M_getblock( path,  dataBuf, M66_MAX_CH )) < 0) {
				PrintMdisError("M_getblock");
			}

			/* count state changes of all channels */
			for( ch=0; ch<M66_MAX_CH; ch++ ){
				
				/* rising edge occured? */
				if( dataBuf[ch] & M66_OUTLH ){
					remainingEdges++;
					G_BufEdge[BUF_LH][ch]++;
				}
					
				/* falling edge occured? */
				if( dataBuf[ch] & M66_OUTHL ){
					remainingEdges++;
					G_BufEdge[BUF_HL][ch]++;
				}
			}
		}

		printf("  Remaining edge-signals:         %5d\n", (int)remainingBlks);
		printf("  Remaining reported-edges:       %5d\n\n", (int)remainingEdges);

#ifdef SIGHDLR_CAN_CALL
		nbrOfReadBlks = G_SigCountEdges + remainingBlks;
		printf("  Number of read 32-byte blocks:  %5d\n", nbrOfReadBlks);
		printf("  Number of multiple ch edges:    %5d\n\n", G_MultipleChEdges);
#endif

		/* count state changes of all channels */
		for( ch=0; ch<M66_MAX_CH; ch++ ){
			if( G_BufEdge[BUF_LH][ch] || G_BufEdge[BUF_HL][ch] )
				printf("  ch=%2d: rises=%4d, descents=%4d, sum=%4d\n",
					(int)ch, (int)G_BufEdge[BUF_LH][ch],  (int)G_BufEdge[BUF_HL][ch],
					    (int)(G_BufEdge[BUF_LH][ch] + G_BufEdge[BUF_HL][ch]));			
		}
		printf("\n");
		
		printf("  Edge-signals      : %5d\n", (int)G_SigCountEdges);
		printf("  Highwater-signals : %5d\n", (int)G_SigCountHighwater);
		printf("  Other-signals     : %5d\n", (int)G_SigCountOthers);
		printf("  ISR-calls         : %5d\n", (int)irqCount);

#ifdef SIGHDLR_CAN_CALL
		if( nbrOfReadBlks != irqCount )
			printf("  *** WARNING: Number of read 32-byte blocks != ISR-calls\n");
#endif

		goto abort;
	
	}/* IRQ */

	/*--------------------+
    |  cleanup            |
    +--------------------*/
	abort:

	if (M_close(path) < 0)
		PrintMdisError("M_close");

	return(0);
}

/********************************* SigHandler *******************************/
/** Signal handler
 *
 *  \param sigCode       \IN  signal code received
*/
static void __MAPILIB SigHandler(u_int32 sigCode)
{
	switch (sigCode){
	
	/* edge signal */
	case  UOS_SIG_USR1:
			
		G_SigCountEdges++;

#ifdef SIGHDLR_CAN_CALL
	{
		u_int32		iCurr, iLast, chWithEdges=0;
		u_int8		ch;
		
		iCurr = G_SigCountEdges & 0x01;
		iLast = !iCurr;

		if ((M_getblock( G_Path,  G_BufRead[iCurr], M66_MAX_CH )) < 0)
			PrintMdisError("M_getblock");

		/* count state changes of all channels */
		for( ch=0; ch<M66_MAX_CH; ch++ ){
		
			/* no edge occured? */
			if( !(G_BufRead[iCurr][ch] & (M66_OUTLH | M66_OUTHL)) )
				continue;
			
			chWithEdges++;

			/* rising edge occured? */
			if( G_BufRead[iCurr][ch] & M66_OUTLH ){
				G_BufEdge[BUF_LH][ch]++;
				G_AllReportedEdges++;
			}
			
			/* falling edge occured? */
			if( G_BufRead[iCurr][ch] & M66_OUTHL ){
				G_BufEdge[BUF_HL][ch]++;
				G_AllReportedEdges++;
			}

			/*--------------------+
			|  validate IN value  |
			+--------------------*/
			/* only one edge occured? */
			if( (G_BufRead[iCurr][ch] & (M66_OUTLH | M66_OUTHL)) !=
				(M66_OUTLH | M66_OUTHL) ){

				/* rising edge? */
				if( (G_BufRead[iCurr][ch]) & M66_OUTLH ){
					if( ((G_BufRead[iCurr][ch]) & M66_OUTVAL) != M66_OUTVAL )
						printf("\n  *** WARNING: ch=%d, only L->H but IN=0\n", ch);
				}
				/* falling edge! */
				else{
					if( ((G_BufRead[iCurr][ch]) & M66_OUTVAL) == M66_OUTVAL )
						printf("\n  *** WARNING: ch=%d, only H->L but IN=1\n", ch);
				}
			}
			/* rising and falling edge occured! */
			else{
				/* Note: IN value is undetermined! */
			}
	
/*
 * The comparison of the last read value with the current read value
 * requires "slow IN state changes". Short IN changes may be lost.
 */
#if 0
			if(  (G_BufRead[iLast][ch] & M66_OUTHL) &&	// last: L->H
				 (G_BufRead[iCurr][ch] & M66_OUTHL) &&	// now:  L->H
				!(G_BufRead[iCurr][ch] & M66_OUTLH) )	// now:  H->L missing
				printf("\n  *** WARNING: ch=%d, L->H => L->H without H-L\n");

			if(  (G_BufRead[iLast][ch] & M66_OUTLH) &&	// last: H->L
				 (G_BufRead[iCurr][ch] & M66_OUTLH) &&	// now:  H->L
				!(G_BufRead[iCurr][ch] & M66_OUTHL) )	// now:  L->H missing
				printf("\n  *** WARNING: ch=%d, H->L => H->L without L->H\n");
#endif
		} /* for */

		printf("\r  Edge-signals=%d, reported-edges=%d, %d ch(s) reported the edges",
			G_SigCountEdges, G_AllReportedEdges, chWithEdges);

		if( chWithEdges > 1 )
			G_MultipleChEdges++;
	}

#else
		printf("\r  Edge-signals=%d",
			(int)G_SigCountEdges);
#endif
		break;

	/* highwater signal */
	case  UOS_SIG_USR2:

		G_SigCountHighwater++;

#ifdef SIGHDLR_CAN_CALL
		if( G_SigCountHighwater == 10 ){
			
			printf("\a\n\n  *** ERROR:  10 buffer highwater signals received\n");

			/* disable interrupt */
			if ((M_setstat(G_Path, M_MK_IRQ_ENABLE, 0)) < 0) 
				PrintMdisError("M_setstat M_MK_IRQ_ENABLE 0");

			UOS_SigMask();
			
			printf("  >> IRQs disabled, signals masked, press any key to abort!\n");
		}
		else
#endif
			printf("\n  *** WARNING:  buffer highwater signal received\n"
			       "  >> Press any key to abort!\n");			
		break;

	default:
		G_SigCountOthers++;
	}
}

/********************************* PrintMdisError **************************/
/** Print MDIS error message
 *
 *  \param info       \IN  info string
*/
static void PrintMdisError(char *info)
{
	printf("\a\n*** can't %s: %s\n", info, M_errstring(UOS_ErrnoGet()));
}

/********************************* PrintUosError ***************************/
/** Print User OSS error message
 *
 *  \param info       \IN  info string
*/
static void PrintUosError(char *info)
{
	printf("\a\n*** can't %s: %s\n", info, UOS_ErrString(UOS_ErrnoGet()));
}





