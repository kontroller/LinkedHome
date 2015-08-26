/*
	Copyright(c) 2014, Kontroller (kontroller.blog@gmail.com)
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met :

	*Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.

	* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and / or other materials provided with the distribution.

	* Neither the name of LinkedHome nor the names of its
	contributors may be used to endorse or promote products derived from
	this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __EO_ESP3_H__
#define __EO_ESP3_H__

#include "Config.h"

//Enocean synchronization byte
#define ESP3_SYNC_BYTE 0x55

//! Length of RX/TX radio buffers in bytes - the maximum decoded telegram length 
#define ESP3_MSG_LENGTH 21

//min len = 6 bytes (sync to crc8h)
#define ESP3_MIN_LENGTH 6

//compute the CRC8
#define PROC_CRC8(u8CRC, u8Data) (CRC8Table_[u8CRC ^ u8Data]) 

//Reference to EnOcean Serial Protocol 3:
//https://www.enocean.com/fileadmin/redaktion/pdf/tec_docs/EnOceanSerialProtocol3_V1.17.pdf

//ESP3 Packet Type
typedef enum
{
	PACKET_RESERVED = 0x00,	//!Reserved
	PACKET_RADIO = 0x01,	//!Radio telegram
	PACKET_RESPONSE = 0x02,	//!Response to any packet
	PACKET_RADIO_SUB_TEL = 0x03,//!Radio subtelegram (EnOcean internal function�)
	PACKET_EVENT = 0x04,	//!Event message
	PACKET_COMMON_COMMAND = 0x05,	//!Common command
	PACKET_SMART_ACK_COMMAND = 0x06,	//!Smart Ack command
	PACKET_REMOTE_MAN_COMMAND = 0x07,	//!Remote management command
	PACKET_PRODUCTION_COMMAND = 0x08,	//!Production command
	PACKET_RADIO_MESSAGE = 0x09,	///!Radio message (chained radio telegrams)
	PACKET_RADIO_ADVANCED  = 0x0A, //!Advanced protocol radio telegram
	PACKET_RADIO_SET = 0x0E,	//!set radio stuff&answear
	PACKET_DEBUG = 0x0F	//!debug message
} PACKET_TYPE;

//ESP3 Response Type
typedef enum
{
	//! Return values are the same as for the ESP3 command response
	//! <b>0x00</b> - Action performed. No problem detected
	RES_OK = 0x00, //!RES_OK ... command is understood and triggered
	//! <b>0x01</b> - Generic Error
	RES_ERROR = 0x01, //!There is an error occurred
	//! <b>0x02</b> - The functionality is not supported by that implementation
	RES_NOT_SUPPORTED = 0x02, //!The functionality is not supported by that implementation
	//! <b>0x03</b> - There was a wrong function parameter
	RES_WRONG_PARAM = 0x03, //!Wrong function parameter
	//! <b>0x04</b> - Operation denied
	RES_OPERATION_DENIED = 0x04, //!Example: memory access denied (code-protected)
	//! <b>0x80</b> - User command
	RES_USER_CMD = 0x80,//!Return codes greater than 0x80 are used for commands with special return information, not commonly useable.

	//! EO-Link specific return codes
	//! <b>0x05</b> - Action couldn't be carried out within a certain time.
	RES_TIME_OUT = 0x05, //!< TIME_OUT
	//! <b>0x06</b> - No byte or telegram received
	RES_NO_RX = 0x06, //!< NO_RX
	//! <b>0x07</b> - The UART receive is ongoing
	RES_ONGOING_RX = 0x07, //!< ONGOING_RX
	//! <b>0x08</b> - A new byte or telegram received
	RES_NEW_RX = 0x08, //!< NEW_RX
	//! <b>0x09</b> - Buffer full, no space in Tx or Rx buffer
	RES_BUFF_FULL = 0x09, //!< BUFF_FULL
	//!	<b>0x0A</b> - Generic out of range return code e.g. address is out of range or the buffer is too small
	RES_OUT_OF_RANGE = 0x0A, //!< OUT_OF_RANGE
	//! <b>0x0B</b> - Error Opening Port
	RES_PORT_ERROR = 0x0B, //!< PORT_ERROR
	//! <b>0xFF</b> - This function is not implemented
	RES_NOT_IMPLEMENTED = 0xFF  //!< NOT_IMPLEMENTED
} RESPONSE_TYPE;

//ESP3 Choice Type (same as RORGS)
typedef enum
{
	//! RPS telegram
	RADIO_CHOICE_RPS = 0xF6,
	//! 1BS telegram
	RADIO_CHOICE_1BS = 0xD5,
	//! 4BS telegram
	RADIO_CHOICE_4BS = 0xA5,
	//! HRC telegram
	RADIO_CHOICE_HRC = 0xA3,
	//! SYS telegram
	RADIO_CHOICE_SYS = 0xA4,
	//! SYS_EX telegram
	RADIO_CHOICE_SYS_EX = 0xC5,
	//! Smart Ack Learn Request telegram
	RADIO_CHOICE_SM_LRN_REQ = 0xC6,
	//! Smart Ack Learn Answer telegram
	RADIO_CHOICE_SM_LRN_ANS = 0xC7,
	//! Smart Ack Reclaim telegram
	RADIO_CHOICE_RECLAIM = 0xA7,
	//! Smart Request telegram
	RADIO_CHOICE_SIGNAL = 0xD0,
	//! Encapsulated addressable telegram
	RADIO_CHOICE_ADT = 0xA6,
	//! Variable Length Data
	RADIO_CHOICE_VLD = 0xD2,
	//! Universal Teach In EEP based
	RADIO_CHOICE_UTE = 0xD4,
	//! Manufacturer Specific Communication
	RADIO_CHOICE_MSC = 0xD1,
	//! Single Teach-In Request
	RADIO_CHOICE_GP_SINGLE_TREQ = 0xB0,
	//! Chained Teach-In Request
	RADIO_CHOICE_GP_CHAINED_TREQ = 0xB1,
	//! Single Teach-In Response
	RADIO_CHOICE_GP_SINGLE_TRES = 0xB2,
	//! Chained Teach-In Response
	RADIO_CHOICE_GP_CHAINED_TRES = 0xB3,
	//! Single Data
	RADIO_CHOICE_GP_SINGLE_DATA = 0xB4,
	//! Chained Data
	RADIO_CHOICE_GP_CHAINED_DATA = 0xB5,
	//! Selective data
	RADIO_CHOICE_GP_SELECT_DATA = 0xB6,
	//! Secure telegram
	RADIO_CHOICE_SEC = 0x30,
	//! Secure telegram	with choice encapsulation
	RADIO_CHOICE_SEC_ENCAPS = 0x31,
	//! Non-secure telegram
	RADIO_CHOICE_NON_SEC = 0x32,
	//! Secure teach-in telegram
	RADIO_CHOICE_SEC_TI = 0x35,

} CHOICE_TYPE;

//ESP3 Common Command Type
typedef enum
{
	//! Order to enter in energy saving mode
	CO_WR_SLEEP = 1,
	//! Order to reset the device
	CO_WR_RESET = 2,
	//! Read the device (SW) version / (HW) version, chip ID etc.
	CO_RD_VERSION = 3,
	//! Read system log from device databank
	CO_RD_SYS_LOG = 4,
	//! Reset System log from device databank
	CO_WR_SYS_LOG = 5,
	//! Perform Flash BIST operation
	CO_WR_BIST = 6,
	//! Write ID range base number
	CO_WR_IDBASE = 7,
	//! Read ID range base number
	CO_RD_IDBASE = 8,
	//! Write Repeater Level off,1,2
	CO_WR_REPEATER = 9,
	//! Read Repeater Level off,1,2
	CO_RD_REPEATER = 10,
	//! Add filter to filter list
	CO_WR_FILTER_ADD = 11,
	//! Delete filter from filter list
	CO_WR_FILTER_DEL = 12,
	//! Delete filters
	CO_WR_FILTER_DEL_ALL = 13,
	//! Enable/Disable supplied filters
	CO_WR_FILTER_ENABLE = 14,
	//! Read supplied filters
	CO_RD_FILTER = 15,
	//! Waiting till end of maturity time before received radio telegrams will transmitted
	CO_WR_WAIT_MATURITY = 16,
	//! Enable/Disable transmitting additional subtelegram info
	CO_WR_SUBTEL = 17,
	//! Write x bytes of the Flash, XRAM, RAM0 ….
	CO_WR_MEM = 18,
	//! Read x bytes of the Flash, XRAM, RAM0 ….
	CO_RD_MEM = 19,
	//! Feedback about the used address and length of the config area and the Smart Ack Table
	CO_RD_MEM_ADDRESS = 20,
	//! Read security informations (level, keys)
	CO_RD_SECURITY = 21,
	//! Write security informations (level, keys)
	CO_WR_SECURITY = 22,
} COMMON_COMMAND_TYPE;

//ESP3 Event Type
typedef enum
{
	SA_RECLAIM_NOT_SUCCESSFUL = 1,	//Informs the backbone of a Smart Ack Client to not successful reclaim.
	SA_CONFIRM_LEARN, // Used for SMACK to confirm/discard learn in/out
	SA_LEARN_ACK, // Inform backbone about result of learn request
	CO_READY, // Inform backbone about the readiness for operation
	CO_EVENT_SECUREDEVICES, // Informs about a secure device
	CO_DUTYCYCLE_LIMIT
} EVENT_TYPE;

//Function Return Code
typedef enum
{
	//! <b>0</b> - Action performed. No problem detected
	EO_OK = 0,							
	//! <b>1</b> - Action couldn't be carried out within a certain time.  
	EO_TIME_OUT,		
	//! <b>2</b> - The write/erase/verify process failed, the flash page seems to be corrupted
	EO_FLASH_HW_ERROR,				
	//! <b>3</b> - A new UART/SPI byte received
	EO_NEW_RX_BYTE,				
	//! <b>4</b> - No new UART/SPI byte received	
	EO_NO_RX_BYTE,					
	//! <b>5</b> - New telegram received
	EO_NEW_RX_TEL,	  
	//! <b>6</b> - No new telegram received
	EO_NO_RX_TEL,	  
	//! <b>7</b> - Checksum not valid
	EO_NOT_VALID_CHKSUM,
	//! <b>8</b> - Telegram not valid  
	EO_NOT_VALID_TEL,
	//! <b>9</b> - Buffer full, no space in Tx or Rx buffer
	EO_BUFF_FULL,
	//! <b>10</b> - Address is out of memory
	EO_ADDR_OUT_OF_MEM,
	//! <b>11</b> - Invalid function parameter
	EO_NOT_VALID_PARAM,
	//! <b>12</b> - Built in self test failed
	EO_BIST_FAILED,
	//! <b>13</b> - Before entering power down, the short term timer had timed out.	
	EO_ST_TIMEOUT_BEFORE_SLEEP,
	//! <b>14</b> - Maximum number of filters reached, no more filter possible
	EO_MAX_FILTER_REACHED,
	//! <b>15</b> - Filter to delete not found
	EO_FILTER_NOT_FOUND,
	//! <b>16</b> - BaseID out of range
	EO_BASEID_OUT_OF_RANGE,
	//! <b>17</b> - BaseID was changed 10 times, no more changes are allowed
	EO_BASEID_MAX_REACHED,
	//! <b>18</b> - XTAL is not stable
	EO_XTAL_NOT_STABLE,
	//! <b>19</b> - No telegram for transmission in queue  
	EO_NO_TX_TEL,
	//!	<b>20</b> - Waiting before sending broadcast message
	EO_TELEGRAM_WAIT,
	//!	<b>21</b> - Generic out of range return code
	EO_OUT_OF_RANGE,
	//!	<b>22</b> - Function was not executed due to sending lock
	EO_LOCK_SET,
	//! <b>23</b> - New telegram transmitted
	EO_NEW_TX_TEL
} RETURN_TYPE;

//Length of the radio telegrams
typedef enum
{
	//! RPS decoded telegram length	in bytes
	RADIO_DEC_LENGTH_RPS =			 8,			 
	//! 1BS decoded telegram length	in bytes
	RADIO_DEC_LENGTH_1BS =			 8,			 
	//! 4BS decoded telegram length	in bytes
	RADIO_DEC_LENGTH_4BS =			 11,			 
	//! HRC decoded telegram length	in bytes
	RADIO_DEC_LENGTH_HRC =			 8,			 
	//! SYS decoded telegram length	in bytes
	RADIO_DEC_LENGTH_SYS =			 11,			 
	//! SYS_EX decoded telegram length	in bytes
	RADIO_DEC_LENGTH_SYS_EX =		 16,			 
	//! Smart ack learn request decoded telegram length in bytes
	RADIO_DEC_LENGTH_SM_LRN_REQ =    17,
	//! Smart ack learn answer telegram length in bytes
	RADIO_DEC_LENGTH_SM_LRN_ANS =    15,	 
	//! Smart ack reclaim telegram length in bytes
	RADIO_DEC_LENGTH_SM_REC =        8,
	//! Signal telegram length in bytes
	RADIO_DEC_LENGTH_SIGNAL =        8,	 
	//! U2S decoded telegram length in bytes
	RADIO_DEC_LENGTH_U2S =           6,
	//! T2S decoded telegram length iin bytes
	RADIO_DEC_LENGTH_T2S =           6,	
	//! UTE
	RADIO_DEC_LENGTH_UTE = 			20,
} RADIO_TEL_LENGTH;

//Telegram structure used for Radio transmission.
typedef union {
	struct raw_TEL_RADIO_TYPE
	{
		//! Telegram seen as a array of bytes without logical structure.
		uint8 bytes[ESP3_MSG_LENGTH];
		//! Radio telegram length, it's not part of the transmitted/received data, it is used only for processing the telegram
		RADIO_TEL_LENGTH u8Length;
	} raw;
			
	//! RPS/1BS/HRC/smart_req radio telegram structure
	struct trps_t1bs_thrc_smart_req{
		CHOICE_TYPE u8Choice;
		uint8  		u8Data;
		uint32 		u32Id;
		uint8  		u8Status;											
		uint8  		u8Chk;
		uint8		u8Fill[ESP3_MSG_LENGTH-8];
		RADIO_TEL_LENGTH u8Length;
	}trps, t1bs, thrc, smart_req;	

	//! 4BS/SYS radio telegram structure
	struct t4bs_tsys{
		CHOICE_TYPE u8Choice;
		uint8  		u8Data3;
		uint8  		u8Data2;
		uint8  		u8Data1;
		uint8  		u8Data0;
		uint32 		u32Id;
		uint8  		u8Status;
		uint8  		u8Chk;
		uint8		u8Fill[ESP3_MSG_LENGTH-11];
		RADIO_TEL_LENGTH u8Length;
	}t4bs,tsys;							   

} TEL_RADIO_TYPE;	

//Telegram parameter structure
typedef union 
{
	//! param structure for transmitted telegrams
	struct p_tx {
		//! number of subtelegrams to send 
		uint8  u8SubTelNum;
		//! to send Destination ID
		uint32 u32DestinationId;

	}p_tx;

	//! param structure for received telegrams
	struct p_rx{
		//! number of subtelegrams received	(= number of originals + number of repeated)
		uint8  u8SubTelNum;
		//! received Destination ID
		uint32 u32DestinationId;
		//! u8Dbm of the last subtelegram calculated from RSSI. Note this value is an unsigned value. The real dBm signal is a negative value.
		uint8 u8Dbm;
	}p_rx;
} TEL_PARAM_TYPE;

#endif 
