#include <Arduino.h>
#include <BLEDevice.h>

#include <M5Unified.hpp>

#include "advertised-device-callbacks.hpp"
#include "connection-callbacks.hpp"
#include "joystick-hat.hpp"

namespace {
    constexpr const uint32_t color_background = 0xFFFFFFU;
    constexpr const uint32_t color_foreground = 0x000000U;
    constexpr const uint32_t color_red = 0xFF0000U;
    constexpr const uint32_t color_yellow = 0xFFFF00U;
    constexpr const uint32_t color_green = 0x00FF00U;

    constexpr const int32_t duty_max = 255;
    constexpr const int32_t duty_min = -255;

    BLEUUID service_uuid("f68d6127-374a-4b53-a9b7-520e187c0310");
    BLEUUID characteristic_uuid("e95ec096-f32d-4c18-99c5-c11a45492a47");

    BLEScan* pBleScan = nullptr;
    AdvertisedDeviceCallbacks advertised_device_callbacks(&pBleScan, service_uuid);

    BLEClient* pClient = nullptr;
    BLERemoteService* pRemoteService = nullptr;
    BLERemoteCharacteristic* pRemoteCharacteristic = nullptr;

    M5Canvas canvas(&M5.Lcd);

    bool is_connected = false;
    ConnectionCallbacks connection_callbacks(is_connected);

    JoystickHat joystick_hat;
    int32_t duty = 0;
}

void setup()
{
    auto config = M5.config();
    M5.begin(config);

    joystick_hat.begin();
    canvas.createSprite(160, 160);
    canvas.setTextColor(color_foreground, color_background);
    canvas.setFont(&fonts::lgfxJapanGothic_16);

    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(color_background);
    M5.Lcd.setTextColor(color_foreground, color_background);
    M5.Lcd.setFont(&fonts::lgfxJapanGothic_16);

    BLEDevice::init("RailModelsController");
    pBleScan = BLEDevice::getScan();
    pBleScan->setAdvertisedDeviceCallbacks(&advertised_device_callbacks);
    pBleScan->setInterval(1349);
    pBleScan->setWindow(449);
    pBleScan->setActiveScan(true);
    pBleScan->start(5, false);

    M5.Lcd.drawString(BLEDevice::getAddress().toString().c_str(), 0, 0);
    M5.Lcd.drawString("Scanning...", 0, 16);

    delay(2000);
}

void loop()
{
    M5.update();

    if (!is_connected)
    {
        BLEAdvertisedDevice* advertised_device = advertised_device_callbacks.get_advertised_device();
        if (advertised_device != nullptr)
        {
            M5.Lcd.setRotation(3);
            canvas.fillScreen(color_background);
            canvas.drawString(advertised_device->getAddress().toString().c_str(), 0, 0);
            canvas.drawString("RSSI: " + String(advertised_device->getRSSI(), DEC) + "  ", 0, 16);
            canvas.pushSprite(0, 0);

            if (M5.BtnA.wasPressed())
            {
                pClient = BLEDevice::createClient();
                pClient->setClientCallbacks(&connection_callbacks);
                pClient->connect(advertised_device);
                pClient->setMTU(517);

                pRemoteService = pClient->getService(service_uuid);
                if (pRemoteService == nullptr)
                {
                    pClient->disconnect();
                    return;
                }

                pRemoteCharacteristic = pRemoteService->getCharacteristic(characteristic_uuid);
                if (pRemoteCharacteristic == nullptr)
                {
                    pClient->disconnect();
                    return;
                }

                is_connected = true;
            }

            return;
        }

        pBleScan->start(0);
        return;
    }

    if (pClient == nullptr)
    {
        return;
    }

    canvas.fillScreen(color_background);
    canvas.drawString("Connected", 0, 0);

    canvas.fillRect(20, 28, 40, 12, color_red);
    canvas.fillRect(20, 40, 40, 12, color_yellow);
    canvas.fillRect(20, 52, 40, 72, color_green);
    canvas.fillRect(20, 124, 40, 12, color_yellow);
    canvas.fillRect(20, 136, 40, 12, color_red);

    canvas.drawLine(16, 28, 16, 148, color_foreground);
    canvas.drawLine(12, 28, 16, 28, color_foreground);
    canvas.drawLine(12, 40, 16, 40, color_foreground);
    canvas.drawLine(12, 52, 16, 52, color_foreground);
    canvas.drawLine(12, 64, 16, 64, color_foreground);
    canvas.drawLine(12, 76, 16, 76, color_foreground);
    canvas.drawLine(12, 88, 16, 88, color_foreground);
    canvas.drawLine(12, 100, 16, 100, color_foreground);
    canvas.drawLine(12, 112, 16, 112, color_foreground);
    canvas.drawLine(12, 124, 16, 124, color_foreground);
    canvas.drawLine(12, 136, 16, 136, color_foreground);
    canvas.drawLine(12, 148, 16, 148, color_foreground);


    const JoystickPosition position = joystick_hat.read();

    int32_t delta = position.getY() / 25;

    if (position.getIsPressed())
    {
        duty = 0;
        delta = 1;
    }
    else
    {
        duty += delta;
    }

    if (duty > duty_max)
    {
        duty = duty_max;
    }

    if (duty < duty_min)
    {
        duty = duty_min;
    }

    const int32_t y = 148 - 120 * (duty - duty_min) / (duty_max - duty_min);
    canvas.drawLine(12, y, 68, y, color_foreground);

    if (pRemoteCharacteristic != nullptr && delta != 0)
    {
        uint8_t value[2] = { (uint8_t) (duty & 0x00FF), (uint8_t) ((duty & 0xFF00) >> 8) };
        pRemoteCharacteristic->writeValue(value, 2);
    }

    M5.Lcd.setRotation(0);
    canvas.pushSprite(0, 0);
}