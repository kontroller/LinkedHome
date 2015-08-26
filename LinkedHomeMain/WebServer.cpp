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

#include "WebServer.h"
#include "WebContent.h"

//We'll define WebServer class only if Wifi is enabled
#ifdef ENABLE_WIFI

//setup()
void WebServer::setup()
{
	LTask.begin();
	connectWifi();
}

//connectWifi()
void WebServer::connectWifi()
{
	if (isWifiConnected())
		return;
	
	lh_.ui().oledShow("Connecting to Wifi");
	LWiFi.begin();
	connected_ = (LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)) != 0);

	if (connected_)
	{
		printWifiStatus();
		server_.begin();
		lh_.wifiInfo() = "Wifi: On";
		lh_.ui().oledShow("Wifi connected.");
	}
	else
	{
		//LWiFi.end();
		lh_.wifiInfo() = "Wifi: Off";
		lh_.ui().oledShow("Wifi failed!");
	}
}//connectWifi()

//loop()
void WebServer::loop()
{
	if (!connected_)
		return;

	//if status changed
	connected_ = (LWiFi.status() == LWIFI_STATUS_CONNECTED);
	if (!connected_)
	{
		lh_.wifiInfo() = "Wifi: Off";
		lh_.ui().oledShow("Wifi disconnected!");
		return;
	}

	client_ = server_.available();
	if (client_)
	{
		WEB_SERVER_DEBUG_println("new client");
		hasClient_ = true;
		process();
		hasClient_ = false;
		client_.stop();
	}//if client
}//loop()

//process() if we have a client
void WebServer::process()
{
	int bufindex = 0;

	//read first line of request
	if (client_.connected() && client_.available())
	{
		rxBuffer_[0] = client_.read();
		rxBuffer_[1] = client_.read();
		bufindex = 2;

		//read the first line
		while (rxBuffer_[bufindex - 2] != '\r' && rxBuffer_[bufindex - 1] != '\n')
		{
			rxBuffer_[bufindex] = client_.read();
			bufindex++;

			if (bufindex == WEB_BUFFER_SIZE - 1)
				break;
		}
		
		//ignore rest of the request (including POST data)
		client_.flush();
		
		//parse REST format
		if (parseREST(bufindex))
		{
			servePage();
		}
	}//if client_
}//process()

//parse REST tokens
bool WebServer::parseREST(int len)
{
	if (len < 6)
		return false;

	paramCount_ = 0;
	rxBuffer_[len] = 0;
	String str = rxBuffer_;

	WEB_SERVER_DEBUG_print("Request: ");
	WEB_SERVER_DEBUG_println(rxBuffer_);

	int index_get = str.indexOf("GET ");		//do we have a GET?
	if (index_get == 0)
		index_get += 4;							//strlen("GET ")
	else
	{
		index_get = str.indexOf("POST ");		//do we have a POST?
		if (index_get == 0)
			index_get += 5;						//strlen("POST ")
		else
			index_get = -1;
	}

	if (index_get > 0)
	{
		int index_sp = str.indexOf(' ', index_get); //index of 2nd space
		if (index_sp > index_get)
		{
			page_ = str.substring(index_get, index_sp);
			
			WEB_SERVER_DEBUG_print("Page: ");
			WEB_SERVER_DEBUG_println(page_.c_str());
			
			//extract parameters
			//example: "/page1/a/b/c/d_e/f" is split into:
			//	page1, a, b, c, d_e, f

			int last_index = page_.indexOf('/');
			if (last_index < 0)
				return true;	//only has a pagename, still valid

			last_index++;
			
			do
			{
				index_sp = page_.indexOf("/", last_index);
				if (index_sp > 0)
				{
					params_[paramCount_++] = page_.substring(last_index, index_sp);

					WEB_SERVER_DEBUG_print("param: ");
					WEB_SERVER_DEBUG_println(params_[paramCount_ - 1]);
				}
				else if (last_index < page_.length())
				{
					params_[paramCount_++] = page_.substring(last_index);

					WEB_SERVER_DEBUG_print("param: ");
					WEB_SERVER_DEBUG_println(params_[paramCount_ - 1]);
					break;
				}
				else
					break;
				
				if (paramCount_ == WEB_MAX_PARAMS)
					break;

				last_index = index_sp + 1;
			}while (true);
		}
	}//if index_get >= 0

	return true;
}//parseREST()

//serve the page that was requested
void WebServer::servePage()
{
	int pageId = 0;
	if (paramCount_ > 0)
	{
		for (int ix = 0; ix < WEB_TOTAL_PAGES; ++ix)
		{
			if (params_[0] == WEB_PAGES[ix][0])
			{
				pageId = ix;
				WEB_SERVER_DEBUG_print("pageId: ");
				WEB_SERVER_DEBUG_println(pageId);
				break;
			}
		}//for
		
		//page 404
		if (pageId == 0)
			pageId = -1;
	}//if

	//if static page is not found
	if (pageId == -1)
	{
		//handle "_set" commands
		if (params_[0] == "_set" && paramCount_ >= 3)
		{
			bool enable = (params_[2] == "on");
			
			if (params_[1] == "pw")
				lh_.enablePower(enable);

			else if (params_[1] == "outlet")
				lh_.enableSensor(RF_OUTLET, enable);
				
			else if (params_[1] == "pir")
				lh_.enableSensor(LOCAL_PIR, enable);
				
			else if (params_[1] == "gas")
				lh_.enableSensor(RF_GAS, enable);
				
			else if (params_[1] == "temp")
				lh_.enableSensor(RF_TEMP, enable);
				
			else if (params_[1] == "door")
				lh_.enableSensor(RF_REED_ON, enable);
				
			else if (params_[1] == "elec")
				lh_.enableSensor(RF_ELEC, enable);
				
			else if (params_[1] == "soil")
				lh_.enableSensor(RF_SOIL, enable);
		}
		else
		{
			//unknown command or page name
			WEB_SERVER_DEBUG_println("Sending 404");
			client_.print(WEB_404);
		}
		delay(100);
	}
	else if (pageId == 0)
	{
		//send status
		String status = lh_.getStatus(true);
		client_.print(WEB_HEADER);
		client_.print(status.c_str());
		client_.print(WEB_FOOTER);
		delay(1000);
	}
	else
	{
		//send page
		WEB_SERVER_DEBUG_println("Sending page");
		client_.print(WEB_HEADER);
		client_.print(WEB_PAGES[pageId][1]);	//body
		client_.print(WEB_FOOTER);
		delay(1000);
	}
}//servePage()

//response() is used by enableSensor() to send back response to web client
void WebServer::response(const char *msg)
{
	if (!hasClient_)
		return;

	WEB_SERVER_DEBUG_println("Web reply: ");
	WEB_SERVER_DEBUG_println(msg);

	//NOTE: without the following HTTP response header, Ajax will not work
	int len = strlen(msg);
	client_.print("HTTP/1.1 200 OK\r\nCache-Control: no-store, no-cache, must-revalidate, max-age=0, \r\nPragma: no-cache\r\nContent-Type: text/xml; charset=utf-8\r\n");
	client_.print("Content-Length: ");
	client_.print(len);
	client_.print("\r\n\r\n");
	
	client_.print(msg);
}//response()

//debug method to print Wifi info
void WebServer::printWifiStatus()
{
  WEB_SERVER_DEBUG_print("SSID: ");
  WEB_SERVER_DEBUG_println(LWiFi.SSID());

  WEB_SERVER_DEBUG_print("IP Address: ");
  WEB_SERVER_DEBUG_println(LWiFi.localIP());

  WEB_SERVER_DEBUG_print("subnet mask: ");
  WEB_SERVER_DEBUG_println(LWiFi.subnetMask());

  WEB_SERVER_DEBUG_print("gateway IP: ");
  WEB_SERVER_DEBUG_println(LWiFi.gatewayIP());

  WEB_SERVER_DEBUG_print("signal strength (RSSI):");
  WEB_SERVER_DEBUG_print(LWiFi.RSSI());
  WEB_SERVER_DEBUG_println(" dBm");
}//printWifiStatus()

#endif	//ENABLE_WIFI
