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

#include <LGSM.h>
#include <Wire.h>
#include "LinkedHome.h"
#include "SeeedOLED.h"
#include "WebServer.h"

/////////////////////////////////////////////////////////
//	ctor, dtor and setup
/////////////////////////////////////////////////////////

//ctor
LinkedHomeUI::LinkedHomeUI(LinkedHome &p_lh):
	lh_(p_lh), oledTick_(0), smsTick_(0), webServer_(NULL)
{
#ifdef ENABLE_WIFI
	webServer_ = new WebServer(lh_, WEB_SERVER_PORT);
#endif
	memset(uiTS_, 0, sizeof(uint32) * TOTAL_LINKED_UIs);
}

//dtor
LinkedHomeUI::~LinkedHomeUI()
{
	delete webServer_;
}

//setup()
void LinkedHomeUI::setup()
{
	LINKEDHOME_DEBUG_println("LinkedHomeUI::setup()");

	//OLED
	Wire.begin();
	Oled.init();  				//initialze SEEED OLED display
	Oled.clearDisplay();        //clear the screen and set start position to top left corner
	Oled.setNormalDisplay();    //Set display to normal mode (i.e non-inverse mode)
	Oled.setHorizontalMode();
	
#ifdef ENABLE_WIFI
	//Web server
	webServer_->setup();
#endif
}

//isWifiConnected() - abstracts webServer function
//	required to hide WebServer details from LinkedHome class
bool LinkedHomeUI::isWifiConnected()
{
#ifdef ENABLE_WIFI
	return webServer_->isWifiConnected();
#endif
	return false;
}

//connectWifi() - abstracts webServer function
//	required to hide WebServer details from LinkedHome class
void LinkedHomeUI::connectWifi()
{
#ifdef ENABLE_WIFI
	webServer_->connectWifi();
#else
	oledShow("Wifi is disabled!");
#endif
}

/////////////////////////////////////////////////////////
//	loops
/////////////////////////////////////////////////////////

//OLEDloop() brings back default message after timeout
void LinkedHomeUI::loop()
{
#ifdef ENABLE_WIFI
	//check for web request
	webServer_->loop();
#endif

	//check for SMS
	gsmLoop();

	//handle OLED timeout
	if (uiTS_[OLED] == 0)
		return;							//timer not enabled

	if (millis() - oledTick_ < 1000)
		return;

	oledTick_ = millis();
	--uiTS_[OLED];

	if (uiTS_[OLED] == 0)
	{
		if (lh_.state_ == LinkedHome_On)
			oledShow("LinkedHome Activated", 0);
		else
			oledShow("LinkedHome Offline", 0);

		//show temperature
		if (lh_.info().length() > 0)
		{
			Oled.setTextXY(3, 0);
			Oled.putString(lh_.info().c_str());
		}

		//show wifi info
		if (lh_.wifiInfo().length() > 0)
		{
			Oled.setTextXY(5, 0);
			Oled.putString(lh_.wifiInfo().c_str());
		}

	}
}//loop()

//checks and processes SMS received
void LinkedHomeUI::gsmLoop()
{
#ifdef ENABLE_GSM
	//we'll only check SMS once every 2 sec
	if (millis() - smsTick_ < 2000)
		return;

	smsTick_ = millis();

	const int SMS_READ_SIZE = 64;
    char buff[SMS_READ_SIZE];
    int len = 0;

    if (LSMS.available()) // Check if there is new SMS
    {
        LSMS.remoteNumber(buff, 20); // display Number part
        LINKEDHOME_DEBUG_println("New SMS received from:");
        LINKEDHOME_DEBUG_println(buff);

		String str = buff;
		if (str.indexOf(LINKHOME_OWNER_NUM) < 0)
		{
			LINKEDHOME_DEBUG_println("WARNING: Unknown SMS Sender!");
		}

        while(true)
        {
            int ch = LSMS.read();
            if (ch < 0)
				break;

            buff[len++] = (char)ch;
            LINKEDHOME_DEBUG_print((char)ch);
			
			if (len == SMS_READ_SIZE)
				break;
        }

        LINKEDHOME_DEBUG_println();
        LSMS.flush(); // delete message
	
		str = buff;

		//we convert to UPPER case to make the commands case insensitive
		str.toUpperCase();

		//we use indexOf below so that we can have multiple commands
		//	in one SMS, for example:
		//		POWER ON OUTLET OFF MOTION OFF

		if (str.indexOf("POWER ON") >= 0)
			lh_.enablePower(true);

		else if (str.indexOf("POWER OFF") >= 0)
			lh_.enablePower(false);

		if (str.indexOf("OUTLET ON") >= 0)
			lh_.enableSensor(RF_OUTLET, true);

		else if (str.indexOf("OUTLET OFF") >= 0)
			lh_.enableSensor(RF_OUTLET, false);

		if (str.indexOf("MOTION ON") >= 0)
			lh_.enableSensor(LOCAL_PIR, true);

		else if (str.indexOf("MOTION OFF") >= 0)
			lh_.enableSensor(LOCAL_PIR, false);

		if (str.indexOf("GAS ON") >= 0)
			lh_.enableSensor(RF_GAS, true);

		else if (str.indexOf("GAS OFF") >= 0)
			lh_.enableSensor(RF_GAS, false);

		if (str.indexOf("ELEC ON") >= 0)
			lh_.enableSensor(RF_ELEC, true);

		else if (str.indexOf("ELEC OFF") >= 0)
			lh_.enableSensor(RF_ELEC, false);

		if (str.indexOf("SOIL ON") >= 0)
			lh_.enableSensor(RF_SOIL, true);

		else if (str.indexOf("SOIL OFF") >= 0)
			lh_.enableSensor(RF_SOIL, false);

		if (str.indexOf("WIFI ON") >= 0)
			connectWifi();

		if (str.indexOf("STATUS") >= 0)
		{
			String status = lh_.getStatus();
			sendSMS(status.c_str());
		}
	}
#endif
}//gsmLoop()

/////////////////////////////////////////////////////////
//	display/send message functions
/////////////////////////////////////////////////////////

//oledShow() shows messages with timeout
//msg: message to show
//durationSec: duration in sec, 0 = permanently on
void LinkedHomeUI::oledShow(const char *msg, byte durationSec)
{
	LINKEDHOME_DEBUG_println(msg);

	uiTS_[OLED] = durationSec;
	oledTick_ = millis();

	Oled.clearDisplay();        //clear the screen and set start position to top left corner
	Oled.setTextXY(0, 0);       //Set the cursor to Xth Page, Yth Column
	Oled.putString(msg);
	
#ifdef ENABLE_WIFI
	//send the message if we have a live client
	webServer_->response(msg);
#endif
}//oledShow()

bool LinkedHomeUI::sendSMS(const char *msg)
{
	//show on OLED
	oledShow(msg);

#ifdef ENABLE_GSM

	//make sure GSM is ready
	bool gsmReady = LSMS.ready();
	int c = 3;
    while(!gsmReady && c--)
	{
        delay(500);
		gsmReady = LSMS.ready();
	}

	if (!gsmReady)
	{
		oledShow("Error: GSM NOT ready!", 0);
		return false;
	}

	//send SMS
    LINKEDHOME_DEBUG_println("Sending SMS");
	LSMS.beginSMS(LINKHOME_OWNER_NUM);
	LSMS.print(msg);

	if (LSMS.endSMS())
	{
		LINKEDHOME_DEBUG_println("SMS sent");
		return true;
	}

	//show error on OLED if not sent
	oledShow("Error: SMS NOT sent");
#endif
	return false;
}//sendSMS()
