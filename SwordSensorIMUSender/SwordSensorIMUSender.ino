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

// values to proper delay
unsigned long microsPerReading, microsPrevious;

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

  microsPerReading = 1000000 / 1;
  microsPrevious = micros();
}
 
void loop() {
  float xAcc, yAcc, zAcc;
  float xGyro, yGyro, zGyro;
  unsigned long microsNow;
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

    
    microsNow = micros();
    if(microsNow - microsPrevious >= microsPerReading){

      BLEDevice central = BLE.central();
        if(central){ // if a central is connected to peripheral
            const unsigned char imuCharArray[20] = {
                str[0],str[1],str[2],str[3],str[4],
                str[5],str[6],str[7],str[8],str[9],
                str[10],str[11],str[12],str[13],str[14],
                str[15],str[16],str[17],str[18],str[19]
            };
            values.writeValue(str);
        }
     
    }
    microsPrevious = microsPrevious + microsPerReading;
  }
}
