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

#ifndef __EnergSocket_H__
#define __EnergSocket_H__

//Energinie module connection pins
const int ENERG_D0 = 2;
const int ENERG_D1 = 4;
const int ENERG_D2 = 7;
const int ENERG_D3 = 3;
const int ENERG_MODSEL = 6;
const int ENERG_CE = 5;

// defines which socket we're controlling
enum EnergSocketType
{
	ENERG_SOCKET_NONE = 0,
	ENERG_SOCKET_1 = 1,
	ENERG_SOCKET_2 = 2,
	ENERG_SOCKET_3 = 4,
	ENERG_SOCKET_4 = 8,
	ENERG_SOCKET_ALL = 0xF
};

/////////////////////////////////////////////////////////
//	class EnergSocket
/////////////////////////////////////////////////////////

//class defines Energenie socket
class EnergSocket
{
	//ctor/dtor left empty as we don't have members
public:
	//setup() method
	void setup()
	{
		// Select the GPIO pins used for the encoder K0-K3 data inputs
		pinMode(ENERG_D0, OUTPUT);
		pinMode(ENERG_D1, OUTPUT);
		pinMode(ENERG_D2, OUTPUT);
		pinMode(ENERG_D3, OUTPUT);
		
		// Select the signal used to select ASK/FSK
		pinMode(ENERG_MODSEL, OUTPUT);
		
		// Select the signal used to enable/disable the modulator
		pinMode(ENERG_CE, OUTPUT);
		
		// Disable the modulator by setting CE pin lo
		digitalWrite(ENERG_CE, LOW);
		
		// Set the modulator to ASK for On Off Keying
		// by setting MODSEL pin lo
		digitalWrite(ENERG_MODSEL, LOW);
		
		// Initialise K0-K3 inputs of the encoder to 0000
		digitalWrite(ENERG_D0, LOW);
		digitalWrite(ENERG_D1, LOW);
		digitalWrite(ENERG_D2, LOW);
		digitalWrite(ENERG_D3, LOW);
	}

	//set() turns one (or all) socket(s) on or off
	void set(EnergSocketType socket, bool turnOn)
	{
		switch(socket)
		{
			case ENERG_SOCKET_ALL:
				digitalWrite(ENERG_D0, HIGH);
				digitalWrite(ENERG_D1, HIGH);
				digitalWrite(ENERG_D2, LOW);
				break;

			case ENERG_SOCKET_1:
				digitalWrite(ENERG_D0, HIGH);
				digitalWrite(ENERG_D1, HIGH);
				digitalWrite(ENERG_D2, HIGH);
				break;

			case ENERG_SOCKET_2:
				digitalWrite(ENERG_D0, LOW);
				digitalWrite(ENERG_D1, HIGH);
				digitalWrite(ENERG_D2, HIGH);
				break;

			case ENERG_SOCKET_3:
				digitalWrite(ENERG_D0, HIGH);
				digitalWrite(ENERG_D1, LOW);
				digitalWrite(ENERG_D2, HIGH);
				break;

			case ENERG_SOCKET_4:
				digitalWrite(ENERG_D0, LOW);
				digitalWrite(ENERG_D1, LOW);
				digitalWrite(ENERG_D2, HIGH);
				break;

			default:
				return;	//unknown socket
		}
		
		if (turnOn)
			digitalWrite(ENERG_D3, HIGH);
		else
			digitalWrite(ENERG_D3, LOW);
		
		// let it settle, encoder requires this
		delay(100);
		// Enable the modulator
		digitalWrite(ENERG_CE, HIGH);
		// keep enabled for a period
		delay(250);
		// Disable the modulator
		digitalWrite(ENERG_CE, LOW);
	}
};

//Example: programming the socket:
//EnergSocket energSocket;
// delay(500);
// energSocket.set(ENERG_SOCKET_1, true);
// delay(500);
// energSocket.set(ENERG_SOCKET_1, false);

#endif
