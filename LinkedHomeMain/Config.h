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

#ifndef __CONFIG_H__
#define __CONFIG_H__

//LinkIt has two serial ports:
//	Serial1 is the hw serial, used by EnOcean
//	Serial is the virtual USB serial, used by PC
//
//This is the buad for virtual serial port for PC
#define LINKEDHOME_DEBUG_SERIAL_BAUD 115200

//Macro to enable Wifi and web server
#define ENABLE_WIFI

//Macro to enable GSM (SMS)
#define ENABLE_GSM

//Macro to enable logging of LinkedHome and LinkedHomeUI events
#define LINKEDHOME_DEBUG

//Macro to enable logging of Web Server
//#define WEB_SERVER_DEBUG

//Macro to enable logging of Enocean protocol (ESP3)
//#define ENO_DEBUG

//Wifi Config
#define WIFI_AP "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP

//GSM Config
#define LINKHOME_OWNER_NUM "+00"

//web port
#define WEB_SERVER_PORT 80

//Motion sensor pin
#define PIR_SENSOR A0

//EnOcean device IDs
#define EO_ROCKER_ID	0x2C8B2900
#define EO_REED_ID		0x52378101
#define EO_TEMP_ID		0x52378108
#define EO_LINKED_HOME	0x12345678
#define EO_LINKED_AUX	0x1234ABCD

//min duration (ms) to hold rocker off button
#define EO_ROCKER_OFF_MIN 2000
#define EO_ROCKER_WIFI_MIN 2000

//alarm interval in seconds, default 1 min
#define ALARM_INTERVAL 30

//RF temperature offset
#define RF_TEMP_ADJUST 5

//RF temp alarms
#define RF_TEMP_MIN 60
#define RF_TEMP_MAX 90

//RF Gas max
#define RF_GAS_MAX 100

//RF Elec max
#define RF_ELEC_MAX 90

//RF Soil min
#define RF_SOIL_MIN 50

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long int uint32;

#endif
