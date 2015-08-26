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

#include <LSD.h>
#include "LinkedHome.h"
#include "Enocean.h"

//readable names of the sensors
const char *LinkedSensorNames[TOTAL_LINKED_SENSORS] =
{
	"Motion",
	"Reed",
	"Rf Temp",
	"Rf Gas",
	"Rf Electricity",
	"Rf Soil",
	"Rf Motion",
	"Rf Outlet",
	"Reed Off",
};

/////////////////////////////////////////////////////////
//	ctor and setup()
/////////////////////////////////////////////////////////

//ctor
LinkedHome::LinkedHome()
	: eno_(NULL), ui_(*this), turnOn_(false), turnOff_(false), pirOn_(false),
	alarmInterval_(ALARM_INTERVAL * 1000)
{
	eno_ = new EnOcean();

	//clear arrays
	memset(alarmTS_, 0, sizeof(uint32) * TOTAL_LINKED_SENSORS);
	memset(sensorEnable_, 0, sizeof(byte) * TOTAL_LINKED_SENSORS);

	//enable sensors
	sensorEnable_[RF_REED_ON] = true;
	sensorEnable_[RF_TEMP] = true;
	//sensorEnable_[RF_GAS] = true;
	//sensorEnable_[LOCAL_PIR] = true;
}//LinkedHome()

//setup()
void LinkedHome::setup()
{
	//debug serial
	Serial.begin(LINKEDHOME_DEBUG_SERIAL_BAUD);
	LINKEDHOME_DEBUG_println("LinkedHome::setup()");
	
	//PIR pin
	pinMode(PIR_SENSOR, INPUT);
	
	//ui
	ui_.setup();
	
	//energenie
	energ_.setup();
	energ_.set(ENERG_SOCKET_1, false);

	file_ = LSD.open("linkedhome.log", FILE_WRITE);

	//init state
	state_ = LinkedHome_Off;
}//setup()

/////////////////////////////////////////////////////////
//	Loop functions
/////////////////////////////////////////////////////////

//int cx = 0;

//loop()
void LinkedHome::loop()
{
	//LINKEDHOME_DEBUG_print(cx++);
	//LINKEDHOME_DEBUG_print("\r");

	eno_->loop();
	ui_.loop();

	if (state_ == LinkedHome_On)
	{
		//PIR
		if (sensorEnable_[LOCAL_PIR] && detectPIR())
			sensorEvent(LOCAL_PIR);

		//Reed switches
		if (sensorEnable_[RF_REED_ON] && eno_->reedOn())
			sensorEvent(RF_REED_ON);

		//NOTE: following RF_REED_ON is not a typo
		if (sensorEnable_[RF_REED_ON] && eno_->reedOff())
			sensorEvent(RF_REED_OFF);

		//RF temp
		if (sensorEnable_[RF_TEMP] && eno_->hasTemperature())
			sensorEvent(RF_TEMP);

		//Gas, Elec, Soil
		if (eno_->hasData() && eno_->sender() == EO_LINKED_AUX)
			sensorEvent(RF_GAS);
	}//if state_ == on

	checkPower();
}//loop()

//checkPower()
void LinkedHome::checkPower()
{	
	//if rocker A pressed
	if (turnOn_ || eno_->rockerButtonA())
	{
		//if off
		if (state_ == LinkedHome_Off)
		{
			state_ = LinkedHome_On;
			powerEvent();
		}

		//if wifi not connected
		else if (!ui_.isWifiConnected())
		{
			//pressed for 2 sec?
			if (eno_->rockerADuration > EO_ROCKER_WIFI_MIN)
				ui_.connectWifi();
			else
				ui_.oledShow("Hold 'On' button for 2 sec to turn Wifi on.");
		}

		//otherwise, we're fine
		else
		{
			ui_.oledShow("LinkedHome activated and Wifi is on!");
		}

		turnOn_ = false;
	}//if rocker A

	//if rocker B pressed
	if (turnOff_ || eno_->rockerButtonB())
	{
		//if on
		if (state_ == LinkedHome_On)
		{
			//pressed for 2 sec?
			if (turnOff_ || eno_->rockerBDuration > EO_ROCKER_OFF_MIN)
			{
				state_ = LinkedHome_Off;
				powerEvent();
			}
			else
				ui_.oledShow("Hold 'Off' button for 2 sec to turn off.");
		}
		else
			ui_.oledShow("LinkedHome is already Off!");

		turnOff_ = false;
	}//if rocker B
	
}//void loop()

/////////////////////////////////////////////////////////
//	Enable/disable functions
/////////////////////////////////////////////////////////

// callback methods for web and text UI
// these functions needs to be very lean
//activate/deactivate main module
void LinkedHome::enablePower(bool val)
{
	if (val)
		turnOn_ = true;
	else
		turnOff_ = true;

	checkPower();
}

//enables/disables a sensor
//	param val: true enables sensor, false disables
void LinkedHome::enableSensor(LinkedSensors sensor, bool val)
{
	if (sensor == RF_OUTLET)
		energ_.set(ENERG_SOCKET_1, val);

	//display message
	String buff = LinkedSensorNames[sensor];
	
	if (sensorEnable_[sensor] == val)
		buff += " is already ";
	else
	{
		sensorEnable_[sensor] = val;
		buff += " turned ";
	}
	
	buff += val ? "ON" : "OFF";
	ui_.oledShow(buff.c_str());
}

/////////////////////////////////////////////////////////
//	Events
/////////////////////////////////////////////////////////

//powerEvent()
void LinkedHome::powerEvent()
{
	const char *msg = 0;
	
	//send SMS and show in OLED
	if (state_ == LinkedHome_On)
		ui_.sendSMS("LinkedHome turned On!");
	else
		ui_.sendSMS("WARNING: LinkedHome turned Off!");
}

//sensorEvent()
void LinkedHome::sensorEvent(LinkedSensors sensor)
{
	LINKEDHOME_DEBUG_print(LinkedSensorNames[sensor]);
	LINKEDHOME_DEBUG_println(" Event");

	//Pir
	if (sensor == LOCAL_PIR)
		sendAlarm_(sensor, "Alarm: Motion detected!");

	//Reed On
	else if (sensor == RF_REED_ON)
	{
		pirOn_ = true;
		sendAlarm_(sensor, "Alarm: Door closed!");
	}

	//Reed Off
	else if (sensor == RF_REED_OFF)
	{
		if (pirOn_)
			sendAlarm_(sensor, "Alarm: Door opened!");

		pirOn_ = false;
	}

	//RF temp
	else if (sensor == RF_TEMP)
	{
		eno_->temperature -= RF_TEMP_ADJUST;

		info_ = "Temp: ";
		info_ += eno_->temperature;
		info_ += " (F)";
		ui_.oledShow(info_.c_str());
		
		if (eno_->temperature <= RF_TEMP_MIN)
			sendAlarm_(sensor, "Alarm: Temp is too low! ", info_.c_str());

		else if (eno_->temperature >= RF_TEMP_MAX)
			sendAlarm_(sensor, "Alarm: Temp is too high! ", info_.c_str());

		else
			logEvent_(info_.c_str());
	}
	
	//we receive the following 3 sensors togather from enocean
	//	sensorEnable_[] is not checked before calling sensorEvent()
	else if (sensor == RF_GAS || sensor == RF_ELEC || sensor == RF_SOIL)
	{
		uint32 data = eno_->data();
		uint8 *data_p = (uint8 *)&data;

		LINKEDHOME_DEBUG_println("Rx Aux data: ");
		LINKEDHOME_DEBUG_println(data_p[0]);
		LINKEDHOME_DEBUG_println(data_p[1]);
		LINKEDHOME_DEBUG_println(data_p[2]);
		LINKEDHOME_DEBUG_println(data_p[3]);

		if (data_p[0] != CMD_READING)
			return;	//invalid signature

		//Gas
		if (sensorEnable_[RF_GAS])
		{
			String val = "Gas: ";
			val += data_p[1];

			if (data_p[1] >= RF_GAS_MAX)
				sendAlarm_(RF_GAS, "Alarm: Gas/Smoke detected! ", val.c_str());
			else
				logEvent_(val.c_str());
		}

		//Elec
		if (sensorEnable_[RF_ELEC])
		{
			String val = "Amp: ";
			val += data_p[2];

			if (data_p[2] >= RF_ELEC_MAX)
				sendAlarm_(RF_ELEC, "Alarm: High electricity use! ", val.c_str());
			else
				logEvent_(val.c_str());
		}

		//Soil
		if (sensorEnable_[RF_SOIL])
		{
			String val = "Soil: ";
			val += data_p[3];

			if (data_p[3] < RF_SOIL_MIN)
				sendAlarm_(RF_SOIL, "Alarm: Soil moisture is low! ", val.c_str());
			else
				logEvent_(val.c_str());
		}
	}//if

}//sensorEvent()

bool LinkedHome::detectPIR()
{
	return (analogRead(PIR_SENSOR) > 100);
}

String LinkedHome::getStatus(bool webFormat)
{
	const char *CRLF = webFormat ? "<BR>" : "\n";
	String out;

	out += "Up for ";
	formatTime_(millis(), out);
	out += CRLF;

	out += "Wifi is ";
	out += ui_.isWifiConnected() ? "ON" : "OFF";
	out += CRLF;

	if (eno_->temperature)
	{
		out += "Temp is ";
		out += eno_->temperature;
		out += " (F)";
		out += CRLF;
	}

	for (int ix = 0; ix < TOTAL_LINKED_SENSORS; ++ix)
	{
		if (ix == RF_REED_OFF)
			continue;

		out += LinkedSensorNames[ix];
		out += sensorEnable_[ix] ? " is ON" : " is OFF";
		out += CRLF;
	}

	return out;
}

/////////////////////////////////////////////////////////
//	SD card logging
/////////////////////////////////////////////////////////

//logs sensor data and timestamp to SD card
void LinkedHome::logEvent_(const char *msg)
{
	if (file_)
		file_.write(msg);
}

/////////////////////////////////////////////////////////
//	misc functions
/////////////////////////////////////////////////////////

void LinkedHome::formatTime_(uint32 ms, String &out)
{
	uint32 sec, min, hr, day, month;
	
	sec = ms / 1000;
	min = sec / 60;
	hr = min / 60;
	day = hr / 24;
	month = day / 30;

	sec %= 60;
	min %= 60;
	hr %= 24;
	day %= 30;

	if (month > 0)
	{
		out += month;
		out += " month(s), ";
	}

	if (day > 0 || month > 0)
	{
		out += day;
		out += " day(s), ";
	}

	if (hr > 0 || day > 0 || month > 0)
	{
		out += hr;
		out += " hr, ";
	}

	out += min;
	out += " min, ";
	out += sec;
	out += " sec";
}

//following function sends alarms
//	it'll skip alarms within min interval
//	only exception is reed events
void LinkedHome::sendAlarm_(LinkedSensors sensor, const char *msg, const char *val)
{
	//we'll log all alarms
	logEvent_(msg);
	if (val)
		logEvent_(val);

	if (!(sensor == RF_REED_ON || sensor == RF_REED_OFF))
	{
		//are we sending too many alarms?
		if (millis() - alarmTS_[sensor] < alarmInterval_)
			return;

		alarmTS_[sensor] = millis();
	}//if not reed even

	//if we have value
	if (val)
	{
		//concate both msg and val
		String str = msg;
		str += val;

		ui_.sendSMS(str.c_str());
	}
	else
		ui_.sendSMS(msg);
}
