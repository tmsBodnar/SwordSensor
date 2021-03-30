#include "Arduino_LSM9DS1.h"
#include "MadgwickAHRS.h"
#include "ArduinoBLE.h"
 
// initialize a Madgwick filter:
Madgwick filter;
// sensor's sample rate is fixed at 104 Hz:
const float sensorRate = 104.00;
 
// values for orientation:
float roll = 0.0;
float pitch = 0.0;
float heading = 0.0;

// bluetooth values
BLEService swordSensorService("a10a9d6e-9075-11eb-a8b3-0242ac130003");
BLEFloatCharacteristic headingValue("2101", BLERead | BLENotify);
BLEFloatCharacteristic pitchValue("2102", BLERead | BLENotify);
BLEFloatCharacteristic rollValue("2103", BLERead | BLENotify);

void setup() {
  Serial.begin(9600);
  // attempt to start the IMU:
  if (!IMU.begin() || !BLE.begin()) {
    if (!IMU.begin()) {
      Serial.println("Failed to initialize IMU");
    }
    if (!BLE.begin()) {
      Serial.println("Failed start ble");
    }
    // stop here if you can't access the IMU:
    while (true);
  }
  // start the filter to run at the sample rate:
  filter.begin(sensorRate);

  //start bluetooth
  BLE.setLocalName("SwordSensor");
  BLE.setAdvertisedService(swordSensorService);
  swordSensorService.addCharacteristic(headingValue);
  swordSensorService.addCharacteristic(pitchValue);
  swordSensorService.addCharacteristic(rollValue);
  BLE.addService(swordSensorService);

  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}
 
void loop() {
  // values for acceleration and rotation:
  float xAcc, yAcc, zAcc;
  float xGyro, yGyro, zGyro;
  BLE.poll();
//  BLEDevice central = BLE.central();
//  if ( central) {
//    while (central.connected()){
      // check if the IMU is ready to read:
      if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
       // read accelerometer and gyrometer:
        IMU.readAcceleration(xAcc, yAcc, zAcc);
        IMU.readGyroscope(xGyro, yGyro, zGyro);
       
        // update the filter, which computes orientation:
        filter.updateIMU(xGyro, yGyro, zGyro, xAcc, yAcc, zAcc);
       
        // print the heading, pitch and roll
        roll = filter.getRoll();
        pitch = filter.getPitch();
        heading = filter.getYaw();
        // print the filter's results:
        Serial.print(heading);
        Serial.print(",");
        Serial.print(pitch);
        Serial.print(",");
        Serial.println(roll);
        headingValue.writeValue(heading);
        pitchValue.writeValue(pitch);
        rollValue.writeValue(roll);
        delay(50);
      }
//    }
//  }
}
