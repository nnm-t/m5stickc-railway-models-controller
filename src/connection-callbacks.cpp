#include "connection-callbacks.hpp"

void ConnectionCallbacks::onConnect(BLEClient* pClient)
{

}

void ConnectionCallbacks::onDisconnect(BLEClient* pClient)
{
	_is_connected = false;
}