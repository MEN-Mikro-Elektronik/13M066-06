/*********************  P r o g r a m  -  M o d u l e **********************/
/*!
 *         \file m66_drv.c
 *      Project: M66 module driver 
 *
 *       \author franke
 *
 *		  \brief Low level driver for M66 M-Modules
 *
 *               The driver supports also D302/A302 M66 onboard devices.
 *
 *     Required: none
 *     \switches _ONE_NAMESPACE_PER_DRIVER_
 */
 /*
 *---------------------------------------------------------------------------
 * Copyright (c) 1995-2019, MEN Mikro Elektronik GmbH
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

#include <MEN/men_typs.h>   /* system dependend definitions   */
#ifndef INT32_OR_64
	#define INT32_OR_64		int32  /* for backward compatibility with old system packages */
#endif

#include <MEN/dbg.h>
#include <MEN/oss.h>        /* os services                    */
#include <MEN/mdis_err.h>   /* mdis error numbers             */
#include <MEN/mbuf.h>       /* buffer lib functions and types */
#include <MEN/maccess.h>    /* hw access macros and types     */
#include <MEN/desc.h>       /* descriptor functions           */
#include <MEN/mdis_api.h>   /* global set/getstat codes       */
#include <MEN/mdis_com.h>   /* info function      codes       */
#include <MEN/modcom.h>     /* id prom access                 */

#include <MEN/ll_defs.h>    /* low level driver definitions   */
#include <MEN/ll_entry.h>   /* low level driver entry struct  */
#include <MEN/m66_drv.h>    /* M66 driver header file         */

static const char IdentString[]=MENT_XSTR(MAK_REVISION);

#include "m66_flex.h"                       /* include data field */

/* pass debug definitions to dbg.h */
#define DBG_MYLEVEL     m66Hdl->dbgLevel

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
/** low-level handle */
typedef struct
{
    int32           ownMemSize;
    u_int32         dbgLevel;
    DBG_HANDLE*     dbgHdl;         /* debug handle */
    OSS_HANDLE      *osHdl;         /* for complicated os */
    MACCESS         maPld;
	MDIS_IDENT_FUNCT_TBL idFuncTbl;	/* id function table */
    MBUF_HANDLE     *inbuf;
    OSS_SIG_HANDLE  *sigHdl;
    OSS_IRQ_HANDLE  *irqHdl;
    u_int8          edgemask[M66_MAX_CH];    /* edge irq enable masks (ch 0..31) */
    u_int16         irqsource;       /* last irq source (channel) */
    u_int32         irqCount;
    u_int32         useModulId;
    u_int8          irqEnabled[M66_MAX_CH];
	u_int8			d302Device;		/* 1:D302/A302 device */ 
} M66_HANDLE;


/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define DBH     m66Hdl->dbgHdl

#define M66_CH_WIDTH    1    /* byte */

#define MOD_ID_MAGIC		0x5346      /* eeprom identification (magic) */
#define MOD_ID_SIZE			128			/* eeprom size */
#define MOD_ID			    66			/* module id */

#define M66_DEFAULT_BUF_SIZE        512   /* byte */
#define M66_DEFAULT_BUF_TIMEOUT    1000   /* ms */
#define M66_DEFAULT_IRQ_DISABLED      0   /* 1 rising, 2 falling, 3 any edge */

#define M66_HW_ACCESS_NO              0
#define M66_HW_ACCESS_PERMITED        1

#define M66_IRQ			1
#define M66_NO_IRQ		2

/*--------------------- M66 register defs/offsets ------------------------*/
#define IOREG(ch)   (ch<<1)             /* channel input regs      */
#define INTREG      0xfe                        /* interrupt status, flex */

/* IOREG masks */
#define D_IELH      0x02                        /* rising  edge irq enable */
#define D_IEHL      0x04                        /* falling edge irq enable */
#define D_IEMSK     0x06                        /* edge enable mask */

#define D_EDLH      0x10                        /* rising  edge occurred */
#define D_EDHL      0x20                        /* falling edge occurred */
#define D_EDMSK     0x30                        /* edge status mask */

#define D_OUTVAL    0x01                        /* pin output switch */
#define D_INVAL     0x08                        /* pin real value */

/* INTREG */
#define I_IRQCH     0x3e                        /* channel mask */

#define M66_FLREG   0xfe                        /* flex load register */

/* D302 specifics */
#define D302_MAGICWORD_REG	0x00FC				/* D302 Magicword Register */

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
static int32 M66_MemCleanup( M66_HANDLE *m66Hdl );

static int32 M66_HwBlockRead
(
    M66_HANDLE *m66Hdl,
    u_int8     *buf,
    int		   calledFromIrq,
    int32      *edgeOccuredP
);

static int32 getStatBlock
(
    M66_HANDLE         *m66Hdl,
    int32              code,
    M_SETGETSTAT_BLOCK *blockStruct
);

static char* M66_Ident( void );
static char* M66_FlexDataIdent( void );
static int32 M66_Init( DESC_SPEC       *descSpec,
					   OSS_HANDLE      *osHdl,
					   MACCESS         *ma,
					   OSS_SEM_HANDLE  *devSemHdl,
					   OSS_IRQ_HANDLE  *irqHdl,
					   LL_HANDLE       **llHdlP );

static int32 M66_Exit( 	      LL_HANDLE **llHdlP );
static int32 M66_Read(        LL_HANDLE *llHdl,  int32 ch, int32 *value );
static int32 M66_Write(       LL_HANDLE *llHdl,  int32 ch, int32 value );
static int32 M66_SetStat(     LL_HANDLE *llHdl,  int32 code, int32 ch, INT32_OR_64 value32_or_64 );
static int32 M66_GetStat(     LL_HANDLE *llHdl,  int32 code, int32 ch, INT32_OR_64 *value32_or_64P );
static int32 M66_BlockRead(   LL_HANDLE *llHdl,  int32 ch,  void  *buf, int32 size, int32 *nbrRdBytesP );
static int32 M66_BlockWrite(  LL_HANDLE *llHdl,  int32 ch,  void  *buf, int32 size, int32 *nbrWrBytesP);
static int32 M66_Irq(         LL_HANDLE *llHdl );
static int32 M66_Info(        int32     infoType, ... );


/*****************************  M66_Ident  *********************************/
/** Gets the pointer to ident string.
 *
 *
 *---------------------------------------------------------------------------
 *  \return  pointer to ident string
 ****************************************************************************/
static char* M66_Ident( void )	
{
    return( (char*)IdentString );
}/*M66_Ident*/


/* single bit setting macros */
#define bitset(byte,mask)  ((byte) |=  (mask))
#define bitclr(byte,mask)  ((byte) &= ~(mask))
#define bitmove(byte,mask,bool) (bool ? bitset(byte,mask) : bitclr(byte,mask))

/*************************  M66_FlexDataIdent  *****************************/
/** Gets the pointer to flex data ident string.
 *
 *
 *---------------------------------------------------------------------------
 *  \return  pointer to ident string
 *
 ****************************************************************************/
static char* M66_FlexDataIdent( void )	
{
    return( (char*)FlexIdentString );
}/*M66_FlexDataIdent*/

/******************************** m66_flexload ******************************/
/** Loads FLEXlogic with binary data.
 *                Binary data is stored in array 'm66_flex' from m66_flex.h.
 *
 *---------------------------------------------------------------------------
 *  \param m66Hdl		\IN m66 low level driver handle
 *  \return   0
 *  
 ****************************************************************************/
static int32 m66_flexload	
(
    M66_HANDLE *m66Hdl
)
{
   u_int8  ctrl,                   /* control word */
           ch, n;                  /* current byte */
   u_int8  tdo;
   u_int8  tck;
   u_int8  tms;
   u_int8  *p = (u_int8*)m66_flex; /* point to binary data */
   u_int32 size;                   /* size of binary data */

   DBGWRT_1((DBH, "LL - m66_flexload\n"));

   size  = (u_int32)(*p++) << 24;   /* get data size */
   size |= (u_int32)(*p++) << 16;
   size |= (u_int32)(*p++) <<  8;
   size |= (u_int32)(*p++);

   tdo = 1 << 0;         /* convert bits to bitmask */
   tck = 1 << 1;
   tms = 1 << 3;

   ctrl = 0x00;            /* control word initial value */

   while(size--)           /* for all bytes */
   {
      ch = *p++;              /* get next data byte */
      n = 4;                  /* data byte: 4*2 bit */

      while(n--)                 /* write data 2 bits */
      {

         bitclr (ctrl,tck);                 /* clear TCK */
         MWRITE_D16( m66Hdl->maPld, M66_FLREG, ctrl );

         bitmove(ctrl,tdo,ch & 0x01);       /* write TDO/TMS bits */
         bitmove(ctrl,tms,ch & 0x02);
         MWRITE_D16( m66Hdl->maPld, M66_FLREG, ctrl );

         bitset (ctrl,tck);                 /* set TCK (pulse) */
         MWRITE_D16( m66Hdl->maPld, M66_FLREG, ctrl );

         ch >>= 2;
      }
   }

   return(0);
}

/**************************** M66_MemCleanup ********************************/
/** Deallocates low-level driver structure,installed signals and buffers.
 *
 *
 *---------------------------------------------------------------------------
 *  \param m66Hdl		\IN m66 low level driver handle
 *	\return   0 | error code
 *
 ****************************************************************************/
static int32 M66_MemCleanup( M66_HANDLE *m66Hdl )	
{
int32       retCode;

    /*--------------------------+
    | remove buffer             |
    +--------------------------*/
    if( m66Hdl->inbuf )
       MBUF_Remove( &m66Hdl->inbuf );

    /*--------------------------+
    | remove installed signal   |
    +--------------------------*/
    /* deinit lldrv memory */
    if( m66Hdl->sigHdl != NULL )
        OSS_SigRemove( m66Hdl->osHdl, &m66Hdl->sigHdl );

    /*-------------------------------------+
    | free ll handle                       |
    +-------------------------------------*/
    /* dealloc lldrv memory */
    retCode = OSS_MemFree( m66Hdl->osHdl, (int8*) m66Hdl, m66Hdl->ownMemSize );

    /* cleanup debug */
    DBGEXIT((&DBH));

    return( retCode );
}/*M66_MemCleanup*/

/**************************** M66_GetEntry ********************************/
/** Gets the entry points of the low-level driver functions.
 *
 *         Note:  Is used by MDIS kernel only.
 *
 *---------------------------------------------------------------------------
 *  \param drvP		\OUT pointer to the initialized structure
 *
 ****************************************************************************/
#ifdef _ONE_NAMESPACE_PER_DRIVER_
    void LL_GetEntry( LL_ENTRY* drvP )
#else
    void __M66_GetEntry( LL_ENTRY* drvP )
#endif /* _ONE_NAMESPACE_PER_DRIVER_ */
{

    drvP->init        = M66_Init;
    drvP->exit        = M66_Exit;
    drvP->read        = M66_Read;
    drvP->write       = M66_Write;
    drvP->blockRead   = M66_BlockRead;
    drvP->blockWrite  = M66_BlockWrite;
    drvP->setStat     = M66_SetStat;
    drvP->getStat     = M66_GetStat;
    drvP->irq         = M66_Irq;
    drvP->info        = M66_Info;

}/*M66_GetEntry*/

/******************************** M66_Init **********************************/
/** Decodes descriptor , allocates and initializes the ll-driver
 *                structure. Loads the PLD and initializes the hardware.
 *                Reads and checks the ID if it is enabled in the descriptor.
 *                Clears and disables the module interrupts.
 *                Switches off all outputs.
 *                The driver supports 32 digital i/o channels.
 *                There may be one read buffer with a width of 32 bytes.
 *
 *   \code             Deskriptor key        Default         Range/Unit
 *
 *                DEBUG_LEVEL_DESC      OSS_DBG_DEFAULT  see oss_os.h
 *                DEBUG_LEVEL_MBUF      OSS_DBG_DEFAULT  see oss_os.h
 *                DEBUG_LEVEL           OSS_DBG_DEFAULT  see oss_os.h
 *
 *                RD_BUF/SIZE           512              0..n*32 in byte
 *
 *                RD_BUF/MODE           MBUF_USR_CTRL    see mbuf.h
 *
 *                RD_BUF/TIMEOUT        1000             1..n in milli sec.
 *
 *                RD_BUF/HIGHWATER      512              0..RD_BUF/SIZE in byte
 *
 *                ID_CHECK              1                0..1 0 - disabled
 *                                                            1 - enabled
 *
 *                CHANNEL_%d/IRQ_ENABLE 0                0..3 0 - disabled
 *                                                            1 - rising  edge
 *                                                            2 - falling edge
 *                                                            3 - any     edge
 *                                                       %d 0..31
 *  \endcode
 *         Note:  Is called by MDIS kernel only.
 *
 *---------------------------------------------------------------------------
 *  \param  descSpec		\IN descriptor specifier
 *  \param  osHdl			\IN pointer to the os specific structure
 *  \param  ma				\IN access handle (in simplest case M66 base address)
 *  \param  devSem			\IN device semaphore for unblocking in wait
 *  \param  irqHdl			\IN irq handle for mask and unmask interrupts
 *  \param  llHdlP			\IN pointer to the variable where low level driver
 *                         handle will be stored
 *
 *  \param *llHdlP			\OUT  low level driver handle
 *  \return   0 | error code
 *
 ****************************************************************************/
static int32 M66_Init
(
    DESC_SPEC       *descSpec,
    OSS_HANDLE      *osHdl,
    MACCESS         *ma,
    OSS_SEM_HANDLE  *devSem,
    OSS_IRQ_HANDLE  *irqHdl,
    LL_HANDLE       **llHdlP
)
{
    DBGCMD( static const char functionName[] = "LL - M66_Init()"; )
    M66_HANDLE *m66Hdl;
    int32      retCode;
    u_int32    gotsize;
    int32      ch;
    u_int32    inBufferSize;
    u_int32    inBufferTimeout;
    DESC_HANDLE *descHdl;
    int         hwAccess;
    int         modIdMagic;
    int         modId;
    u_int32     irqMask;
    volatile u_int16     intFromCh;
    u_int32     mode;
    u_int32     highWater;
    u_int32     dbgLevelDesc;
    u_int32     dbgLevelMbuf;


    hwAccess = M66_HW_ACCESS_NO;

    retCode = DESC_Init( descSpec, osHdl, &descHdl );
    if( retCode )
    {
        return( retCode );
    }/*if*/

    /*-------------------------------------+
    |  LL-HANDLE allocate and initialize   |
    +-------------------------------------*/
    m66Hdl = (M66_HANDLE*) OSS_MemGet( osHdl, sizeof(M66_HANDLE), &gotsize );
    if( m66Hdl == NULL )
    {
       *llHdlP = 0;
       return( ERR_OSS_MEM_ALLOC );
    }/*if*/

    *llHdlP = (LL_HANDLE*) m66Hdl;

    /* fill turkey with 0 */
    OSS_MemFill( osHdl, gotsize, (char*) m66Hdl, 0 );
    m66Hdl->ownMemSize = gotsize;
    m66Hdl->osHdl      = osHdl;
    m66Hdl->maPld      = *ma;
    m66Hdl->irqHdl        = irqHdl;

    DBG_MYLEVEL = OSS_DBG_DEFAULT;
    DBGINIT((NULL,&DBH));

    /*-------------------------------------+
    |   get DEBUG LEVEL                    |
    +-------------------------------------*/
    /* DEBUG_LEVEL_DESC */
    retCode = DESC_GetUInt32( descHdl,
                              OSS_DBG_DEFAULT,
                              &dbgLevelDesc,
                              "DEBUG_LEVEL_DESC",
                              NULL );
    if( retCode != 0 && retCode != ERR_DESC_KEY_NOTFOUND ) goto CLEANUP;
    DESC_DbgLevelSet(descHdl, dbgLevelDesc);
    retCode = 0;

    /* DEBUG_LEVEL_MBUF */
    retCode = DESC_GetUInt32( descHdl,
                              OSS_DBG_DEFAULT,
                              &dbgLevelMbuf,
                              "DEBUG_LEVEL_MBUF",
                              NULL );
    if( retCode != 0 && retCode != ERR_DESC_KEY_NOTFOUND ) goto CLEANUP;
    retCode = 0;

    /* DEBUG_LEVEL - LL */
    retCode = DESC_GetUInt32( descHdl,
                              OSS_DBG_DEFAULT,
                              &m66Hdl->dbgLevel,
                              "DEBUG_LEVEL",
                              NULL );
    if( retCode != 0 && retCode != ERR_DESC_KEY_NOTFOUND ) goto CLEANUP;
    retCode = 0;

    DBGWRT_1((DBH, "%s\n", functionName));

    /*-------------------------------------+
    |  creates read 1 buffer for all ch    |
    +-------------------------------------*/
    retCode = DESC_GetUInt32( descHdl,
                              M66_DEFAULT_BUF_SIZE,
                              &inBufferSize,
                              "RD_BUF/SIZE",
                              0 );
    if( retCode != 0 && retCode != ERR_DESC_KEY_NOTFOUND ) goto CLEANUP;
    retCode = 0;

    retCode = DESC_GetUInt32( descHdl,
                              M_BUF_USRCTRL,
                              &mode,
                              "RD_BUF/MODE",
                              0 );
    if( retCode != 0 && retCode != ERR_DESC_KEY_NOTFOUND ) goto CLEANUP;
    retCode = 0;

    retCode = DESC_GetUInt32( descHdl,
                              M66_DEFAULT_BUF_TIMEOUT,
                              &inBufferTimeout,
                              "RD_BUF/TIMEOUT",
                              0 );
    if( retCode != 0 && retCode != ERR_DESC_KEY_NOTFOUND ) goto CLEANUP;
    retCode = 0;

    retCode = DESC_GetUInt32( descHdl,
                              M66_DEFAULT_BUF_SIZE,
                              &highWater,
                              "RD_BUF/HIGHWATER",
                              0 );
    if( retCode != 0 && retCode != ERR_DESC_KEY_NOTFOUND ) goto CLEANUP;
    retCode = 0;

    retCode = MBUF_Create( osHdl, devSem, m66Hdl, inBufferSize,
                           M66_CH_WIDTH * M66_MAX_CH,
                           mode,
                           MBUF_RD, highWater, inBufferTimeout,
                           m66Hdl->irqHdl,
                           &m66Hdl->inbuf );
    if( retCode ) goto CLEANUP;

    /*-------------------------------------+
    |  descriptor - use module id ?        |
    +-------------------------------------*/
    retCode = DESC_GetUInt32( descHdl,
                              1,
                              &m66Hdl->useModulId,
                              "ID_CHECK",
                              NULL );
    if( retCode != 0 && retCode != ERR_DESC_KEY_NOTFOUND ) goto CLEANUP;
    retCode = 0;

    if( m66Hdl->useModulId)
    {
		/* is the M66 an onboard D302/A302 device? */
		modIdMagic = MREAD_D16( m66Hdl->maPld, D302_MAGICWORD_REG );
        if( modIdMagic == MOD_ID_MAGIC )
        {
			/* D302/A302 detected */
			m66Hdl->d302Device = 1;
			DBGWRT_2((DBH, "D302/A302 detected\n"));
        }/*if*/
		else
		{
			/* get id from M66 M-Module eeprom */
			modIdMagic = m_read((U_INT32_OR_64)m66Hdl->maPld, 0);
			modId      = m_read((U_INT32_OR_64)m66Hdl->maPld, 1);
			if( modIdMagic != MOD_ID_MAGIC )
			{
				 DBGWRT_ERR((DBH, "*** %s: m_read() id - illegal magic word\n",
							  functionName ));
				 retCode = ERR_LL_ILL_ID;
				 goto CLEANUP;
			}/*if*/

			if( modId != MOD_ID )
			{
				 DBGWRT_ERR((DBH, "*** %s:  m_read() id - illegal module id\n",
							  functionName ));
				 retCode = ERR_LL_ILL_ID;
				 goto CLEANUP;
			}/*if*/
		}/*else*/
	}/*if*/

    /*------------------------------+
    |  init id function table       |
    +------------------------------*/
	/* driver's ident function */
	m66Hdl->idFuncTbl.idCall[0].identCall = M66_Ident;
	m66Hdl->idFuncTbl.idCall[1].identCall = M66_FlexDataIdent;
	/* library's ident functions */
	m66Hdl->idFuncTbl.idCall[2].identCall = MBUF_Ident;
	m66Hdl->idFuncTbl.idCall[3].identCall = DESC_Ident;
	m66Hdl->idFuncTbl.idCall[4].identCall = OSS_Ident;
	/* terminator */
	m66Hdl->idFuncTbl.idCall[5].identCall = NULL;

    /*-------------------------+
    | load FLEX logic          |
    +-------------------------*/
    if( !m66Hdl->d302Device )
		m66_flexload( m66Hdl );

    hwAccess = M66_HW_ACCESS_PERMITED;

    /* dummy access to clear interrupt after flexload */
    intFromCh = MREAD_D16( m66Hdl->maPld, INTREG );

    /*--------------------------+
    | config all channels       |
    +--------------------------*/
    for( ch = 0; ch < M66_MAX_CH; ch++ )
    {
        retCode = DESC_GetUInt32( descHdl,
                              M66_DEFAULT_IRQ_DISABLED,
                              &irqMask,
                              "CHANNEL_%d/IRQ_ENABLE",
                              ch );
        if( retCode != 0 && retCode != ERR_DESC_KEY_NOTFOUND ) goto CLEANUP;
        retCode = 0;

        m66Hdl->edgemask[ch] = (u_int8)(irqMask << 1);

        /* clear and disable interrupt, switch output off */
        MWRITE_D16( m66Hdl->maPld, IOREG(ch), 0);
    }/*for*/

    m66Hdl->irqsource = 0;              /* reset irqsource flag */

    DESC_Exit( &descHdl );
    return( retCode );

 CLEANUP:
    DESC_Exit( &descHdl );
    if( hwAccess == M66_HW_ACCESS_PERMITED )
        M66_Exit( llHdlP );
    else
    {
        M66_MemCleanup( m66Hdl );
        *llHdlP = 0;
    }/*if*/

    return( retCode );

}/*M66_Init*/

/****************************** M66_Exit ************************************/
/** Deinitializes the hardware, disables interrupts,
 *                frees allocated memory.
 *
 *         Note:  Is called by MDIS kernel only.
 *
 *---------------------------------------------------------------------------
 *  \param llHdlP		\IN pointer to low level driver handle
 *	\return  0 | error code
 *
 ****************************************************************************/
static int32 M66_Exit
(
    LL_HANDLE **llHdlP
)
{
    DBGCMD( static const char functionName[] = "LL - M66_Exit()"; )
    int         ch;
    M66_HANDLE  *m66Hdl = (M66_HANDLE*) *llHdlP;
    volatile u_int16     intFromCh;
    int32       retCode;

    DBGWRT_1((DBH, "%s\n", functionName));

    /*--------------------------+
    | output off / disable irq  |
    +--------------------------*/
	for (ch=0; ch<M66_MAX_CH; ch++)
		MWRITE_D16(m66Hdl->maPld, IOREG(ch), 0);

    /* dummy access to clear interrupt after flexload */
    intFromCh = MREAD_D16( m66Hdl->maPld, INTREG );


    retCode = M66_MemCleanup( m66Hdl );
    *llHdlP = 0;

    return( retCode );
}/*M66_Exit*/

/****************************** M66_Read ************************************/
/** Reads input state from current channel.
 *
 *
 *---------------------------------------------------------------------------
 *  \param llHdl		\IN pointer to low-level driver data structure
 *  \param ch			\IN current channel 0..31
 *  \param valueP		\IN pointer to variable where read value stored
 *  \param *valueP		\OUT read value 0=low, 1=high
 *  \return   0 | error code
 *
 ****************************************************************************/
static int32 M66_Read
(
    LL_HANDLE   *llHdl,
    int32       ch,
    int32       *valueP
)
{
    DBGCMD( static const char functionName[] = "LL - M66_Read()"; )
    M66_HANDLE*       m66Hdl = (M66_HANDLE*) llHdl;
    u_int8            rdVal;
    DBGWRT_1((DBH, "%s from ch=%d\n", functionName, ch));

    rdVal  = (u_int8) MREAD_D16( m66Hdl->maPld, IOREG(ch) );
    *valueP = ( rdVal & D_INVAL ? 1 : 0);        /* read value */
    DBGWRT_3((DBH, "%04x -> %d\n", rdVal, *valueP ));

    return(0);
}

/****************************** M66_Write ***********************************/
/** The output of the current channel will be switched on or off.
 *
 *
 *---------------------------------------------------------------------------
 *  \param llHdl		\IN pointer to low-level driver data structure
 *  \param ch			\IN current channel 0..31
 *  \param value		\IN switch on/off   0=off, 1=on
 *	\return  0 | error code
 *
 ****************************************************************************/
static int32 M66_Write
(
    LL_HANDLE *llHdl,
    int32  ch,
    int32  value
)
{
    DBGCMD( static const char functionName[] = "LL - M66_Write"; )
    M66_HANDLE*       m66Hdl = (M66_HANDLE*) llHdl;
    DBGWRT_1((DBH, "%s to ch=%d val %08x\n", functionName, ch, value));

    if (value)
    {
        MSETMASK_D16(m66Hdl->maPld, IOREG(ch), D_OUTVAL );
    }
    else
    {
        MCLRMASK_D16(m66Hdl->maPld, IOREG(ch), D_OUTVAL );
	}

    return(0);
}

/****************************** M66_SetStat *********************************/
/** Changes the device state.
 *
 *   \code common codes               values          meaning			
 *     M_MK_IRQ_ENABLE           0               disable module interrupt
 *                               1               enable  module interrupt
 *                                               by writing edge channel mask
 *
 *     M_LL_DEBUG_LEVEL          see oss.h       en/disable debug output
 *                                               at task or interrupt level
 *
 *     M_LL_IRQ_COUNT            irq count       set the module interrupt counter
 *
 *
 *   specific codes             values          meaning
 *     M66_EDGE_MASK             0               no edge
 *                               1 M66_IELH      rising edge
 *                               2 M66_IEHL      falling edge
 *                                               set the edge mask of the current
 *                                               channel
 *
 *     M66_SIG_EDGE_OCCURRED     signal number   installs the signal sent by ISR
 *
 *     M66_SIG_CLR_EDGE_OCCURRED signal number   deinstalls the signal
 * \endcode
 *---------------------------------------------------------------------------
 *  \param llHdl			\IN pointer to low-level driver data structure
 *  \param code				\IN setstat code
 *  \param ch				\IN current channel (ignored for some codes)
 *  \param value32_or_64	\IN setstat value or pointer to blocksetstat data
 *  \return 0 | error code
 *
 *
 ****************************************************************************/
static int32 M66_SetStat
(
    LL_HANDLE *llHdl,
    int32  code,
    int32  ch,
    INT32_OR_64  value32_or_64
)
{
    DBGCMD( static const char functionName[] = "LL - M66_SetStat()"; )
    M66_HANDLE      *m66Hdl = (M66_HANDLE*) llHdl;
    int32           retCode;
    volatile u_int8 intFromCh;
    u_int8          cfgVal;
	int32		    value = (int32)value32_or_64;	/* store 32bit value */

    DBGWRT_1((DBH, "%s code=$%04lx data=%ld\n", functionName, code, value));

    switch(code)
    {
        /* ------ common setstat codes --------- */
        /*--------------------+
        |  irq enable         |
        +--------------------*/
        case M_MK_IRQ_ENABLE:
          
			/* --- disable command? --- */	
			if( value==0){

				/* for each channel */
				for( ch=0; ch<M66_MAX_CH; ch++ ){
					/* clear edge enable flags */
					MCLRMASK_D16( m66Hdl->maPld, IOREG(ch), D_IEMSK );
					/* save disable state for ch */
					m66Hdl->irqEnabled[ch] = 0;
				}
			}
			/* --- enable command! --- */	
			else {
     
				/* for each channel */
				for( ch=0; ch<M66_MAX_CH; ch++ ){
				  
					/* channel is enabled? */
					if( m66Hdl->irqEnabled[ch] ){
						/* reconfig enable flags */
						MSETMASK_D16(m66Hdl->maPld, IOREG(ch), m66Hdl->edgemask[ch] );
					}
					/* channel is disabled! */
					else{
						  /* sw - workaround
							 hw generate interrupt - if a edge was occurred
							 read the channel address to clear interrupt
							 and clear the edge occurred flags
							 before generating interrupt */

						  /* clear interrupt */
						  intFromCh = (u_int8) MREAD_D16( m66Hdl->maPld, INTREG );

						  /* don't change the io flag, clear the occurred flags */
						  cfgVal = (u_int8) MREAD_D16( m66Hdl->maPld, IOREG(ch) );
						  cfgVal |= m66Hdl->edgemask[ch];	/* set edge enable flags */
						  cfgVal &= ~D_EDMSK;				/* clear edge status flags */
						  MWRITE_D16( m66Hdl->maPld, IOREG(ch), cfgVal );
					}/*if*/

					/* any edge enable flag set? */
					if( m66Hdl->edgemask[ch] )
						/* save enable state for ch */
						m66Hdl->irqEnabled[ch] = 1;
				}/*for*/
			}/*if*/
          break;

        /*------------------+
        |  ch direction     |
        +------------------*/
        case M_LL_CH_DIR:
            if( value != M_CH_INOUT )
            {
                DBGWRT_ERR((DBH, "*** %s: illegal channel mode\n", functionName));
                return( ERR_LL_ILL_PARAM );
            }/*if*/
            break;

        /*------------------+
        |  irq count        |
        +------------------*/
        case M_LL_IRQ_COUNT:
            m66Hdl->irqCount = value;
            break;

        /*------------------+
        |  debug level      |
        +------------------*/
        case M_LL_DEBUG_LEVEL:
            m66Hdl->dbgLevel = value;
            break;



        /* ------ special setstat codes --------- */
        /*--------------------+
        |  edge mask of ch    |
        +--------------------*/
        case M66_EDGE_MASK:
            m66Hdl->edgemask[ch] = (u_int8) ( D_IEMSK & ( value << 1 ));

            /* reconfigure if irq(s) enabled */
            if( m66Hdl->irqEnabled[ch] )
            {
               MCLRMASK_D16( m66Hdl->maPld,IOREG(ch), D_IEMSK );
               MSETMASK_D16( m66Hdl->maPld,IOREG(ch), m66Hdl->edgemask[ch] );
            }
            break;

        /*--------------------+
        |  signal conditions  |
        +--------------------*/
        case M66_SIG_EDGE_OCCURRED:

           if( m66Hdl->sigHdl != NULL )   /* already defined ? */
           {
               DBGWRT_ERR((DBH, "*** %s: signal already defined\n", functionName));
               return(ERR_OSS_SIG_SET);        /* can't set ! */
           }/*if*/
           
           retCode = OSS_SigCreate( m66Hdl->osHdl, value, &m66Hdl->sigHdl );
           return( retCode );
           /* break; not reached */

        case M66_SIG_CLR_EDGE_OCCURRED:
           retCode = (OSS_SigRemove( m66Hdl->osHdl, &m66Hdl->sigHdl ) );
           return( retCode );
           /* break; not reached */

        /*--------------------+
        |  (unknown)          |
        +--------------------*/
        default:
            if(    ( M_RDBUF_OF <= code && code <= (M_WRBUF_OF+0x0f) )
                || ( M_RDBUF_BLK_OF <= code && code <= (M_RDBUF_BLK_OF+0x0f) )
              )
                return( MBUF_SetStat( m66Hdl->inbuf,
                                      NULL,
                                      code,
                                      value ) );

            DBGWRT_ERR((DBH, "*** %s: unkown setstat code\n", functionName));
            return(ERR_LL_UNK_CODE);
    }/*switch*/

    return(0);
}

/****************************** M66_GetStat *********************************/
/** Gets the device state.
 *
 *  \code common codes               values          meaning
 *     M_LL_CH_NUMBER            32              number of channels
 *
 *     M_LL_CH_DIR               M_CH_INOUT      direction of curr ch
 *                                               always in/out
 *
 *     M_LL_CH_LEN               1               length in bit
 *
 *     M_LL_CH_TYP               M_CH_BINARY     binary
 *
 *     M_LL_IRQ_COUNT            0..x            module irq count
 *
 *     M_LL_ID_CHECK             0..1            SPROM ID is checked
 *                                               in M66_Init()
 *
 *     M_LL_DEBUG_LEVEL          see oss.h       current debug level
 *
 *     M_LL_ID_SIZE              128             eeprom size [bytes] (*1*)
 *
 *     M_LL_BLK_ID_DATA		     -               eeprom raw data (*2*)
 *
 *     M_MK_BLK_REV_ID           pointer to the ident function table
 *
 *
 *   specific codes             values          meaning
 *     M66_EDGE_MASK             0               no edge
 *                               1 M66_IELH      rising edge
 *                               2 M66_IEHL      falling edge
 *                                               get the edge mask of the current
 *                                               channel
 *
 *     M66_SIG_EDGE_OCCURRED     signal number   get the signal number
 *                                               0 if not installed
 *
 *     M66_IRQ_SOURCE            irq source      irq triggering channel
 *                                               of the last irq
 *
 *   Note for D302/A302 devices:
 *     (*1*) M_LL_ID_SIZE returns 0
 *     (*2*) M_LL_BLK_ID_DATA not supported
 * \endcode
 *---------------------------------------------------------------------------
 *  \param llHdl		\IN pointer to low-level driver data structure
 *  \param code			\IN getstat code
 *  \param ch			\IN current channel (ignored for some codes)
 *  \param value32_or_64P	\IN pointer to variable where value stored or
 *                         pointer to blocksetstat data
 *  \param *value32_or_64P	\OUT getstat value
 *  \return   0 | error code
 *
 ****************************************************************************/
static int32 M66_GetStat
(
    LL_HANDLE *llHdl,
    int32  code,
    int32  ch,
    INT32_OR_64  *value32_or_64P
)
{
	DBGCMD( static const char functionName[] = "LL - M66_GetStat()"; )
    M66_HANDLE  *m66Hdl = (M66_HANDLE*) llHdl;
    int32       processId;
	int32		*valueP = (int32*)value32_or_64P;   /* pointer to 32bit value      */
    INT32_OR_64 *value64P = value32_or_64P;          /*stores 32/64 bit pointer */

    DBGWRT_1((DBH, "%s code=$%04lx\n", functionName, code));

    switch(code)
    {
        /* ------ common getstat codes --------- */
        /*------------------+
        |  get ch count     |
        +------------------*/
        case M_LL_CH_NUMBER:
            *valueP = M66_MAX_CH;
            break;

        /*------------------+
        |  ch direction     |
        +------------------*/
        case M_LL_CH_DIR:
             *valueP = M_CH_INOUT;
            break;

        /*--------------------+
        |  ch length (bit)    |
        +--------------------*/
        case M_LL_CH_LEN:
             *valueP = 1;
             break;

        /*------------------+
        |  ch typ           |
        +------------------*/
        case M_LL_CH_TYP:
             *valueP = M_CH_BINARY;
             break;

        /*------------------+
        |  irq count        |
        +------------------*/
        case M_LL_IRQ_COUNT:
            *valueP = m66Hdl->irqCount;
            break;

        /*------------------+
        |  id check enabled |
        +------------------*/
        case M_LL_ID_CHECK:
            *valueP = m66Hdl->useModulId;
            break;

        /*------------------+
        |  debug level       |
        +------------------*/
        case M_LL_DEBUG_LEVEL:
            *valueP = m66Hdl->dbgLevel;
            break;

        /*--------------------+
        |  id prom size       |
        +--------------------*/
        case M_LL_ID_SIZE:
			/* D302/A302 device */
			if( m66Hdl->d302Device )
				*valueP = 0;
			else
				*valueP = MOD_ID_SIZE;
            break;

        /*--------------------+
        |  ident table        |
        +--------------------*/
        case M_MK_BLK_REV_ID:
           *value64P = (INT32_OR_64)&m66Hdl->idFuncTbl;
           break;

        /* ------ special getstat codes --------- */
        /*------------------+
        |  get irq source   |
        +------------------*/
        case M66_IRQ_SOURCE:
            *valueP = m66Hdl->irqsource;
            break;

        /*--------------------+
        |  edge mask of ch    |
        +--------------------*/
        case M66_EDGE_MASK:
            *valueP = (m66Hdl->edgemask[ch] & D_IEMSK) >> 1;
            break;

        /*--------------------+
        |  signal conditions  |
        +--------------------*/
        case M66_SIG_EDGE_OCCURRED:
           OSS_SigInfo( m66Hdl->osHdl, m66Hdl->sigHdl, (int32*)valueP, &processId );
           break;

        /*--------------------+
        |  (unknown)          |
        +--------------------*/
        default:
            if(    ( M_LL_BLK_OF <= code && code <= (M_LL_BLK_OF+0xff) )
                || ( M_DEV_BLK_OF <= code && code <= (M_DEV_BLK_OF+0xff) )
              )
                return( getStatBlock( m66Hdl, code, (M_SETGETSTAT_BLOCK*) value64P ) );


            if(    ( M_RDBUF_OF <= code && code <= (M_WRBUF_OF+0x0f) )
                || ( M_RDBUF_BLK_OF <= code && code <= (M_RDBUF_BLK_OF+0x0f) )
              )
                return( MBUF_GetStat( m66Hdl->inbuf,
                                      NULL,
                                      code,
                                     (int32*)valueP ) );

            DBGWRT_ERR((DBH, "*** %s: unkown getstat code\n", functionName));
            return(ERR_LL_UNK_CODE);

    }/*switch*/

    return(0);
}

/******************************* M66_BlockRead ******************************/
/** Reads all channels to buffer. Stores input state and occurred
 *                edges. Always starts with channel 0.
 *
 *   \code        Supported modes:  M_BUF_USRCTRL         reads from hardware
 *                                  M_BUF_RINGBUF         reads from buffer
 *                                  M_BUF_RINGBUF_OVERWR   "     "     "
 *                                  M_BUF_CURRBUF          "     "     "
 *
 *                buffer structure
 *
 *                byte     0    1..30    31    32
 *                       +---+- - - - -+----++----+---
 *             channel   | 0 |  1..30  | 31 ||  0 | .....
 *                       +---+- - - - -+----++----+---
 *
 *                byte structure
 *
 *                bit      7..3     2        1         0
 *                       +- - -+---------+---------+-------+
 *            meaning    |  no |  H->L   |  L->H   | value |
 *                       +- - -+---------+---------+-------+
 *  \endcode
 *---------------------------------------------------------------------------
 *  \param llHdl		\IN pointer to low-level driver data structure
 *  \param ch			\IN current channel (always ignored)
 *  \param buf          \IN buffer to store read values
 *  \param size         \IN must be multiple of buffer width (32)
 *  \param nbrRdBytesP  \IN pointer to variable where number of read bytes
 *                             is stored
 *  \param *nbrRdBytesP \OUT number of read bytes
 *  \return       0 | error code
 *
 ****************************************************************************/
static int32 M66_BlockRead
(
    LL_HANDLE  *llHdl,
    int32     ch,
    void      *buf,
    int32     size,
    int32     *nbrRdBytesP
)
{
    DBGCMD( static const char functionName[] = "LL - M66_BlockRead()"; )
    M66_HANDLE *m66Hdl = (M66_HANDLE*) llHdl;
    u_int8     *bufP = (u_int8*)buf;           /* ptr to buffer */
    u_int32    nbrToRead;
    u_int32    nbrRead;
    u_int32    loopy;
    int32      fktRetCode;
    int32      bufMode;

    DBGWRT_1((DBH, "%s\n", functionName));

    *nbrRdBytesP = 0;

    nbrToRead = size / M66_MAX_CH;

    fktRetCode = MBUF_GetBufferMode( m66Hdl->inbuf , &bufMode );
    if( fktRetCode != 0 && fktRetCode != ERR_MBUF_NO_BUF )
    {
        DBGWRT_ERR((DBH, "*** %s: MBUF_GetBufferMode()\n", functionName));
        return( fktRetCode );
    }/*if*/

    switch( bufMode )
    {
   	    int32      dummy;

        case M_BUF_USRCTRL:
           for( loopy = 0; loopy < nbrToRead; loopy++ )
           {
               nbrRead = M66_HwBlockRead( m66Hdl, bufP, M66_NO_IRQ, &dummy );
               bufP += nbrRead;
               *nbrRdBytesP += nbrRead;
           }/*for*/
           fktRetCode   = 0; /* ovrwr ERR_MBUF_NO_BUFFER */
           break;

        case M_BUF_RINGBUF:
        case M_BUF_RINGBUF_OVERWR:
        case M_BUF_CURRBUF:
        default:
           fktRetCode = MBUF_Read( m66Hdl->inbuf, (u_int8*) buf, size,
                                   nbrRdBytesP );

    }/*switch*/

    return( fktRetCode );
}

/****************************** M66_HwBlockRead *****************************/
/** Reads all 32 channels.
 *                Stores the input states and the occurred edges in the
 *                buffer.
 *                Clears the edge flag registers.
 *
 *---------------------------------------------------------------------------
 *  \param m66Hdl			\IN pointer to low-level driver data structure
 *  \param buf				\IN buffer to store read values
 *  \param calledFromIrq	\IN M66_NO_IRQ or M66_IRQ if called from IRQ
 *  \param edgeOccuredP		\IN ptr to buffer for edge occured
 *  \param *edgeOccuredP	\OUT shows if edge occurred since last block was read
 *  \return        nbr of read bytes ( 32 )
 *
 ****************************************************************************/
static int32 M66_HwBlockRead	
(
    M66_HANDLE *m66Hdl,
    u_int8     *buf,
    int		   calledFromIrq,
    int32      *edgeOccuredP
)
{
    DBGCMD( static const char functionName[] = "LL - M66_HwBlockRead()"; )
    int32			ch;
    u_int8			rdVal1, rdVal2;
    OSS_IRQ_STATE	oldState = (OSS_IRQ_STATE) 0;

    DBGWRT_1((DBH, "%s\n", functionName));

	*edgeOccuredP = 0;

	/* called from M66_Irq()? */
	if( calledFromIrq == M66_NO_IRQ )
	{
		/* called from M66_BlockRead() --> mask IRQs */
		oldState = OSS_IrqMaskR( m66Hdl->osHdl, m66Hdl->irqHdl );
    }
    
    /*----------------------------------+
    | read all channel values+flags     |
    +----------------------------------*/
    for( ch=0; ch<M66_MAX_CH; ch++){

		/* (1) read from data i/o register (to get edge-occured-flags) */
        rdVal1   = (u_int8) MREAD_D16( m66Hdl->maPld, IOREG(ch) );

		/* any edge-occured-flag set? */
		if( rdVal1 & D_EDMSK ){

			/* (2) write to data i/o register (to reset edge-occured-flags) */
			MWRITE_D16(m66Hdl->maPld, IOREG(ch), rdVal1 &~ (rdVal1 & D_EDMSK));

			/* only one edge-occured-flag set? */
			if( (rdVal1 & D_EDMSK) != D_EDMSK ){

				/* 
				 * Consideration for the case that a second edge occures
				 * during one loop iteration:
				 *
				 * Only if the input signal is stable for a period of 40us
				 * the I/O controller will update the IN bit in the
				 * Data Input/Output Register.
				 * It is possible that the IN bit change and therefore
				 * an additional edge occures during one loop iteration
				 * (between the read and subsequent write to the IOREG).
				 * But a third edge shouldn't never happen during one loop
				 * iteration.
				 *
				 * If a new edge occures between read (1) and write (2)
				 * then write (2) will reset the new edge-occured-flag
				 * on error. The new occured edge would be lost.
				 *
				 * To fix this error, we perform the following steps:
				 * - (3) read again from data i/o register
				 * - Determine if a new edge has been occured between
				 *   read (1) and read (3): IN at read (1) != IN at read (3)
				 *   a) If any edge-occured-flag is set at read (3), then the
				 *      new edge has been occured after write (2) and we have to
				        reset the second edge-accured-flag as well (4).
				 *   b) If no edge-occured-flag is set at read (3), then the
				 *      new edge has been occured between read (1) and write (2).
				 *   In both cases, we assume that the new occured edge is of
				 *   the other edge type as detected at read (1). Therefore we
				 *   notify that a rising and a falling edge occured and
				 *   return the last read value from read (3).
				 */

				/* (3) read from data i/o register */
				rdVal2 = (u_int8) MREAD_D16( m66Hdl->maPld, IOREG(ch) );

				/* new edge occured? */
				if( ((rdVal1 & D_INVAL) != (rdVal2 & D_INVAL)) ){

					/* edge-occured-flag set? */
					if(	rdVal2 & D_EDMSK ){
						DBGWRT_2((DBH, " second edge - occured after write (2)\n"));
						/* (4) reset second edge-occured-flag */
						MWRITE_D16(m66Hdl->maPld, IOREG(ch), rdVal2 &~ (rdVal2 & D_EDMSK));
					}
					else {
						DBGWRT_2((DBH, " second edge - occured between read (1) and write (2)\n"));
					}

					/* using read (2) state and notify both edges */
					rdVal1 = rdVal2 | D_EDMSK;
				}
			}/* only one edge-occured-flag set? */
		}/* any edge-occured-flag set? */

        *buf++ = (u_int8)(( rdVal1 & 0x38) >> 3);
        *edgeOccuredP |= rdVal1 & D_EDMSK;

	}/*for*/

	if( calledFromIrq == M66_NO_IRQ )
	{
		/* reenable */
		OSS_IrqRestore( m66Hdl->osHdl, m66Hdl->irqHdl, oldState );
	}

    return(M66_MAX_CH);     /* 32 bytes read */
}

/****************************** M66_BlockWrite ******************************/
/** Switch maximum 32 channels. Always starts with channel 0.
 *
 *  \code    Supported modes:  M_BUF_USRCTRL         writes to hardware
 *
 *
 *                buffer structure
 *
 *                byte     0    1..30    31
 *                       +---+- - - - -+----+
 *             channel   | 0 |  1..30  | 31 |
 *                       +---+- - - - -+----+
 *
 *                byte structure
 *
 *                bit      7..1    0
 *                       +- - -+-------+
 *            meaning    |  no | value |  0=off, 1=on
 *                       +- - -+-------+
 *  \endcode
 *---------------------------------------------------------------------------
 *  \param llHdl		\IN pointer to low-level driver data structure
 *  \param ch			\IN current channel (always ignored)
 *  \param buf			\IN buffer where output values are stored
 *  \param size			\IN number of bytes to write (max. 32)
 *  \param nbrWrBytesP  \OUT number of written bytes
 *  \return       0 | error code
 *
 ****************************************************************************/
static int32 M66_BlockWrite
(
    LL_HANDLE *llHdl,
    int32     ch,
    void      *buf,
    int32     size,
    int32     *nbrWrBytesP
)
{
    DBGCMD( static const char functionName[] = "LL - M66_BlockWrite()"; )
    M66_HANDLE*       m66Hdl = (M66_HANDLE*) llHdl;
    int8 *bufptr = (int8*) buf;           /* ptr to buffer */
    u_int8     rdVal;

    DBGWRT_1((DBH, "%s size=%ld\n", functionName, size));

    *nbrWrBytesP = 0;

    if( size > M66_MAX_CH )
        size = M66_MAX_CH;

    /*----------------------------------+
    | write all channel values          |
    +----------------------------------*/
    for( ch=0; ch<size; ch++ )
    {
        rdVal = (u_int8) MREAD_D16( m66Hdl->maPld, IOREG(ch) );

        if( *bufptr++ )
            rdVal |= D_OUTVAL;
        else
            rdVal &= ~D_OUTVAL;

        MWRITE_D16( m66Hdl->maPld, IOREG(ch), rdVal );
    }

    *nbrWrBytesP = ch;

    return( 0 );
}

/****************************** M66_Irq ************************************/
/** The irq routine reads all channels.
 *                It stores the input states and the occurred edges to the
 *                read buffer if available.
 *
 *                It clears the edge flag registers.
 *                If signal is installed (setstat code M66_SIG_EDGE_OCCURRED)
 *                this is sent.
 *
 *                It clears the module irq and stores the number of the channel that
 *                triggered the irq. (getstat code M66_IRQ_SOURCE)
 *
 *                It intcrements the module irq counter if an edge was
 *                detected. (getstat code M_LL_IRQ_COUNT)
 *
 *---------------------------------------------------------------------------
 *  \param llHdl		\IN pointer to low-level driver data structure
 *  \return MDIS_IRQ_DEV_NOT | MDIS_IRQ_DEVICE
 *
 ****************************************************************************/
static int32 M66_Irq
(
    LL_HANDLE *llHdl
)
{
    M66_HANDLE *m66Hdl = (M66_HANDLE*) llHdl;
    u_int8     *buf, ch, intreg;
    u_int8     dummy[M66_MAX_CH];
    int32      gotsize;
    int32      edgeOccured;

    IDBGWRT_1((DBH, ">> M66_Irq:\n"));

    /*------------------------------+
    |  input values (all channels)  |
    +------------------------------*/
	M66_HwBlockRead( m66Hdl, dummy, M66_IRQ, &edgeOccured );

	/* not our interrupt? */
	if( !edgeOccured )
        return( LL_IRQ_DEV_NOT );
    
	if( (buf = (u_int8*) MBUF_GetNextBuf( m66Hdl->inbuf, 1, &gotsize)) != 0 ){
		OSS_MemCopy( m66Hdl->osHdl, M66_MAX_CH, (char*)dummy, (char*)buf );
        MBUF_ReadyBuf( m66Hdl->inbuf );	/* blockread ready */
    }
    else{
	    IDBGWRT_ERR((DBH, ">> *** WARNING: M66_Irq: (dummy read)\n"));
	}

    /*----------------------+
    | handle signal cond.   |
    +----------------------*/
    /* send signal on cond. 1 */
    if( m66Hdl->sigHdl != NULL )
        if( OSS_SigSend( m66Hdl->osHdl, m66Hdl->sigHdl ) )
            IDBGWRT_ERR( ( DBH, ">> *** M66_Irq: OSS_SigSend failed\n"));

    /*----------------------+
    | check/reset irq       |
    +----------------------*/
    intreg = (u_int8) MREAD_D16( m66Hdl->maPld, INTREG );
    ch = (u_int8)((intreg & I_IRQCH) >> 1);
    IDBGWRT_2(( DBH,">> M66_Irq: ch=%d\n", ch ));

    /*----------------------+
    | re-enable edge irqs   |
    +----------------------*/
    MSETMASK_D16( m66Hdl->maPld, IOREG(ch), m66Hdl->edgemask[ch] );


    m66Hdl->irqsource = ch;              /* stores the irq source */
    m66Hdl->irqCount++;
    return( LL_IRQ_DEVICE );

}/*M66_Irq*/

/****************************** M66_Info ***********************************/
/** Get information about hardware and driver requirements.
 *
 *                The following info codes are supported:\n
 *
 *  \code              Code                      Description
 *                ------------------------  -----------------------------
 *                LL_INFO_HW_CHARACTER      hardware characteristics
 *                LL_INFO_ADDRSPACE_COUNT   nr of required address spaces
 *                LL_INFO_ADDRSPACE         address space information
 *                LL_INFO_IRQ               interrupt required
 *                LL_INFO_LOCKMODE          process lock mode required
 *  \endcode
 *                The LL_INFO_HW_CHARACTER code returns all address and
 *                data modes (ORed) which are supported by the hardware
 *                (MDIS_MAxx, MDIS_MDxx).\n
 *
 *                The LL_INFO_ADDRSPACE_COUNT code returns the number
 *                of address spaces used by the driver.\n
 *
 *                The LL_INFO_ADDRSPACE code returns information about one
 *                specific address space (MDIS_MAxx, MDIS_MDxx). The returned 
 *                data mode represents the widest hardware access used by 
 *                the driver.\n
 *
 *                The LL_INFO_IRQ code returns whether the driver supports an
 *                interrupt routine (TRUE or FALSE).\n
 *
 *                The LL_INFO_LOCKMODE code returns which process locking
 *                mode the driver needs (LL_LOCK_xxx).\n
 * 
 *---------------------------------------------------------------------------
 *  \param infoType		\IN info code
 *  \param ...			\IN argument(s)
 *  \return       success (0) or error code
 *
 ****************************************************************************/
static int32 M66_Info
(
   int32  infoType,
   ...
)
{
    int32   error;
    va_list argptr;
    u_int32 *nbrOfAddrSpaceP;
    u_int32 *addrModeP;
    u_int32 *dataModeP;
    u_int32 *addrSizeP;
    u_int32 *useIrqP;
    u_int32 addrSpaceIndex;

    error = 0;
    va_start( argptr, infoType );

    switch( infoType )
    {
        case LL_INFO_HW_CHARACTER:
          addrModeP  = va_arg( argptr, u_int32* );
          dataModeP  = va_arg( argptr, u_int32* );
          *addrModeP = MDIS_MA08;
          *dataModeP = MDIS_MD08;
          break;

        case LL_INFO_ADDRSPACE_COUNT:
          nbrOfAddrSpaceP  = va_arg( argptr, u_int32* );
          *nbrOfAddrSpaceP = 1;
          break;

        case LL_INFO_ADDRSPACE:
          addrSpaceIndex = va_arg( argptr, u_int32 );
          addrModeP  = va_arg( argptr, u_int32* );
          dataModeP  = va_arg( argptr, u_int32* );
          addrSizeP  = va_arg( argptr, u_int32* );

          switch( addrSpaceIndex )
          {
              case 0:
                *addrModeP = MDIS_MA08;
                *dataModeP = MDIS_MD16;
                *addrSizeP = 0x100;
                break;

              default:
                 error = ERR_LL_ILL_PARAM;
          }/*switch*/
          break;

        case LL_INFO_IRQ:
          useIrqP  = va_arg( argptr, u_int32* );
          *useIrqP = 1;
          break;

       	case LL_INFO_LOCKMODE:
		{
			u_int32 *lockModeP = va_arg(argptr, u_int32*);
			*lockModeP = LL_LOCK_CALL;
			break;
	    }
	    
        default:
          error = ERR_LL_ILL_PARAM;
    }/*switch*/

    va_end( argptr );
    return( error );
}/*M66_Info*/


/************************** getStatBlock ************************************/
/** Decodes the M_SETGETSTAT_BLOCK code and executes them.
 *
 *  \code  supported codes      values        meaning\n
 *
 *    M_LL_BLK_ID_DATA                   read the M-Module ID from SROM\n
 *      blockStruct->size  0..0xff       number of bytes to read\n
 *      blockStruct->data  pointer       user buffer where ID data stored\n
 * \encode
 *
 *---------------------------------------------------------------------------
 *  \param m66Hdl		\IN m66 handle
 *  \param code         \IN getstat code
 *  \param blockStruct  \IN the struct with code size and data buffer
 *  \param blockStruct	\OUT  filled data buffer in blockStruct->data
 *  \return             0 | error code
 *
 ****************************************************************************/
static int32 getStatBlock	
(
    M66_HANDLE         *m66Hdl,
    int32              code,
    M_SETGETSTAT_BLOCK *blockStruct
)
{
    DBGCMD( static const char functionName[] = "LL - getStatBlock"; )
    int32   error;
    u_int8  i;
    u_int32 maxWords;
    u_int16 *dataP;

    DBGWRT_1((DBH, "%s\n", functionName) );

    error = 0;
    switch( code )
    {
        case M_LL_BLK_ID_DATA:
		  /* D302/A302 device */
		  if( m66Hdl->d302Device )
		  {
				DBGWRT_2((DBH, "M_LL_BLK_ID_DATA not supported for D302/A302 device\n" ));
		  }
		  /* M66 M-Module */
		  else{
			  if (blockStruct->size < MOD_ID_SIZE)		/* check buf size */
				  return(ERR_LL_USERBUF);

			   maxWords = blockStruct->size / 2;
			   dataP = (u_int16*)(blockStruct->data);
			   for( i=0; i<maxWords; i++ )
			   {
				   *dataP = (u_int16)m_read((U_INT32_OR_64)m66Hdl->maPld, i);
				   dataP++;
			   }/*for*/
		  }
		  break;

        default:
           DBGWRT_ERR((DBH, "*** %s: unkown blockgetstat code\n", functionName));
           error = ERR_LL_UNK_CODE;
    }/*switch*/

    return( error );
}/*getStatBlock*/









