#pragma once

#include <Arduino.h>
#include <BLEDevice.h>

class AdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
	BLEScan** _pBleScan;
	BLEUUID& _service_uuid;

	BLEAdvertisedDevice* _advertised_device = nullptr;

public:
	AdvertisedDeviceCallbacks(BLEScan** pBleScan, BLEUUID& service_uuid) : _pBleScan(pBleScan), _service_uuid(service_uuid)
	{
	}

	void onResult(BLEAdvertisedDevice advertisedDevice) override;

	BLEAdvertisedDevice* get_advertised_device();
};