/// <reference path="TSDef/p5.global-mode.d.ts" />

"use strict";

// orientation variables:
let heading = 0.0;
let pitch = 0.0;
let roll = 0.0;
// ble variables
let myBLE;
const serviceUUID = "a10a9d6e-9075-11eb-a8b3-0242ac130003"
let valueCharacteristic;
let incoming;
let byteLength = 20;


function setup() {
  createCanvas(500, 600, WEBGL);
  myBLE = new p5ble();
  const connectButton = createButton('Connect')
  connectButton.mousePressed(connectToBle);
}

function connectToBle() {
  // Connect to a device by passing the service UUID
  myBLE.connect(serviceUUID, gotCharacteristics);
}
// A function that will be called once got characteristics
function gotCharacteristics(error, characteristics) {
  if (error) console.log('characteristic error: ', error);
  if (characteristics) {
    valueCharacteristic = characteristics[0];
    console.log(valueCharacteristic);
  //  myBLE.read(valueCharacteristic, 'string', gotValue);
    myBLE.startNotifications(valueCharacteristic, handleValue, 'string');
  } else {
      console.log("characteristic doesn't match.");
  }
}

function handleValue(value){
  var str 	= "";
  if(value) {
  console.log("gotValue: ", typeof(value));
	//	concat and split string for roll, pitch, yaw (e.g. "-0.58,2.20,328.76")
		 for(var i=0; i<byteLength; i++){
       console.log('loop',value.getInt8(i));
		 	str = str + String.fromCharCode(value.getInt8(i));
		 }
		var imu = str.split(',');

		//update globals
		heading	= parseFloat(imu[0]);
		pitch	= parseFloat(imu[1]);
		roll = parseFloat(imu[2]);
    }
  }

function draw() {
    background(255); // set background to white
    push();          // begin object to draw
    // variables for matrix translation:
    let c1 = cos(radians(roll));
    let s1 = sin(radians(roll));
    let c2 = cos(radians(pitch));
    let s2 = sin(radians(pitch));
    let c3 = cos(radians(heading));
    let s3 = sin(radians(heading));
    applyMatrix(c2 * c3, s1 * s3 + c1 * c3 * s2,
      c3 * s1 * s2 - c1 * s3, 0, -s2, c1 * c2,
      c2 * s1, 0, c2 * s3, c1 * s2 * s3 - c3 * s1,
      c1 * c3 + s1 * s2 * s3, 0, 0, 0, 0, 1);

    // draw arduino board:
    drawArduino();
    pop();           // end of object
}
// draws the Arduino Nano:
function drawArduino() {
  // the base board:
  stroke(0, 90, 90); // set outline color to darker teal
  fill(0, 130, 130); // set fill color to lighter teal
  box(300, 10, 120); // draw Arduino board base shape

  // the CPU:
  stroke(0);         // set outline color to black
  fill(80);          // set fill color to dark grey
  translate(30, -6, 0); // move to correct position
  box(60, 0, 60);    // draw box

  // the radio module:
  stroke(80);       // set outline color to grey
  fill(180);        // set fill color to light grey
  translate(80, 0, 0); // move to correct position
  box(60, 15, 60);  // draw box

  // the USB connector:
  translate(-245, 0, 0); // move to correct position
  box(35, 15, 40);   // draw box
}

 
