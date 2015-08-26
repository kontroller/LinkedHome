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

#ifndef __WEB_CONTENT_H__
#define __WEB_CONTENT_H__

/////////////////////////////////////////////////////////
//	Web Contents
//	This page contains the HTML codes for web server
//		and will look a little bit messy
/////////////////////////////////////////////////////////

// HTTP Request message
const char *WEB_404 = "HTTP/1.1 404 Not Found\nServer: arduino\nContent-Type: text/html\n\n<html><head><title>Arduino Web Server - Error 404</title></head><body><h1>Error 404: Sorry, that page cannot be found!</h1></body>";

// HTML Header for pages
const char *WEB_HEADER = "<html><head><title>LinkedHome</title>\
<meta name=\"viewport\" content=\"width=device-width, user-scalable=no\">\
<script>\
var min_jx=function(url){\
var xh=window.XMLHttpRequest?new XMLHttpRequest():new ActiveXObject('MicrosoftXMLHTTP');\
document.getElementById('msg').innerHTML = url;\
xh.onreadystatechange = function(){\
if (this.readyState == 4 && this.status == 200)\
document.getElementById('msg').innerHTML = this.responseText;};\
xh.open('GET', url, true);xh.send();\
}\
</script></head><body><div>\
<center><h3>LinkedHome</h3><a href=\"/\">Status</a> \
<a href=\"/control\">Control</a><br/><br/> \
<a href=\"/sensors\">Sensors</a><br/><br/> \
<div id=\"msg\"></div></center><br/>";

const char *WEB_FOOTER = "</div></body></html>";

#define WEB_TOTAL_PAGES 3

/////////////////////////////////////////////////////////
//	Static pages
/////////////////////////////////////////////////////////

//following defines page name and body
//	we have 3 static pages below, "/" (index), "control" and "sensors"
//	WARNING: do not create a page below with name "_set"
//
const char *WEB_PAGES[WEB_TOTAL_PAGES][2] =
{
	//index page
	{
		"/",
		"<h3>LinkedHome Status</h3>"
	},

	//LinkedHome and outlet control
	{
		"control",
"<h3>LinkedHome Control</h3>\
Power: \
<button type=\"button\" onclick=\"min_jx('_set/pw/on')\">LinkedHome On</button> \
<button type=\"button\" onclick=\"min_jx('_set/pw/off')\">LinkedHome Off</button><br/><br/>"
"Outlet: \
<button type=\"button\" onclick=\"min_jx('_set/outlet/on')\">Outlet On</button> \
<button type=\"button\" onclick=\"min_jx('_set/outlet/off')\">Outlet Off</button><br/><br/>"
	},

	//configuration page
	{
		"sensors",
"<h3>Sensor configuration</h3>\
Motion Sensor: \
<button type=\"button\" onclick=\"min_jx('_set/pir/on')\">On</button> \
<button type=\"button\" onclick=\"min_jx('_set/pir/off')\">Off</button><br/><br/>\
Door Alarm: \
<button type=\"button\" onclick=\"min_jx('_set/door/on')\">On</button> \
<button type=\"button\" onclick=\"min_jx('_set/door/off')\">Off</button><br/><br/>\
Temperature Sensor: \
<button type=\"button\" onclick=\"min_jx('_set/temp/on')\">On</button> \
<button type=\"button\" onclick=\"min_jx('_set/temp/off')\">Off</button><br/><br/>\
Gas/Smoke Sensor: \
<button type=\"button\" onclick=\"min_jx('_set/gas/on')\">On</button> \
<button type=\"button\" onclick=\"min_jx('_set/gas/off')\">Off</button><br/><br/>\
Electricity Monitor: \
<button type=\"button\" onclick=\"min_jx('_set/elec/on')\">On</button> \
<button type=\"button\" onclick=\"min_jx('_set/elec/off')\">Off</button><br/><br/>\
Soil Monitor: \
<button type=\"button\" onclick=\"min_jx('_set/soil/on')\">On</button> \
<button type=\"button\" onclick=\"min_jx('_set/soil/off')\">Off</button><br/><br/>"
	},
};

#endif
