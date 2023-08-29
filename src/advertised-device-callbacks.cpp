#include "advertised-device-callbacks.hpp"

void AdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice)
{
	if (!advertisedDevice.haveServiceUUID())
	{
		return;
	}

	if (!advertisedDevice.isAdvertisingService(_service_uuid))
	{
		return;
	}

	(*_pBleScan)->stop();

	delete _advertised_device;
	_advertised_device = new BLEAdvertisedDevice(advertisedDevice);
}

BLEAdvertisedDevice* AdvertisedDeviceCallbacks::get_advertised_device()
{
	return _advertised_device;
}