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

#ifndef __ENOCEAN_H__
#define __ENOCEAN_H__

#include "Enocean_ESP3.h"

// Serial1 (HW serial) baud
//	NOTE: do not change this value
#define ENO_SERIAL_BAUD 57600

//serial port timeout in MS
#define ENO_SERIAL_TIMEOUT 10

#ifdef ENO_DEBUG
	#define ENO_DEBUG_print Serial.print
	#define ENO_DEBUG_println Serial.println
#else
	#define ENO_DEBUG_print //
	#define ENO_DEBUG_println //
#endif

/////////////////////////////////////////////////////////
//	EnOcean device Ids
/////////////////////////////////////////////////////////

//the device IDs
#ifndef EO_ROCKER_ID
	#define EO_ROCKER_ID	0x2C8B2900
#endif

#ifndef EO_REED_ID
	#define EO_REED_ID		0x52378101
#endif

#ifndef EO_TEMP_ID
	#define EO_TEMP_ID		0x52378108
#endif

#ifndef EO_LINKED_HOME
	#define EO_LINKED_HOME	0x12345678
#endif

#ifndef EO_LINKED_AUX
	#define EO_LINKED_AUX	0x1234ABCD
#endif

//table for CRC8 computing
const uint8 CRC8Table_[256] = {
  0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d, 0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
  0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d, 0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
  0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd, 0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
  0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea, 0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
  0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a, 0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
  0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a, 0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
  0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4, 0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
  0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44, 0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
  0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f, 0x6A, 0x6d, 0x64, 0x63, 0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
  0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13, 0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8D, 0x84, 0x83,
  0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3
  };

/////////////////////////////////////////////////////////
//	class EnOcean
/////////////////////////////////////////////////////////

class EnOcean
{
	/**
	* ESP3 packet structure through the serial port.
	* Protocol bytes are generated and sent by the application
	*
	* Sync = 0x55
	* CRC8H
	* CRC8D
	*
	*    1              2                1           1           1      u16DataLen + u8OptionLen       1
	* +------+------------------+---------------+-----------+-----------+-------------/------------+-----------+
	* | 0x55 |     u16DataLen   |   u8OptionLen |  u8Type   |    CRC8H  |            DATAS         |   CRC8D   |
	* +------+------------------+---------------+-----------+-----------+-------------/------------+-----------+
	*
	* DATAS structure:
	*                   u16DataLen                    u8OptionLen
	* +--------------------------------------------+----------------------+
	* |                   Data                     |     Optional         |
	* +--------------------------------------------+----------------------+
	*
	*/

	//private members, have '_' suffix
	bool rockerA_, rockerB_;			//rocker A and B event
	bool reedOn_, reedOff_;				//reed on and off event
	bool hasTemp_, hasData_;			//flag for temp and data received

	byte rxBuff_[32];					//Rx and Tx buffer
	TEL_RADIO_TYPE rxRadioTelegram_;	//holds Rx telegram
	TEL_PARAM_TYPE rxTelParam_;			//holds Rx telegram params
	uint32 packetSize_;					//holds Rx packet size
	uint32 data_;						//holds Rx data
	uint32 sender_;

public:
	//public members directly accessed from other classes
	uint32 rockerTs, rockerADuration, rockerBDuration;
	uint32 reedOnDuration;
	byte temperature;

	/////////////////////////////////////////////////////////
	//	public functions
	/////////////////////////////////////////////////////////

	//ctor
	EnOcean():
		rockerA_(false), rockerB_(false),
		reedOn_(false), reedOff_(false),
		hasTemp_(false), hasData_(false),
		data_(0), 
		rockerTs(0), rockerADuration(0), rockerBDuration(0),
		reedOnDuration(0), temperature(0)
	{
		Serial1.begin(ENO_SERIAL_BAUD);
	}

	//reset serial interface and state data
	void reset()
	{
		//reset serial
		Serial1.flush();
		Serial1.end();
		Serial1.begin(ENO_SERIAL_BAUD);

		rockerA_ = rockerB_ = false;
		reedOn_ = reedOff_ = false;
		hasTemp_ = hasData_ = false;
		rockerTs = rockerADuration = rockerBDuration = 0;
		reedOnDuration = 0;
		temperature = 0;
	}

	//returns true if rocker button A was pressed
	bool rockerButtonA()
	{
		if (!(rockerA_ && rockerADuration > 0))
			return false;

		//return value and set to false
		rockerA_ = false;
		return true;
	}

	//returns true if rocker button B was pressed
	bool rockerButtonB()
	{
		if (!(rockerB_ && rockerBDuration > 0))
			return false;

		//return value and set to false
		rockerB_ = false;
		return true;
	}

	//returns true if reed was on
	bool reedOn()
	{
		if (!reedOn_)
			return false;

		//return value and set to false
		reedOn_ = false;
		return true;
	}

	//returns true if reed was off
	bool reedOff()
	{
		if (!reedOff_)
			return false;

		//return value and set to false
		reedOff_ = false;
		return true;
	}

	//returns true if temperature received
	bool hasTemperature()
	{
		if (!hasTemp_)
			return false;

		//return value and set to false
		hasTemp_ = false;
		return true;
	}

	//returns true if data received
	bool hasData()
	{
		if (!hasData_)
			return false;

		//return value and set to false
		hasData_ = false;
		return true;
	}
	
	//returns last received data
	uint32 data()
	{
		return data_;
	}

	//returns last sender id
	uint32 sender()
	{
		return sender_;
	}
	
	//loop() checks for serial data and processes if any
	void loop()
	{
		if (Serial1.available() < ESP3_MIN_LENGTH)
			return;

		//read header
		if (!serialWaitFor_(ESP3_MIN_LENGTH))
			return;

		processPacket_();
	}//read()

	//send 1 byte data
	void send(byte data, uint32 u32Id = EO_LINKED_HOME)
	{
		const int TX_DATA_SIZE = 14;
		uint8 buff[TX_DATA_SIZE];
		
		//RADIO_CHOICE_1BS info
		buff[0] = RADIO_CHOICE_1BS;		//data choice
		buff[1] = data;					//data
		memcpy(&buff[2], &u32Id, 4);	//device id
		buff[6] = 0;					//status

		//PACKET_RADIO info
		buff[7] = 1;					//subtelegram number
		u32Id = 0xFFFF;
		memcpy(&buff[8], &u32Id, 4);	//destination id
		buff[12] = 0xFF;				//dBm
		buff[13] = 0;					//security

		processSend_(PACKET_RADIO, buff, TX_DATA_SIZE);
	}//send()

	//send 4 bytes data
	void send(uint32 data, uint32 u32Id = EO_LINKED_HOME)
	{
		const int TX_DATA_SIZE = 17;
		uint8 buff[TX_DATA_SIZE];

		//RADIO_CHOICE_4BS info
		buff[0] = RADIO_CHOICE_4BS;		//data choice
		memcpy(&buff[1], &data, 4);		//data
		memcpy(&buff[5], &u32Id, 4);	//device id
		buff[9] = 0;					//status

		//PACKET_RADIO info
		buff[10] = 1;					//subtelegram number
		u32Id = 0xFFFF;
		memcpy(&buff[11], &u32Id, 4);	//destination id
		buff[15] = 0xFF;				//dBm
		buff[16] = 0;					//security

		processSend_(PACKET_RADIO, buff, TX_DATA_SIZE);
	}//send()

	/////////////////////////////////////////////////////////
	//	private functions
	/////////////////////////////////////////////////////////
private:

	//waits for number of data from serial port
	//	has a timeout (ENO_SERIAL_TIMEOUT)
	bool serialWaitFor_(int count, int offset = 0)
	{
		//initialize timer and buffer
		unsigned long ts = millis();
		byte *buff = &rxBuff_[offset];

		while(true)
		{
			//if we have bytes, read them
			while (Serial1.available() && (buff - rxBuff_ < count))
				*buff++ = Serial1.read();

			//if we've read all
			if (buff - rxBuff_ == count)
				return true;

			//if timeout
			if (millis() - ts > ENO_SERIAL_TIMEOUT)
				return false;

			//short wait until bytes appear
			byte cx = 0xFF;
			while (!Serial1.available() && cx)
				cx--;
		}//while
	}//serialWaitFor_()

	//processes Rx bytes
	RETURN_TYPE processPacket_()
	{
		//Sync
		if(rxBuff_[0] != ESP3_SYNC_BYTE)
			return EO_NOT_VALID_TEL;

		ENO_DEBUG_println("==============================");
		ENO_DEBUG_println("Valid Sync");

		// Header CRC
		uint8 u8CRC = PROC_CRC8(u8CRC, ((uint8*)rxBuff_)[1]);
		u8CRC = PROC_CRC8(u8CRC, ((uint8*)rxBuff_)[2]);
		u8CRC = PROC_CRC8(u8CRC, ((uint8*)rxBuff_)[3]);
		u8CRC = PROC_CRC8(u8CRC, ((uint8*)rxBuff_)[4]);

		if (u8CRC != ((uint8*)rxBuff_)[5])
			return EO_NOT_VALID_CHKSUM; //bad CRC

		ENO_DEBUG_println("Valid header CRC");

		// When both length fields are 0, then this telegram is not allowed.
		if((rxBuff_[1] == 0) && (rxBuff_[2] ==0) && (rxBuff_[3] == 0))
			return EO_NOT_VALID_TEL;

		//calc packet size
		uint32 dataSize = (((uint32)rxBuff_[1])<<8) + rxBuff_[2] + rxBuff_[3];
		packetSize_ = dataSize + ESP3_MIN_LENGTH + 1; // +CRC8D = 1

		ENO_DEBUG_print("Data size: ");
		ENO_DEBUG_println(dataSize);
		ENO_DEBUG_print("Packet size: ");
		ENO_DEBUG_println(packetSize_);

		//verify min packet size (ESP min len + 1 byte data + 1 byte data CRC)
		if (packetSize_ < ESP3_MIN_LENGTH + 2)
			return EO_NOT_VALID_TEL;

		ENO_DEBUG_println("Valid min len");

		//read rest of the bytes
		if (!serialWaitFor_(packetSize_, ESP3_MIN_LENGTH))
		{
			ENO_DEBUG_println("Could not read rest of packet!");
			return EO_NOT_VALID_TEL;
		}

#ifdef ENO_DEBUG_RX_DATA
		 //print rx buffer
		 ENO_DEBUG_print("rxBuff: ");
		 for (byte ix = 0; ix < packetSize_; ix++)

		 {
			 ENO_DEBUG_print(rxBuff_[ix]);
			 ENO_DEBUG_print(", ");

		 }
		 ENO_DEBUG_println();
#endif

		// Data CRC
		u8CRC = 0;
		for (byte ix = 0; ix < dataSize; ix++)
			u8CRC = PROC_CRC8(u8CRC, rxBuff_[ix + 6]);

		if(u8CRC != ((uint8*)rxBuff_)[packetSize_ - 1])
			return EO_NOT_VALID_CHKSUM;

		ENO_DEBUG_println("Valid data CRC");

		switch(((uint8*)rxBuff_)[4])
		{
			case PACKET_RADIO:
				ENO_DEBUG_println("RADIO Telegram");
				processTelegramType_();
			break;

			case PACKET_RESPONSE:
				ENO_DEBUG_println("RESPONSE Telegram");
				processResponseType_();
			break;

			case PACKET_RADIO_SUB_TEL:
				ENO_DEBUG_println("PACKET_RADIO_SUB_TEL Telegram");
			break;

			case PACKET_EVENT:
				ENO_DEBUG_println("PACKET_EVENT Telegram");
				processEventType_();
			break;

			case PACKET_COMMON_COMMAND:
				ENO_DEBUG_println("PACKET_COMMON_COMMAND Telegram");
			break;

			case PACKET_SMART_ACK_COMMAND:
				ENO_DEBUG_println("PACKET_SMART_ACK_COMMAND Telegram");
			break;

			case PACKET_REMOTE_MAN_COMMAND:
				ENO_DEBUG_println("PACKET_REMOTE_MAN_COMMAND Telegram");
			break;

			case PACKET_PRODUCTION_COMMAND:
				ENO_DEBUG_println("PACKET_PRODUCTION_COMMAND Telegram");
			break;

			case PACKET_RADIO_MESSAGE:
				ENO_DEBUG_println("PACKET_RADIO_MESSAGE Telegram");
			break;

			case PACKET_RADIO_ADVANCED :
				ENO_DEBUG_println("PACKET_RADIO_ADVANCED Telegram");
			break;

			case PACKET_RADIO_SET:
				ENO_DEBUG_println("PACKET_RADIO_SET Telegram");
			break;

			case PACKET_DEBUG:
				ENO_DEBUG_println("PACKET_DEBUG Telegram");
			break;

			default:
				ENO_DEBUG_print("\r\n unknown TYPE");
				break;
		}//switch telegram type

		// DEBUGDisplayRAW(rxRadioTelegram_);
		return EO_NEW_RX_TEL;
	} //processPacket_()

	//processes telegram type packets
	void processTelegramType_()
	{
		//! number of subtelegrams received	(= number of originals + number of repeated)
		rxTelParam_.p_rx.u8SubTelNum = rxBuff_[packetSize_-8];

		ENO_DEBUG_print("Sub tel num: ");
		ENO_DEBUG_println(rxTelParam_.p_rx.u8SubTelNum);

		//! received Destination ID
		memcpy(&(rxTelParam_.p_rx.u32DestinationId), &rxBuff_[packetSize_-7], 4);

		ENO_DEBUG_print("Dest id: ");
		ENO_DEBUG_println(rxTelParam_.p_rx.u32DestinationId);

		//! u8Dbm of the last subtelegram calculated from RSSI.
		//	Note this value is an unsigned value. The real dBm signal is a negative value.
		rxTelParam_.p_rx.u8Dbm = rxBuff_[packetSize_-3];

		ENO_DEBUG_print("RSSI: ");
		ENO_DEBUG_println(rxTelParam_.p_rx.u8Dbm);

		//switch RORG
		switch( ((uint8*)rxBuff_)[6])
		{
			case RADIO_CHOICE_RPS:
				ENO_DEBUG_println("RADIO_CHOICE_RPS");

				rxRadioTelegram_.trps.u8Choice	= RADIO_CHOICE_RPS;
				rxRadioTelegram_.trps.u8Data 	= rxBuff_[7];
				memcpy(&(rxRadioTelegram_.trps.u32Id), &rxBuff_[8], 4);
				rxRadioTelegram_.trps.u8Status 	= rxBuff_[12];
				rxRadioTelegram_.trps.u8Length 	= RADIO_DEC_LENGTH_RPS;
				rxRadioTelegram_.raw.u8Length 	= RADIO_DEC_LENGTH_RPS;

				ENO_DEBUG_print("Id: ");
				ENO_DEBUG_println(rxRadioTelegram_.trps.u32Id);
				ENO_DEBUG_print("Data: ");
				ENO_DEBUG_println(rxRadioTelegram_.trps.u8Data);
				ENO_DEBUG_print("Status: ");
				ENO_DEBUG_println(rxRadioTelegram_.trps.u8Status);

				//if rocker
				if (rxRadioTelegram_.trps.u32Id == EO_ROCKER_ID)
				{
					//rocker button A pressed
					if (rxRadioTelegram_.trps.u8Data == 112)	//N-message
					{
						rockerA_ = true;
						rockerTs = millis();
						rockerADuration = 0;
					}
					//rocker button B pressed
					else if (rxRadioTelegram_.trps.u8Data == 80)	//U-message
					{
						rockerB_ = true;
						rockerTs = millis();
						rockerBDuration = 0;
					}
					//rocker button released
					if (rxRadioTelegram_.trps.u8Data == 0)
					{
						if (rockerA_)
							rockerADuration = millis() - rockerTs;

						//not using else if below, in case B release missed before
						if (rockerB_)
							rockerBDuration = millis() - rockerTs;
					}//if release
				}//if rocker
				break;

			case RADIO_CHOICE_1BS:
				ENO_DEBUG_println("RADIO_CHOICE_1BS");

				rxRadioTelegram_.t1bs.u8Choice	= RADIO_CHOICE_1BS;
				rxRadioTelegram_.t1bs.u8Data 	= rxBuff_[7];
				memcpy(&(rxRadioTelegram_.t1bs.u32Id), &rxBuff_[8], 4);
				rxRadioTelegram_.t1bs.u8Status 	= rxBuff_[12];
				rxRadioTelegram_.t1bs.u8Length 	= RADIO_DEC_LENGTH_1BS;
				rxRadioTelegram_.raw.u8Length 	= RADIO_DEC_LENGTH_1BS;

				ENO_DEBUG_print("Id: ");
				ENO_DEBUG_println(rxRadioTelegram_.t1bs.u32Id);
				ENO_DEBUG_print("Data: ");
				ENO_DEBUG_println(rxRadioTelegram_.t1bs.u8Data);
				ENO_DEBUG_print("Status: ");
				ENO_DEBUG_println(rxRadioTelegram_.t1bs.u8Status);

				//if reed
				if (rxRadioTelegram_.t1bs.u32Id == EO_REED_ID)
				{
					//reed on
					if (rxRadioTelegram_.t1bs.u8Data == 9)
					{
						reedOn_ = true;
						reedOnDuration = millis();
					}
					//reed off
					else if (rxRadioTelegram_.t1bs.u8Data == 8)
					{
						reedOff_ = true;
						reedOnDuration = millis() - reedOnDuration;
					}
				}//if reed
				else
				{
					data_ = rxRadioTelegram_.t1bs.u8Data;
					sender_ = rxRadioTelegram_.t1bs.u32Id;
					hasData_ = true;
				}
				break;

			case RADIO_CHOICE_4BS:
				ENO_DEBUG_println("RADIO_CHOICE_4BS");

				rxRadioTelegram_.t4bs.u8Choice	= RADIO_CHOICE_4BS;
				rxRadioTelegram_.t4bs.u8Data3 	= rxBuff_[7];
				rxRadioTelegram_.t4bs.u8Data2 	= rxBuff_[8];
				rxRadioTelegram_.t4bs.u8Data1 	= rxBuff_[9];
				rxRadioTelegram_.t4bs.u8Data0 	= rxBuff_[10];
				memcpy(&(rxRadioTelegram_.t4bs.u32Id), &rxBuff_[11], 4);
				rxRadioTelegram_.t4bs.u8Status 	= rxBuff_[15];
				rxRadioTelegram_.t4bs.u8Length 	= RADIO_DEC_LENGTH_4BS;
				rxRadioTelegram_.raw.u8Length 	= RADIO_DEC_LENGTH_4BS;

				ENO_DEBUG_print("Id: ");
				ENO_DEBUG_println(rxRadioTelegram_.t4bs.u32Id);
				ENO_DEBUG_print("Data: ");
				ENO_DEBUG_println(rxRadioTelegram_.t4bs.u8Data3);
				ENO_DEBUG_println(rxRadioTelegram_.t4bs.u8Data2);
				ENO_DEBUG_println(rxRadioTelegram_.t4bs.u8Data1);
				ENO_DEBUG_println(rxRadioTelegram_.t4bs.u8Data0);
				ENO_DEBUG_print("Status: ");
				ENO_DEBUG_println(rxRadioTelegram_.t4bs.u8Status);

				//if temp sensor
				if (rxRadioTelegram_.t4bs.u32Id == EO_TEMP_ID)
				{
					temperature = rxRadioTelegram_.t4bs.u8Data1;
					hasTemp_ = true;
				}
				else
				{
					memcpy(&data_, &rxBuff_[7], 4);
					sender_ = rxRadioTelegram_.t4bs.u32Id;
					hasData_ = true;
				}

				ENO_DEBUG_print("Status: ");
				ENO_DEBUG_println(rxRadioTelegram_.t4bs.u8Status);
				break;

			case RADIO_CHOICE_VLD:
				ENO_DEBUG_println("RADIO_CHOICE_VLD");

				rxRadioTelegram_.raw.bytes[0] = RADIO_CHOICE_VLD;
				rxRadioTelegram_.raw.bytes[1] = rxBuff_[7];
				rxRadioTelegram_.raw.bytes[2] = rxBuff_[8];
				rxRadioTelegram_.raw.bytes[3] = rxBuff_[9];

				switch(rxBuff_[7])
				{
					case 0x04:
						ENO_DEBUG_println("CMD4\r\n");
						rxRadioTelegram_.raw.bytes[4] = rxBuff_[10];
						rxRadioTelegram_.raw.bytes[5] = rxBuff_[11];
						rxRadioTelegram_.raw.bytes[6] = rxBuff_[12];
						rxRadioTelegram_.raw.bytes[7] = rxBuff_[13];
						//TODO rxRadioTelegram_.raw.u8Length = 0x0A;
						break;
					case 0x07:
						ENO_DEBUG_println("CMD7\r\n");
						rxRadioTelegram_.raw.bytes[4] = rxBuff_[10];
						rxRadioTelegram_.raw.bytes[5] = rxBuff_[11];
						rxRadioTelegram_.raw.bytes[6] = rxBuff_[12];
						rxRadioTelegram_.raw.bytes[7] = rxBuff_[13];
						rxRadioTelegram_.raw.bytes[8] = rxBuff_[14];
						rxRadioTelegram_.raw.bytes[9] = rxBuff_[15];
						rxRadioTelegram_.raw.bytes[10] = rxBuff_[16];
						//TODO rxRadioTelegram_.raw.u8Length 	= 0x0E;
						break;
					default:
						break;
				}
				break;

			case RADIO_CHOICE_UTE:
				ENO_DEBUG_println("RADIO_CHOICE_UTE");

				rxRadioTelegram_.raw.bytes[0] = RADIO_CHOICE_UTE;
				rxRadioTelegram_.raw.bytes[1] = rxBuff_[7];
				rxRadioTelegram_.raw.bytes[2] = rxBuff_[8];
				rxRadioTelegram_.raw.bytes[3] = rxBuff_[9];
				rxRadioTelegram_.raw.bytes[4] = rxBuff_[10];
				rxRadioTelegram_.raw.bytes[5] = rxBuff_[11];
				rxRadioTelegram_.raw.bytes[6] = rxBuff_[12];
				rxRadioTelegram_.raw.bytes[7] = rxBuff_[13];
				memcpy(&(rxRadioTelegram_.raw.bytes[8]), &rxBuff_[14], 4);
				rxRadioTelegram_.raw.bytes[12] 	= rxBuff_[18];
				//TODO rxRadioTelegram_.raw.u8Length 	= 0xE; //UTE_TELEGRAM_LENGTH;	//0xE
				break;

			default:
				ENO_DEBUG_print("EEP not supported\r\n");
				break;
		}//switch RORG
	}//processTelegramType_()

	//processes response type packet
	// this function only prints debug messages
	void processResponseType_()
	{
		switch(rxBuff_[6])	//RESPONSE_TYPE
		{
			case RES_OK:
				ENO_DEBUG_println("RES_OK");
				break;

			case RES_ERROR:
				ENO_DEBUG_println("RES_ERROR");
				break;

			case RES_NOT_SUPPORTED:
				ENO_DEBUG_println("RES_NOT_SUPPORTED");
				break;

			case RES_WRONG_PARAM:
				ENO_DEBUG_println("RES_WRONG_PARAM");
				break;

			case RES_OPERATION_DENIED:
				ENO_DEBUG_println("RES_OPERATION_DENIED");
				break;

			case RES_USER_CMD:
				ENO_DEBUG_println("RES_USER_CMD");
				break;

			case RES_TIME_OUT:
				ENO_DEBUG_println("RES_TIME_OUT");
				break;

			case RES_NO_RX:
				ENO_DEBUG_println("RES_NO_RX");
				break;

			case RES_ONGOING_RX:
				ENO_DEBUG_println("RES_ERROR");
				break;

			case RES_NEW_RX:
				ENO_DEBUG_println("RES_NEW_RX");
				break;

			case RES_BUFF_FULL:
				ENO_DEBUG_println("RES_BUFF_FULL");
				break;

			case RES_OUT_OF_RANGE:
				ENO_DEBUG_println("RES_OUT_OF_RANGE");
				break;

			case RES_PORT_ERROR:
				ENO_DEBUG_println("RES_PORT_ERROR");
				break;

			case RES_NOT_IMPLEMENTED:
				ENO_DEBUG_println("RES_NOT_IMPLEMENTED");
				break;

			default:
				ENO_DEBUG_println("Invalid response type");
			break;
		}//switch PACKET_RESPONSE:
	}//processResponseType_()

	//processes event type packet
	// this function only prints debug messages
	void processEventType_()
	{
		switch(rxBuff_[6])	//EVENT_TYPE
		{
			case SA_RECLAIM_NOT_SUCCESSFUL:
				ENO_DEBUG_println("SA_RECLAIM_NOT_SUCCESSFUL");
				break;

			case SA_CONFIRM_LEARN:
				ENO_DEBUG_println("SA_CONFIRM_LEARN");
				break;

			case SA_LEARN_ACK:
				ENO_DEBUG_println("SA_LEARN_ACK");
				break;

			case CO_READY:
				ENO_DEBUG_println("CO_READY");
				break;

			case CO_EVENT_SECUREDEVICES:
				ENO_DEBUG_println("CO_EVENT_SECUREDEVICES");
				break;

			case CO_DUTYCYCLE_LIMIT:
				ENO_DEBUG_println("CO_DUTYCYCLE_LIMIT");
				break;

			default:
				ENO_DEBUG_println("Invalid event type");
			break;
		}//switch PACKET_EVENT:
	}//processEventType_()

	//processes Tx packet
	bool processSend_(PACKET_TYPE type, uint8 *buff, uint8 dataLen)
	{
		ENO_DEBUG_println("Tx sync");

		//ESP3 packet:
		/*    1              2                1           1           1      u16DataLen + u8OptionLen       1
		* +------+------------------+---------------+-----------+-----------+-------------/------------+-----------+
		* | 0x55 |     u16DataLen   |   u8OptionLen |  u8Type   |    CRC8H  |            DATAS         |   CRC8D   |
		* +------+------------------+---------------+-----------+-----------+-------------/------------+-----------+
		*/

		//Sync
		rxBuff_[0] = ESP3_SYNC_BYTE;

		//packet size
		rxBuff_[1] = dataLen >> 8;			//u16DataLen H
		rxBuff_[2] = dataLen & 0xFF;		//u16DataLen L
		rxBuff_[3] = 0;						//u8OptionLen

		//packet type
		rxBuff_[4] = type;

		// Header CRC
		uint8 u8CRC = PROC_CRC8(u8CRC, ((uint8*)rxBuff_)[1]);
		u8CRC = PROC_CRC8(u8CRC, ((uint8*)rxBuff_)[2]);
		u8CRC = PROC_CRC8(u8CRC, ((uint8*)rxBuff_)[3]);
		u8CRC = PROC_CRC8(u8CRC, ((uint8*)rxBuff_)[4]);
		rxBuff_[5] = u8CRC;

		ENO_DEBUG_println("Tx CRC");

		//calc packet size
		packetSize_ = dataLen + ESP3_MIN_LENGTH + 1; // +CRC8D = 1

		ENO_DEBUG_print("Tx data size: ");
		ENO_DEBUG_println(dataLen);
		ENO_DEBUG_print("Tx packet size: ");
		ENO_DEBUG_println(packetSize_);

		//copy buff and calc CRC
		u8CRC = 0;
		for (byte ix = 0; ix < dataLen; ix++)
		{
			rxBuff_[ix + 6] = buff[ix];
			u8CRC = PROC_CRC8(u8CRC, buff[ix]);
		}
		rxBuff_[packetSize_ - 1] = u8CRC;

		//Tx packet
		Serial1.write(rxBuff_, packetSize_);

		ENO_DEBUG_println("Tx write done");
		return true;
	}//processSend_()

};//class EnOcean

#endif
