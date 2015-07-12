//  ---------------------------------------------------------------------------------
//  Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
// 
//  The MIT License (MIT)
// 
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
// 
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
// 
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//  ---------------------------------------------------------------------------------

#include "stdafx.h"
#include <arduino.h>
#include "json.h"
#include "Configuration.h"
#include "amqp\amqp.h"
#include <iostream>


#define CONFIG_FILE_PATH		"GalileoSpsLon.exe.config"
#define REFRESH_DATA_INTERVAL	1000 // milliseconds

amqp::Address	eventHubAddress;
amqp::Sender	amqpSender;
Configuration	config;

int _tmain(int argc, _TCHAR* argv[])
{
	return RunArduinoSketch();
}

void setup()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	try {
		config.load(CONFIG_FILE_PATH);
		eventHubAddress.setAddress(config.host(),
			config.user(),
			config.password(),
			config.eventHubName());

		if (config.amqpMessageTracking()) {
			amqpSender.enableTracking();
		}

		// Initialize the Digital pin 8 as input from the sensor
		pinMode(D8, INPUT);
		// Initialize the Digital pins 2 and 3 as output
		pinMode(D2, OUTPUT); // -- Green led
		pinMode(D3, OUTPUT); // -- Red led

	}
	catch (std::exception &e) {
		Log(e.what());
	}
}

void loop()
{
	
	try {
		
		int hasMovement =  digitalRead(D8);

		std::cout << "Sensor value: " << hasMovement << std::endl;

		digitalWrite(D3, hasMovement ? HIGH : LOW);
		digitalWrite(D2, hasMovement ? LOW : HIGH);

		// Create JSON messsage payload
		JSONObject jsonData;

		jsonData[L"location"] = new JSONValue(config.location());
		jsonData[L"organization"] = new JSONValue(config.organization());
		jsonData[L"guid"] = new JSONValue(config.guidW());
		jsonData[L"displayname"] = new JSONValue(config.deviceNameW());
		jsonData[L"value"] = new JSONValue((bool) hasMovement);
		jsonData[L"subject"] = new JSONValue(config.subjectW());
		jsonData[L"timecreated"] = new JSONValue(config.getTimeNow());

		amqp::JsonMessage telemetryMessage(config.subject(), jsonData, amqp::IMessage::UTF8);

		// The AMQP Prton lib requires the partition-key to be 16 bytes max
		telemetryMessage.addAnnotation(amqp::AMQPSymbol("x-opt-partition-key"), amqp::AMQPuuid(config.guid().substr(0, 16)));

		// Send AMQP message to Azure Event Hub
		amqpSender.send(telemetryMessage, eventHubAddress);

		Sleep(REFRESH_DATA_INTERVAL);
	}
	catch (std::exception &e) 
	{
		Log(e.what());
	}
}