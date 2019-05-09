/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: m66_main.c
 *      Project: MDIS 4.x
 *
 *       Author: franke
 *        $Date: 2009/06/24 11:09:32 $
 *    $Revision: 1.13 $
 *
 *  Description: test of the m66_drv.c m66_drv.h
 *               signal handling and buffer modes
 *
 *
 * !!!!!!!!!!!!!!!!!!! IMPORTANT NOTE: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * Modify the max descriptor in order to use this program:
 *	CHANNEL_2/IRQ_ENABLE = U_INT32 1
 *	CHANNEL_5/IRQ_ENABLE = U_INT32 3
 * all other CHANNEL_x/IRQ_ENABLE must be zero!!
 *
 *     Required: -
 *     Switches:
 *
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: m66_main.c,v $
 * Revision 1.13  2009/06/24 11:09:32  CRuff
 * R: new MDIS path type MDIS_PATH
 * M: changed type of filedescriptor for M_open to MDIS_PATH
 *
 * Revision 1.12  2009/05/08 09:42:27  dpfeuffer
 * R: sighdl() uses wrong calling convention under Windows
 * M: __MAPILIB keyword added to sighdl()
 *
 * Revision 1.11  2009/03/30 14:30:40  ufranke
 * cosmetics
 *
 * Revision 1.10  2008/09/17 18:15:05  CKauntz
 * R: No 64 bit support
 * M: Changed fd to INT32_OR_64 to support 64 bit OS
 *
 * Revision 1.9  2008/09/15 15:17:25  ufranke
 * R: diab compiler warnings
 * M: cosmetics
 *
 * Revision 1.8  2008/01/18 14:28:12  ufranke
 * cosmetics
 *
 * Revision 1.7  2005/03/04 09:31:48  ufranke
 * fixed
 *  + missing UOS_SigExit()
 *
 * Revision 1.6  2004/08/30 15:43:49  dpfeuffer
 * minor modifications for MDIS4/2004 conformity
 *
 * Revision 1.5  2004/08/30 14:22:46  agromann
 * VXWORKS switch removed
 *
 * Revision 1.4  2001/06/26 15:53:33  franke
 * cosmetics NO_MAIN_FUNC removed
 *
 * Revision 1.3  2000/08/09 14:09:21  kp
 * - Call UOS_MikroDelayInit(). Otherwise UOS_MikroDelay fails!
 * - Added note about descriptor
 * - added printout of occurred interrupts
 *
 * Revision 1.2  1998/07/15 15:36:46  Franke
 * update to MDIS 4.1
 *
 * Revision 1.1  1998/02/19 14:39:42  Schmidt
 * Added by mcvs
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1998..2009 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static char *RCSid="$Id: m66_main.c,v 1.13 2009/06/24 11:09:32 CRuff Exp $";

#include <MEN/men_typs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <MEN/usr_oss.h>
#include <MEN/mdis_api.h>

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

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
static int  SigNo;
static int  SignalCount;

static int m66_openclose( char *devName );
static int m66_test( char *devName  );


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
        printf("usage: m66_main <device name>\n");
        return 1;
    }

    printf("\tnotice:\n");
    printf("\tneeds following descriptor entries\n"
           "\t channel  2 irq on rising  edge\n"
           "\t channel  5 irq on both    edges\n"
           "\t channel 31 irq on falling edge\n" );

    m66_openclose(argv[1]);
    m66_test(argv[1]);
    return 0;
 }

/***************************** sighdl ***************************************
 *
 *  Description: Signal handler.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  sigNo  signal number
 *
 *  Output.....:  -
 *
 *  Globals....:  SigNo, SignalCount
 *
 ****************************************************************************/
static void __MAPILIB sighdl( u_int32 sigNo )
{
    SigNo = sigNo;
    SignalCount++;
}


/***************************** errShow ***************************************
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
}/*errShow*/

/**************************** CheckInteger **********************************
 *
 *  Description:  Compare integers.
 *
 *
 *---------------------------------------------------------------------------
 *  Input......:  soll    cmp value
 *                ist     current value
 *                cmpType '=' | '<' | '>'
 *                quit    -
 *
 *  Output.....:  return  T_OK or T_ERROR
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
static int32 CheckInteger( int32 soll, int32 ist, char cmpType, int32 quit )
{
    int32 ret = T_OK;

    switch( cmpType )
    {
        case '<':
           if( soll < ist )
           {
               printf("    => OK\n");
           }
           else
           {
               printf(" '=' failed %d %d\n", (int)soll, (int)ist );
               ret = T_ERROR;
           }/*if*/
           break;

        case '>':
           if( soll > ist )
           {
               printf("    => OK\n");
           }
           else
           {
               printf(" '=' failed %d %d\n", (int)soll, (int)ist );
               ret = T_ERROR;
           }/*if*/
           break;

        case '=':
        default:
           if( soll == ist )
           {
               if( !quit )
                   printf("    => OK\n");
           }
           else
           {
               printf(" '==' failed %d %d\n", (int)soll, (int)ist );
               ret = T_ERROR;
           }/*if*/

    }/*switch*/

    return( ret );
}/*CheckInteger*/


/**************************** m66_openclose *********************************
 *
 *  Description:  Open and close the device. Read the SW-Revision ID's
 *                and the M-Modul ID.
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
static int m66_openclose( char *devName )
{
   int    ret = T_OK;
   MDIS_PATH path;
   int32  size;
   char   *buf;
   M_SETGETSTAT_BLOCK blkStruct;
   u_int16 *dataP;
   u_int32 i;
   u_int32 maxWords;

   printf("%s\n", RCSid);
   printf("=========================\n");
   printf("m66 - open close \n");
   printf("    open\n");
   path = M_open( devName );
   if( path >= 0 )
   {
       printf("    => OK\n");

       printf("    revision id's\n");
       size = 4096;
       buf = (char*) malloc( size );
       if( buf )
       {
           *buf = 0;
           blkStruct.size = size;
           blkStruct.data = buf;

           if( M_getstat( path, M_MK_BLK_REV_ID, (int32*) &blkStruct ) )
           {
               errShow();
               printf("    => Error\n");
               ret = T_ERROR;
           }
           else
           {
               printf("    => OK\n");
           }/*if*/

           printf( buf );

           printf("    module id\n");
           maxWords = 128;
           blkStruct.size = maxWords*2;
           if( M_getstat( path, M_LL_BLK_ID_DATA, (int32*) &blkStruct ) )
           {
               errShow();
               printf("    => Error\n");
               ret = T_ERROR;
           }
           else
           {
               printf("    => OK\n");
               dataP = (u_int16*) blkStruct.data;
               for( i=1; i<=maxWords; i++ )
               {
                   printf( " 0x%04x", (int)*dataP );
                   if( !(i%8) )
                       printf("\n");
                   dataP++;
               }/*for*/
           }/*if*/


           free(buf);
       }
       else
           printf(" can't allocate user buffer\n");





       printf("    close\n");
       if( M_close( path ) == 0 )
       {
           printf("    => OK\n");

       }
       else
       {
           errShow();
           printf("    => Error\n");
           ret = T_ERROR;
       }/*if*/
   }
   else
   {
       errShow();
       printf("    => Error\n");
       ret = T_ERROR;
   }/*if*/

   return( ret );
}/**/

/******************************* m66_mbuf ***********************************
 *
 *  Description:  Open and close the device. Read the current buffer
 *                settings.
 *
 *         Note:  The buffer settings are default values or descriptor
 *                dependend.
 *                The test will fail if the descriptor values are not
 *                equal to the compare values.
 *
 *---------------------------------------------------------------------------
 *  Input......:  devName device name in the system e.g. "/m66/0"
 *
 *  Output.....:  return  T_OK or T_ERROR
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
static int m66_mbuf( char *devName )
{
   int ret = T_OK;
   MDIS_PATH path;
   int32 blkMode;
   int32 bufWidth;
   int32 bufSize;
   int32 timeout;
   int32 highWater;


   printf("=========================\n");
   printf("m66 - mbuf \n");
   printf("    open\n");
   path = M_open( devName );
   if( path >= 0 )
   {
       printf("    => OK\n");

       printf("    blkMode\n");
       ret = M_getstat( path, M_BUF_RD_MODE     , &blkMode ); if( ret ) goto CLEANUP;
       if( CheckInteger( M_BUF_USRCTRL, blkMode, '=', 0 ) ) goto CLEANUP;

       printf("    bufWidth\n");
       ret = M_getstat( path, M_BUF_RD_WIDTH    , &bufWidth );if( ret ) goto CLEANUP;
       if( CheckInteger( M66_MAX_CH, bufWidth, '=', 0 ) ) goto CLEANUP;

       printf("    timeout\n");
       ret = M_getstat( path, M_BUF_RD_TIMEOUT  , &timeout ); if( ret ) goto CLEANUP;
       if( CheckInteger( 1000, timeout, '=', 0 ) ) goto CLEANUP;

       printf("    bufSize\n");
       ret = M_getstat( path, M_BUF_RD_BUFSIZE  , &bufSize ); if( ret ) goto CLEANUP;
       if( CheckInteger( 512, bufSize, '=', 0 ) ) goto CLEANUP;

       printf("    highWater\n");
       ret = M_getstat( path, M_BUF_RD_HIGHWATER, &highWater ); if( ret ) goto CLEANUP;
       if( CheckInteger( 512, highWater, '=', 0 ) ) goto CLEANUP;


CLEANUP:
       if( ret != T_OK )
       {
           errShow();
           printf("    => Error\n");
           ret = T_ERROR;
       }/*if*/

       printf("    close\n");
       if( M_close( path ) == 0 )
       {
           printf("    => OK\n");
       }
       else
       {
           errShow();
           printf("    => Error\n");
           ret = T_ERROR;
       }/*if*/
   }
   else
   {
       errShow();
       printf("    => Error\n");
       ret = T_ERROR;
   }/*if*/

   return( ret );
}/*m66_mbuf*/


/******************************* m66_test ***********************************
 *
 *  Description:  Open and close the device.
 *                Read and write in M_IO_EXEC and M_IO_EXEC_INC mode. (channel)
 *                Check the buffer mode M_BUF_USRCTRL for get/ and setblock.
 *                Check the buffer mode M_BUF_RINGBUF for getblock and irq.
 *                Check signal handling and
 *                correct irq edge enable. ( L->H, H->L, both )
 *                Check the correct irq causing channel.
 *
 *                ( puh - that were all supported features )
 *
 *---------------------------------------------------------------------------
 *  Input......:  devName device name in the system e.g. "/m66/0"
 *
 *  Output.....:  return  T_OK or T_ERROR
 *
 *  Globals....:  ---
 *
 ****************************************************************************/
static int m66_test( char *devName )
{
    MDIS_PATH  path;
    int32  ch;
    int    i;
    int32  wrVal;
    int32  rdVal;
    int32  oldDebugLevel;
    u_int8 buf[2*M66_MAX_CH]; /*usr buffer*/
    int    retCode;
    int    error;
    int32  edgeMask;
    int32  irqCh;
    int    nbrOfReadBytes;
    int32  irqChA;
    int32  irqChB;

	UOS_MikroDelayInit();
    m66_mbuf(devName);

    printf("=========================\n");
    printf("m66 test\n");
    UOS_SigInit( sighdl );                    /* install signal routine */
    UOS_SigInstall( UOS_SIG_USR1 );

    if( (path = M_open(devName)) < 0 ) goto M66_TESTERR;

    /* debug level switch on for interrupt */
    if( M_getstat(path, M_LL_DEBUG_LEVEL, &oldDebugLevel ) ) goto M66_TESTERR;
    oldDebugLevel = oldDebugLevel | 0x02;
    if( M_setstat(path, M_LL_DEBUG_LEVEL, oldDebugLevel ) ) goto M66_TESTERR;

    /*-----------------------------------------+
    |  check read write                        |
    +------------------------------------------*/
    if( M_setstat( path, M_MK_IO_MODE, M_IO_EXEC ) ) goto M66_TESTERR;
    printf("   write read\n");
    error = 0;
    for( ch=0; ch<M66_MAX_CH; ch++)
    {
        M_setstat(path, M_MK_CH_CURRENT, ch);    /* set current channel */
        wrVal = ch & 1;
        M_write(path, wrVal );                  /* write channel       */
        UOS_MikroDelay(500);                  /* wait a little       */
        M_read(path,&rdVal);                    /* read back channel   */
        if( rdVal != wrVal )
        {
           CheckInteger( wrVal, rdVal, '=', 0 );
           error = 1;
        }
    }/*for*/

    if( error )
        printf("    => Error\n");
    else
        printf("    => OK\n");

    /*----------------------------------------------------------------+
    |  check blockread nbrRdBytes and 0101...pattern and rising edge  |
    +----------------------------------------------------------------*/
    printf("   blockread M_BUF_USRCTRL mode\n");
    if( M_setstat( path, M_BUF_RD_MODE, M_BUF_USRCTRL ) ) goto M66_TESTERR;
    error = 0;
    retCode = M_getblock( path, buf, M66_MAX_CH );
    if( retCode < 0 ) goto M66_TESTERR;
    printf("   blockread nbr of read bytes\n");
    if( !CheckInteger( M66_MAX_CH, retCode, '=', 0 ) )
    {
        printf("   blockread input pattern\n");
        for( ch=0; ch<M66_MAX_CH; ch++ )
        {
           if( ch&1 )
               wrVal = 1 | M66_OUTLH;
           else
               wrVal = 0;

           if( wrVal != buf[ch] )
           {
               printf(" input val ch%d\n", (int) ch );
               error = 1;
           }/*if*/
        }/*for*/
    }/*if*/
    if( error )
        printf("    => Error\n");
    else
        printf("    => OK\n");


    /*--------------------------------------------------------------+
    |  check blockwrite nbrRdBytes and 1010...pattern and rise/fall |
    +--------------------------------------------------------------*/
    printf("   blockwrite\n");
    error = 0;
    for( ch=0; ch<M66_MAX_CH; ch++ )
        buf[ch] = (u_int8)((ch+1) & 1);

    retCode = M_setblock( path, buf, M66_MAX_CH );
    if( retCode < 0 ) goto M66_TESTERR;
    UOS_MikroDelay(500);                /* wait a little       */
    printf("   blockwrite nbr of written bytes\n");
    if( !CheckInteger( M66_MAX_CH, retCode, '=', 0 ) )
    {
        M_getblock( path, buf, M66_MAX_CH );
        printf("   blockwrite input pattern\n");
        for( ch=0; ch<M66_MAX_CH; ch++ )
        {
           if( (ch+1)&1 )
               wrVal = 1 | M66_OUTLH;
           else
               wrVal = M66_OUTHL;

           if(  wrVal != buf[ch] )
           {
               printf(" input val ch%d\n", (int) ch );
               error = 1;
           }/*if*/
        }/*for*/
    }/*if*/
    if( error )
        printf("    => Error\n");
    else
        printf("    => OK\n");

    /*-----------------------------------------+
    |  check auto inc mode                     |
    +------------------------------------------*/
    printf("   autoinc on/off\n");
    if( M_setstat(path,M_MK_CH_CURRENT, 0 ) ) goto M66_TESTERR;
    if( M_setstat(path,M_MK_IO_MODE, M_IO_EXEC_INC) ) goto M66_TESTERR;

    wrVal = 1;
    for( i=0; i<2; i++ )
    {
        for( ch=0; ch<M66_MAX_CH; ch++ )
        {
            if( M_write( path, wrVal ) < 0 ) goto M66_TESTERR;
            UOS_Delay( 1 );
        }/*for*/
        for( ch=0; ch<M66_MAX_CH; ch++ )
        {
            if( M_read( path, &rdVal ) < 0 ) goto M66_TESTERR;
            UOS_Delay( 1 );
        }/*for*/
        wrVal = !wrVal;
    }/*for*/

    /*-----------------------------------------+
    |  check no inc mode and irq + edge mask   |
    +------------------------------------------*/
    error = 0;
    printf("    M_BUF_RINGBUF, irq enable and signal\n");


    SignalCount = 0;

    if( M_setstat( path, M66_SIG_EDGE_OCCURRED, UOS_SIG_USR1) ) goto M66_TESTERR;  /* activate condition 1 */

    if( M_setstat( path, M_BUF_RD_MODE, M_BUF_RINGBUF ) ) goto M66_TESTERR;
    if( M_setstat(path,M_MK_IRQ_ENABLE,1) ) goto M66_TESTERR;

    printf("    no autoinc L->H and irq\n");
    if( M_setstat(path,M_MK_IO_MODE, M_IO_EXEC) ) goto M66_TESTERR;
    for( ch=0; ch<M66_MAX_CH; ch++ )
    {
        /* low -> high */
        if( M_setstat(path,M_MK_CH_CURRENT,ch) ) goto M66_TESTERR;
        if( M_write( path, ON ) < 0 ) goto M66_TESTERR;
            UOS_Delay( 1 );

        /* check irqSource == edgeMask */
        if( M_getstat(path, M66_EDGE_MASK, &edgeMask) ) goto M66_TESTERR;
        if( edgeMask & M66_IELH )
        {
            if( M_getstat(path, M66_IRQ_SOURCE, &irqCh) ) goto M66_TESTERR;
			printf("     irq on channel %d occurred\n", (int)irqCh);
            if( irqCh != ch )
            {
                printf("    irqCh%d != ch%d\n", (int)irqCh, (int)ch );
                error = 1;
            }/*if*/
        }/*if*/
    }/*for*/
    if( error )
        printf("    => Error\n");
    else
        printf("    => OK\n");


    /*--------------------------------------------------------------+
    |  check signal count                                           |
    +--------------------------------------------------------------*/
    printf("   check installed signal and signal count\n");
    if( M_getstat( path, M66_SIG_EDGE_OCCURRED, &rdVal ) ) goto M66_TESTERR;
    if( CheckInteger( UOS_SIG_USR1, rdVal, '=', 0 ) )
        printf("    => Error\n");

    if( M_setstat( path, M66_SIG_CLR_EDGE_OCCURRED, UOS_SIG_USR1 ) ) goto M66_TESTERR;
    if( CheckInteger( 2, SignalCount, '=', 0 ) )
        printf("    => Error\n");


    /*--------------------------------------------------------------+
    |  check blockread                                              |
    +--------------------------------------------------------------*/
    printf("   blockread M_BUF_RINGBUF mode\n");

    error = 0;
    nbrOfReadBytes = M_getblock( path, buf, (2 * M66_MAX_CH) );
    if( nbrOfReadBytes < 0 ) goto M66_TESTERR;
    error = 0;
    if( nbrOfReadBytes != (2 * M66_MAX_CH) )
    {
         printf(" getblock not enough read %d\n", nbrOfReadBytes );
         error = 1;
    }
    else
    {
        irqChA=2; /* like descriptor ch2 - rising edge enabled */
        irqChB=5; /* like descriptor ch5 - rising & falling edge enabled */
        /* check the buffer
                   ch2 first interrupt
                   |
                   |        ch5 second interrupt
                   |         |
        00 - 03 03 03 00 ... |
        32 - 01 01 01 03 03 03 00 ...
        64 - xx ...
        */
        for( i=0; i<(2*M66_MAX_CH); i++ )
        {
            if( i <= irqChA )
                error = CheckInteger( M66_OUTVAL | M66_OUTLH, buf[i], '=', 1 );
            else
            {
                if( i < M66_MAX_CH )
                    error = CheckInteger( 0, buf[i], '=', 1 );
                else
                {
                    if( (i-M66_MAX_CH) <= irqChA )
                        error = CheckInteger( M66_OUTVAL, buf[i], '=', 1 );
                    else
                    {
                        if( (i-M66_MAX_CH) <= irqChB )
                            error = CheckInteger( M66_OUTVAL | M66_OUTLH, buf[i], '=', 1 );
                        else
                        {
                            error = CheckInteger( 0, buf[i], '=', 1 );
                        }/*if*/
                    }/*if*/
                }/*if*/
            }/*if*/
        }/*for*/

    }/*if*/
    if( error )
        printf("    => Error\n");
    else
        printf("    => OK\n");


    /*-----------------------------------------+
    |  check no inc mode and irq + edge mask   |
    +------------------------------------------*/
    printf("    no autoinc H->L and irq\n");
    error = 0;
    for( ch=0; ch<M66_MAX_CH; ch++ )
    {
        /* high -> low */
        if( M_setstat(path,M_MK_CH_CURRENT,ch) ) goto M66_TESTERR;
        if( M_write( path, OFF ) < 0 ) goto M66_TESTERR;
            UOS_Delay( 1 );

        /* check irqSource == edgeMask */
        if( M_getstat(path, M66_EDGE_MASK, &edgeMask) ) goto M66_TESTERR;
        if( edgeMask & M66_IEHL )
        {
            if( M_getstat(path, M66_IRQ_SOURCE, &irqCh) ) goto M66_TESTERR;
			printf("     irq on channel %d occurred\n", (int)irqCh);

            if( irqCh != ch )
            {
                printf("    irqCh%d != ch%d\n", (int)irqCh, (int)ch );
                error = 1;
            }/*if*/
        }/*if*/
    }/*for*/
    if( error )
        printf("    => Error\n");
    else
        printf("    => OK\n");

    if( M_close( path ) )
    	goto M66_TESTERR;
    UOS_SigExit();

    printf("    => OK\n");
    return( 0 );

M66_TESTERR:
    errShow();
    printf("    => Error\n");
    M_close( path );
    UOS_SigExit();
    return( 1 );

}/*m66_test*/





