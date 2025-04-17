#include <Wire.h>
#include <SparkFunLSM6DSO.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

LSM6DSO myIMU;
BLECharacteristic *pCharacteristic;

float baselineZ = 0;
int stepCount = 0;
bool stepDetected = false;
float threshold = 0.5;

void calibrateSensor() {
  Serial.println("Calibrating...");
  float sumZ = 0;
  const int samples = 50;
  
  for (int i = 0; i < samples; i++) {
    sumZ += myIMU.readFloatAccelZ();
    delay(20);
  }

  baselineZ = sumZ / samples;
  Serial.print("Baseline Z: ");
  Serial.println(baselineZ, 4);
}

void setupBLE() {
  BLEDevice::init("ESP32_StepCounter");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();
  Serial.println("BLE advertising as 'ESP32_StepCounter'");
}

void setup() {
  Serial.begin(9600);
  delay(500);

  Wire.begin(21, 22);

  if (!myIMU.begin()) {
    Serial.println("Could not connect to IMU.");
    while (1);
  }

  if (myIMU.initialize(BASIC_SETTINGS)) {
    Serial.println("LSM6DSO ready. Settings loaded.");
  }

  setupBLE();
  calibrateSensor();
}

void loop() {
  float z = myIMU.readFloatAccelZ();
  float accZ = z - baselineZ;

  if (abs(accZ) > threshold && !stepDetected) {
    stepCount++;
    stepDetected = true;

    Serial.print("Step Count: ");
    Serial.println(stepCount);

    String countStr = String(stepCount);
    pCharacteristic->setValue(countStr.c_str());
    pCharacteristic->notify();
  }

  if (abs(accZ) < 0.3) {
    stepDetected = false;
  }

  delay(20);
}
