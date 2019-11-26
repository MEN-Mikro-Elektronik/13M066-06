/****************************************************************************
 ************                                                    ************
 ************	                    m66_demo.c                   ************
 ************                                                    ************
 ***************************************************************************/
/*!
 *       \author ds
 *
 *		  \brief Demonstration program for M66 with demo adapter
 *
 *     Required: -
 *     \switches NO_MAIN_FUNC
 */
/*
 *---------------------------------------------------------------------------
 * Copyright 1998-2019, MEN Mikro Elektronik GmbH
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
#include <string.h>

#include <MEN/usr_oss.h>
#include <MEN/mdis_api.h>
#include <MEN/mdis_err.h>

static const char IdentString[]=MENT_XSTR(MAK_REVISION);

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

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
static int M66_Demo( char *devName );
static void ShowError( void );


/******************************** main *************************************/
/** main() - function
 *
 *---------------------------------------------------------------------------
 *  \param argc		\IN number of arguments
 *	\param *argv[]	\IN pointer to arguments 
 *
 *  \return	0 if no error |	1 if error
 *
 
 ****************************************************************************/
int main( int argc, char *argv[ ] )
{
	if( argc < 2){
		printf("usage: m66_demo <device name>\n");
		return 1;
	}
	M66_Demo(argv[1]);
	return 0;
}


/******************************* M66_Demo **********************************/
/** Demo Tool for M66.
 *				  - open the device
 *				  - configure the device
 *				  - write and read operations
 *				  - close the device
 *
 *---------------------------------------------------------------------------
 *  \param devName		\IN Device Name
 *
 *  \return	0 if no error |	1 if error
 *
 *  Globals....:  -
 ****************************************************************************/
static int M66_Demo( char *devName )
{
    MDIS_PATH  path;
    int32  ch, i, j, value;
    int32  run = 10;

	/* define matrix for 7-segment display						 */
	/*						  0  1  2  3  4  5  6				 */
	u_int8  data[11][7] = {	{ 1, 1, 1, 1, 1, 1, 0 },	/* 0	 */
							{ 0, 1, 1, 0, 0, 0, 0 },	/* 1	 */
							{ 1, 1, 0, 1, 1, 0, 1 },	/* 2	 */
							{ 1, 1, 1, 1, 0, 0, 1 },	/* 3	 */
							{ 0, 1, 1, 0, 0, 1, 1 },	/* 4	 */
							{ 1, 0, 1, 1, 0, 1, 1 },	/* 5	 */
							{ 1, 0, 1, 1, 1, 1, 1 },	/* 6	 */
							{ 1, 1, 1, 0, 0, 0, 0 },	/* 7	 */
							{ 1, 1, 1, 1, 1, 1, 1 },	/* 8	 */
							{ 1, 1, 1, 1, 0, 1, 1 },	/* 9	 */
							{ 0, 0, 0, 0, 0, 0, 0 } };	/* clear */


    printf("\n");
	printf("m66_demo - demonstration program for the M66 module\n");
    printf("===================================================\n\n");

    printf("%s\n\n", IdentString);

	/*---------------------------+  
    | open the device            |
    +---------------------------*/
	if( (path = M_open(devName)) < 0 ) goto CLEANUP;
	printf("Device %s opened\n\n", devName);

	/*---------------------------+  
    | device configuration       |
    +---------------------------*/
	/* set non auto-increment mode */
	printf("M_setstat() - set non auto-increment mode\n\n");
	if( M_setstat( path, M_MK_IO_MODE, M_IO_EXEC ) < 0 ) goto CLEANUP;

	/*---------------------------+  
    | write number, toggle, read |
    +---------------------------*/
	while(run--)
	{
		/* output number */
		for( i=0; i<11; i++ )
		{
			if(i<10) printf("M_setblock() - output number %d on channel 0..6\n",(int)i);
			if( M_setblock( path, data[i], 7 ) < 0) goto CLEANUP;

			/* toggle point segment */
			printf("toggle point segment (channel 7)\n");
			if( M_setstat(path, M_MK_CH_CURRENT, 7) < 0 ) goto CLEANUP;
			for( j=0; j<11; j++)
			{
				if( M_write(path, j%2) < 0 ) goto CLEANUP;
				UOS_Delay( 100 );
			}

			/* read values back */
			printf("read values back (channel 0..6)  : ");
			for( ch=0; ch<7; ch++)
			{
				if( M_setstat(path, M_MK_CH_CURRENT, ch) < 0 ) goto CLEANUP;
				if( M_read(path, &value ) < 0 ) goto CLEANUP;
				printf(" %d", (int)value);
			}
			printf("\n\n");
		}

		/*---------------------------+  
		|  rotate segments (I)       |
		+---------------------------*/
		printf("rotate segments (I)\n");
		/* configuration - set auto-increment mode */
		if( M_setstat( path, M_MK_IO_MODE, M_IO_EXEC_INC ) < 0 ) goto CLEANUP;

		/* set segments */
		if( M_setstat(path, M_MK_CH_CURRENT, 0) < 0 ) goto CLEANUP;
		for( j=0; j<6; j++)
		{
			if( M_write(path, 1 ) < 0 ) goto CLEANUP;
			UOS_Delay( 120 );
		}
		/* clear segments */
		if( M_setstat(path, M_MK_CH_CURRENT, 0) < 0) goto CLEANUP;
		for( j=0; j<6; j++)
		{
			if( M_write(path, 0 ) < 0 ) goto CLEANUP;
			UOS_Delay( 120 );
		}

		/* configuration - set non auto-increment mode */
		if( M_setstat( path, M_MK_IO_MODE, M_IO_EXEC ) < 0 ) goto CLEANUP;

		/*---------------------------+  
		|  rotate segments (II)      |
		+---------------------------*/
		printf("rotate segments (II)\n\n");
		for(i=0; i<12; i++)
		{
			for(ch=0; ch<6; ch++)
			{
				if( M_setstat(path, M_MK_CH_CURRENT, ch) < 0 ) goto CLEANUP;

				/* set channel ch to 1 */
				if( M_write(path, 1 ) < 0 ) goto CLEANUP;
				UOS_Delay( 120-(i*10+ch) );
				/* set channel ch to 0*/
				if( M_write(path, 0 ) < 0 ) goto CLEANUP;
			}
		}
	}/* while(run--) */
    
    /*----------------------+  
    | close the device      |
    +-----------------------*/
    if( M_close(path) < 0) goto CLEANUP;
	printf("\nDevice %s closed\n", devName);

    printf("=> OK\n");
    return 0;
	
	
CLEANUP:
    ShowError();
    printf("=> Error\n");
    return 1;
}

/******************************** ShowError ********************************/
/** Show MDIS or OS error message.
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *
 *  Output.....:  -
 *
 *  Globals....:  -
 ****************************************************************************/
static void ShowError( void )
{
   u_int32 error;

   error = UOS_ErrnoGet();

   printf("*** %s ***\n",M_errstring( error ) );
}



