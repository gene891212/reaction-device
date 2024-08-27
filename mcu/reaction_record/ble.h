/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define DEVICE_NAME "Reaction Device"
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define REACTION_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define MAX3010X_UUID "d2912856-de63-11ed-b5ea-0242ac120002"
#define SRARTGAME_UUID "d2912856-de63-11ed-b5ea-0242ac120001"

BLEServer* pServer;
BLEService* pService;
BLECharacteristic* pReactionCh;
BLECharacteristic* pMax3010xCh;
BLECharacteristic* pStartgameCh;
bool deviceConnected = false;


class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};


void setupBLE() {
  BLEDevice::init(DEVICE_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  pService = pServer->createService(SERVICE_UUID);

  pReactionCh = pService->createCharacteristic(
    REACTION_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
  pReactionCh->addDescriptor(new BLE2902());
  pMax3010xCh = pService->createCharacteristic(
    MAX3010X_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
  pMax3010xCh->addDescriptor(new BLE2902());
  pStartgameCh = pService->createCharacteristic(
    SRARTGAME_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
  pStartgameCh->addDescriptor(new BLE2902());
  pService->start();

  // Start advertising
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  // Serial.println("Characteristic defined! Now you can read it in your phone!");
}
