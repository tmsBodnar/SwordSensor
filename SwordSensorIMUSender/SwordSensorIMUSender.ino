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
BLEStringCharacteristic values("2d1946ee-93ae-11eb-a8b3-0242ac130003", BLERead | BLENotify, 20);

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
  swordSensorService.addCharacteristic(values);
  BLE.addService(swordSensorService);

  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}
 
void loop() {
  float xAcc, yAcc, zAcc;
  float xGyro, yGyro, zGyro;
  String str = "";
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

    str = str + headingValue + "," + pitchValue + "," + rollValue;
    
    BLEDevice central = BLE.central();
      if (central) { // if a central is connected to peripheral
          values.writeValue(str);
          Serial.println(str);
      }
  }
}
