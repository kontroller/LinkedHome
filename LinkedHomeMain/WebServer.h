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

#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__

#include "LinkedHome.h"

//We'll define WebServer class only if Wifi is enabled
#ifdef ENABLE_WIFI

#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiServer.h>
#include <LWiFiClient.h>
#include <string.h>

#ifdef WEB_SERVER_DEBUG
	#define WEB_SERVER_DEBUG_print Serial.print
	#define WEB_SERVER_DEBUG_println Serial.println
#else
	#define WEB_SERVER_DEBUG_print //
	#define WEB_SERVER_DEBUG_println //
#endif

#define WEB_BUFFER_SIZE 32
#define WEB_MAX_PARAMS 5

/////////////////////////////////////////////////////////
//	class WebServer
/////////////////////////////////////////////////////////

//Web Server class which hosts the web site for 
//	the control and configuration of LinkedHome
class WebServer
{
public:
	char rxBuffer_[WEB_BUFFER_SIZE];	//Rx buffer for the web server
	String page_;
	String params_[WEB_MAX_PARAMS];		//REST tokens kept in this
	int paramCount_;					//number of REST tokens

	LWiFiServer server_;				//TCP server
	LWiFiClient client_;				//TCP client

	bool connected_;					//flag for wifi connection
	bool hasClient_;					//flag used for web response

	LinkedHome &lh_;					//reference to LinkedHome
										//NOTE: this must be a pointer or reference

public:
	WebServer(LinkedHome &lh, int port):
		lh_(lh), paramCount_(0), server_(port),
		connected_(false), hasClient_(false)
	{
	}

	void setup();
	void loop();

	bool isWifiConnected()
	{
		connected_ = (LWiFi.status() == LWIFI_STATUS_CONNECTED);
		return connected_;
	}

	void connectWifi();
	void response(const char *msg);

private:	
	void process();
	bool parseREST(int len);
	void servePage();
	void printWifiStatus();
};

#endif	//ENABLE_WIFI
#endif	//__WEB_SERVER_H__
