#include "Arduino_LSM9DS1.h"
#include "MadgwickAHRS.h"
#include "ArduinoBLE.h"
 
// initialize a Madgwick filter:
Madgwick filter;
// sensor's sample rate is fixed at 104 Hz:
const float sensorRate = 104.00;
 
// values for orientation:
float rollValue = 0.0;
float pitchValue = 0.0;
float headingValue = 0.0;

// bluetooth values
BLEService swordSensorService("a10a9d6e-9075-11eb-a8b3-0242ac130003");
BLEFloatCharacteristic heading("2101", BLERead | BLENotify);
BLEFloatCharacteristic pitch("2102", BLERead | BLENotify);
BLEFloatCharacteristic roll("2103", BLERead | BLENotify);

void setup() {
  Serial.begin(9600);
  if (!IMU.begin() || !BLE.begin()) {
    if (!IMU.begin()) {
      Serial.println("Failed to initialize IMU");
    }
    if (!BLE.begin()) {
      Serial.println("Failed start ble");
    }
    while (true);
  }
  filter.begin(sensorRate);

  //start bluetooth
  BLE.setLocalName("SwordSensor");
  BLE.setAdvertisedService(swordSensorService);
  swordSensorService.addCharacteristic(heading);
  swordSensorService.addCharacteristic(pitch);
  swordSensorService.addCharacteristic(roll);
  BLE.addService(swordSensorService);

  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}
 
void loop() {
  float xAcc, yAcc, zAcc;
  float xGyro, yGyro, zGyro;
  BLE.poll();
      
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    // read accelerometer and gyrometer:
    IMU.readAcceleration(xAcc, yAcc, zAcc);
    IMU.readGyroscope(xGyro, yGyro, zGyro);
       
    // update the filter, which computes orientation:
    filter.updateIMU(xGyro, yGyro, zGyro, xAcc, yAcc, zAcc);
       
    // print the heading, pitch and roll
    rollValue = filter.getRoll();
    pitchValue = filter.getPitch();
    headingValue = filter.getYaw();

    Serial.print("heading: ");
    Serial.print(headingValue);
    Serial.print(" pitch: ");
    Serial.print(pitchValue);
    Serial.print(" roll: ");
    Serial.println(rollValue);
    
    heading.writeValue(headingValue);
    pitch.writeValue(pitchValue);
    roll.writeValue(rollValue);
    delay(50);
      }
}
