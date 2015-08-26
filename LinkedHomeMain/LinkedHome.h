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

#ifndef __LINKEDHOME_H__
#define __LINKEDHOME_H__

#include <Arduino.h>
#include <LSD.h>
#include "Config.h"
#include "EnergSocket.h"
#include "LinkedHomeUI.h"

#ifdef LINKEDHOME_DEBUG
	#define LINKEDHOME_DEBUG_print Serial.print
	#define LINKEDHOME_DEBUG_println Serial.println
#else
	#define LINKEDHOME_DEBUG_print //
	#define LINKEDHOME_DEBUG_println //
#endif

/////////////////////////////////////////////////////////
//	LinkedHome state, command and sensor type
/////////////////////////////////////////////////////////

//defines LinkedHome state (on, off, etc)
typedef enum {
	LinkedHome_Uninit,
	LinkedHome_Off,
	LinkedHome_On
}LinkedHomeState;

//defines commands sent/received to/from aux unit
typedef enum
{
	CMD_NONE,
	CMD_ON,
	CMD_OFF,
	CMD_SEND,				//LinkedHomeMain sends this for a force reading
	CMD_READING,			//LinkedHomeAux sends this as first bye of reading
}LinkedHomeAuxCommand;

//defines all sensors
typedef enum
{
	LOCAL_PIR,
	RF_REED_ON,
	RF_TEMP,
	RF_GAS,
	RF_ELEC,
	RF_SOIL,
	RF_PIR,
	RF_OUTLET,
	RF_REED_OFF,			//RF_REED_OFF is not a separate sensor, but a state
							//	this reduces a lot of code
	TOTAL_LINKED_SENSORS,	//holds count of total sensors we have
}LinkedSensors;

/////////////////////////////////////////////////////////
//	class LinkedHome
/////////////////////////////////////////////////////////

class EnOcean;

//defines the LinkedHome main unit
class LinkedHome
{
	//private members
	EnOcean *eno_;								//internal EnOcean object
	EnergSocket energ_;							//Energenie socket

	LinkedHomeState state_;						//state machine
	LinkedHomeUI ui_;							//the UI (OLED/Web/SMS) handler

	bool sensorEnable_[TOTAL_LINKED_SENSORS];	//sensor config
	uint32 alarmTS_[TOTAL_LINKED_SENSORS];		//last alarm timestamps

	bool turnOn_, turnOff_, pirOn_;
	String info_;								//temperature info to show on LCD
	String wifiInfo_;							//wifi info to show on LCD
	uint32 alarmInterval_;						//min interval in between alarms
	LFile file_;

	friend class LinkedHomeUI;

public:
	LinkedHome();
	LinkedHomeUI &ui() { return ui_; }
	String &info() { return info_; }
	String &wifiInfo() { return wifiInfo_; }

	void setup();
	void loop();
	void checkPower();

	void enablePower(bool val);
	void enableSensor(LinkedSensors, bool val);

	void powerEvent();
	void sensorEvent(LinkedSensors);

	bool detectPIR();
	String getStatus(bool webFormat = false);

private:
	void logEvent_(const char *msg);
	void formatTime_(uint32 ms, String &out);
	void sendAlarm_(LinkedSensors sensor, const char *msg, const char *val = 0);

};

#endif
