#pragma once

#include <Arduino.h>
#include <BLEDevice.h>

class ConnectionCallbacks : public BLEClientCallbacks
{
	bool& _is_connected;
public:

	ConnectionCallbacks(bool& is_connected) : _is_connected(is_connected)
	{

	}

	void onConnect(BLEClient* pClient) override;

	void onDisconnect(BLEClient* pClient) override;

};