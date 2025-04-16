#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>


#define LED_PIN 17
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


class MyCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *pCharacteristic) {
   std::string value = pCharacteristic->getValue();


   if (value == "ON") {
     digitalWrite(LED_PIN, LOW);
     Serial.println("LED ON");
   } else if (value == "OFF") {
     digitalWrite(LED_PIN, HIGH);
     Serial.println("LED OFF");
   } else {
     Serial.println("Error. Please write one of the two commands: ON or OFF");
   }
 }
};


void setup() {
 Serial.begin(9600);
 pinMode(LED_PIN, OUTPUT);
 digitalWrite(LED_PIN, HIGH);  // Start with LED OFF


 BLEDevice::init("ESP32_LED_Controller");
 BLEServer *pServer = BLEDevice::createServer();
 BLEService *pService = pServer->createService(SERVICE_UUID);


 BLECharacteristic *pCharacteristic = pService->createCharacteristic(
   CHARACTERISTIC_UUID,
   BLECharacteristic::PROPERTY_WRITE
 );


 pCharacteristic->setCallbacks(new MyCallbacks());
 pService->start();


 BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
 pAdvertising->addServiceUUID(SERVICE_UUID);
 pAdvertising->start();


 Serial.println("BLE LED ready to use. Use 'ON' or 'OFF' commands.");
}


void loop() {
}


