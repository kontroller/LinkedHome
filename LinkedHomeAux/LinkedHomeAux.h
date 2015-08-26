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

#include "Config.h"
#include "Enocean.h"

#ifdef LINKEDHOME_DEBUG
	#define LINKEDHOME_DEBUG_print Serial.print
	#define LINKEDHOME_DEBUG_println Serial.println
#else
	#define LINKEDHOME_DEBUG_print //
	#define LINKEDHOME_DEBUG_println //
#endif

typedef enum
{
	CMD_NONE,
	CMD_ON,
	CMD_OFF,
	CMD_SEND,				//LinkedHomeMain sends this for a force reading
	CMD_READING,			//LinkedHomeAux sends this as first bye of reading
}LinkedHomeAuxCommand;

//LinkedHome takes 156 bytes
class LinkedHomeAux
{
	EnOcean eno_;			//internal EnOcean object
	bool enabled_;
	uint32 lastTick_;

public:
	LinkedHomeAux();
	void setup();
	void loop();
	void sendReading();
};

LinkedHomeAux::LinkedHomeAux():
	enabled_(true)
{
}

void LinkedHomeAux::setup()
{
	//debug serial
	Serial.begin(LINKEDHOME_DEBUG_SERIAL_BAUD);
	LINKEDHOME_DEBUG_println("LinkedHomeAux::setup()");

	//PIR pin
	pinMode(GAS_SENSOR, INPUT);
	pinMode(ELEC_SENSOR, INPUT);
	pinMode(SOIL_SENSOR, INPUT);

	lastTick_ = millis();
}

void LinkedHomeAux::loop()
{
	eno_.loop();

	if (eno_.hasData())
	{
		if (eno_.data() == CMD_ON)
		{
			enabled_ = true;
			LINKEDHOME_DEBUG_println("Aux Enabled");
		}
		else if (eno_.data() == CMD_OFF)
		{
			enabled_ = false;
			LINKEDHOME_DEBUG_println("Aux Disabled");
		}
		else if (eno_.data() == CMD_SEND)
		{
			sendReading();
		}
	}//if eno_.hasData
	
	if (!enabled_)
		return;

	if (millis() - lastTick_ < AUX_SEND_INTERVAL)
		return;
		
	sendReading();
}

void LinkedHomeAux::sendReading()
{
	LINKEDHOME_DEBUG_println("Aux reading");

	uint32 data;
	uint8 *data_p = (uint8 *)&data;

	data_p[0] = CMD_READING;				//byte 0
	data_p[1] = analogRead(GAS_SENSOR);		//byte 1
	data_p[2] = analogRead(ELEC_SENSOR);	//byte 2
	data_p[3] = analogRead(SOIL_SENSOR);	//byte 3

	LINKEDHOME_DEBUG_println("Aux data: ");
	LINKEDHOME_DEBUG_println(data_p[0]);
	LINKEDHOME_DEBUG_println(data_p[1]);
	LINKEDHOME_DEBUG_println(data_p[2]);
	LINKEDHOME_DEBUG_println(data_p[3]);
	
	eno_.send(data, EO_LINKED_AUX);

	lastTick_ = millis();
}

#endif
