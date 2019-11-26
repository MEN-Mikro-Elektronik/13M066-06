/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  m66_doc.c
 *
 *      \author  ck
 *
 *      \brief   User documentation for M66 module driver
 *
 *     Required: -
 *
 *     \switches -
 */
 /*
 *---------------------------------------------------------------------------
 * Copyright 2007-2019, MEN Mikro Elektronik GmbH
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

/*! \mainpage
    This is the documentation of the MDIS low-level driver for the M66 module.

    The M-Module M66 is an I/0 card with 32 binary inputs/outputs. Each channel
    can be used as input or output independent of the other channels.

    \n
    \section Variants Variants
    The M66 is an A08/D08 M-Module. It can be operated on all M-Module
    carrier boards. In order to support different CPU/carrier board
    combinations and to achieve maximum performance, the M66 driver can be
    built in some variants at compilation time:

    \code
    Driver              Variant Description
    --------            --------------------------------
    Standard            A08 address mode, non-swapped
    _sw 				A08 address mode, swapped
    \endcode

    Here are some combinations of MEN CPU and carrier boards together with the
    required variants:

    \code
    CPU                    Carrier Board          Driver Variant
    ----------------       -------------          --------------
    MEN A15	(MPC VME)	   A203N (VME64)		   Standard
    MEN A15 (MPC VME)      A015B                   _sw
    MEN A15 (MPC VME)      A201  (VME64)           Standard
	MEN F14 (Pentium CPCI) F205 (CPCI Carrier)	   Standard
	\endcode

    \n \section FuncDesc Functional Description

    \n \subsection reading Reading and Writing Data
	The driver supports setting and clearing of each channel I/O by writing or reading
	at the current channel number. The channel number can be set with M66_SetStat or
	checked with M66_GetStat codes.

    \n \section interrupts Interrupts
    The driver supports interrupts from the M-Module configurable at each channel.

    \n \section signals Signals
    The driver supports 32 binary signals.
    \n

    \n \section id_prom ID PROM
    The M-Module's ID PROM can be checked for validity before the device is
    initialized. You can set the ID_CHECK option in the device descriptor.


    \n \section api_functions Supported API Functions

    <table border="0">
    <tr>
        <td><b>API function</b></td>
        <td><b>Functionality</b></td>
        <td><b>Corresponding low level function</b></td></tr>

    <tr><td>M_open()</td><td>Open device</td><td>M66_Init()</td></tr>

    <tr><td>M_close()     </td><td>Close device             </td>
    <td>M66_Exit())</td></tr>
    <tr><td>M_read()      </td><td>Read from device         </td>
    <td>M66_Read()</td></tr>
    <tr><td>M_write()     </td><td>Write to device          </td>
    <td>M66_Write()</td></tr>
    <tr><td>M_setstat()   </td><td>Set device parameter     </td>
    <td>M66_SetStat()</td></tr>
    <tr><td>M_getstat()   </td><td>Get device parameter     </td>
    <td>M66_GetStat()</td></tr>
    <tr><td>M_getblock()  </td><td>Block read from device   </td>
    <td>M66_BlockRead()</td></tr>
    <tr><td>M_setblock()  </td><td>Block write from device  </td>
    <td>M66_BlockWrite()</td></tr>
    <tr><td>M_errstringTs() </td><td>Generate error message </td>
    <td>-</td></tr>
    </table>

    \n \section descriptor_entries Descriptor Entries

    The low-level driver initialization routine decodes the following entries
    ("keys") in addition to the general descriptor keys:

    <table border="0">
    <tr><td><b>Descriptor entry</b></td>
        <td><b>Description</b></td>
        <td><b>Values</b></td>
    </tr>
    <tr><td>ID_CHECK</td>
        <td>ID_CHECK = U_INT32 0</td>
    	<td>0..1, default: 1</td>
    </tr>
    <tr>
    	<td>RD_BUF</td>
    </tr>
    <tr>
    	<td>RD_BUF/MODE</td>
        <td>MODE = U_INT32 0</td>
        <td>0..3, default: 0</td>
    </tr>
    <tr>
    	<td>RD_BUF/SIZE</td>
        <td>SIZE = U_INT32 512</td>
        <td>0..n*32, default: 512</td>
    </tr>
    <tr>
    	<td>RD_BUF/TIMEOUT</td>
        <td>TIMEOUT = U_INT32 1000</td>
        <td>1..n in milli sec., default: 1000</td>
    </tr>
    <tr>
    	<td>RD_BUF/HIGHWATER</td>
        <td>HIGHWATER = U_INT32 512</td>
        <td>0..RDBUF/SIZE in byte, default: 512</td>
    </tr>
    <tr>
    	<td>CHANNEL_%d</td>
        <td>CHANNEL_%d </td>
        <td>0..31</td>
    </tr>
    <tr>
    	<td>CHANNEL_%d/IRQ_ENABLE</td>
        <td>IRQ_ENABLE = U_INT32 0</td>
        <td>0..3, default: 0</td>
    </tr>


    </table>

    \n \section programs Overview of provided programs

    \subsection m66_simp  Simple example for using the driver
    m66_simp.c (see example section)

    \subsection m66_demo  Simple example for using the driver
    m66_demo.c (see example section)
    Demo application for the M66 M-Module with demo adapter
    =======================================================
    This simple application:
    - open the M66 device
    - do 100x
    	- set channel 0..6 to output one number (0..9) on the
    	   7-segment display of the demo adapter
    	- toggle channel 7 to blink the point segment
    	- read back the values from channel 0..6
    	- rotate segments
    	- give out the next number (0..9)
    	and so on
    - close the M66 device
    Note: To abort the programm press CTRL+C


*/

/** \example m66_simp.c */
/** \example m66_demo.c */

/*! \page m66dummy MEN logo
  \menimages
*/


