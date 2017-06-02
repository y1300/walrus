#include <CurieBLE.h>
#include <CurieIMU.h>

#include "network-arduino.hpp"

BLEPeripheral blePeripheral;
BLEService accelerometerService("20A10010-E8F2-537E-4F6C-D104768A1214");
BLEFloatCharacteristic accelerometerCharacteristic("20A10011-E8F2-537E-4F6C-D104768A1214", BLERead);

bool moving = false;                
bool calibrateOffsets = true;

unsigned long cooldownTime = 750;     //Cooldown period before another switch can happen, in milliseconds
unsigned long lastSwitchTime = 0;     // Time of the last switch in 'moving' state
unsigned long interruptTime = 0;      // Time of the last interrupt
unsigned long readingInterval = 75;  // Time between readings when logging, in milliseconds
const float accelerationMultiplier = 16384; // Value to divide by to get an acceleration in mg

int ax, ay, az; // Accelerometer values
float readingsBuffer[50] = {0.0f}; 
int readingsIndex = 0;
float normalisedReadings[numInputNodes];
Network_A *network;
float latestClassification;

void setup() {
  
  /* Initialise and calibrate the IMU */
  CurieIMU.begin();

  CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 1);
  
  /* Enable Motion Detection */
  CurieIMU.setDetectionThreshold(CURIE_IMU_MOTION, 20); // mg
  CurieIMU.setDetectionDuration(CURIE_IMU_MOTION, 10);  // number of consecutive positive samples required to detect motion
  CurieIMU.interrupts(CURIE_IMU_MOTION);

  /* Enable Zero Motion Detection */
  CurieIMU.setDetectionThreshold(CURIE_IMU_ZERO_MOTION, 35 );  // mg
  CurieIMU.setDetectionDuration(CURIE_IMU_ZERO_MOTION, 0.5);   // seconds
  CurieIMU.interrupts(CURIE_IMU_ZERO_MOTION);
  
  /* Initialise Network */
  network = new Network_A();

  /* Initialise pseudorandom number generator */
  randomSeed(analogRead(0));
  
  /* Initialise the BLE service */
  blePeripheral.setLocalName("ExerciseLogger");
  blePeripheral.setAdvertisedServiceUuid(accelerometerService.uuid());

  blePeripheral.addAttribute(accelerometerService);
  blePeripheral.addAttribute(accelerometerCharacteristic);
  accelerometerCharacteristic.setValue("000000");
  
  blePeripheral.begin();

  /* Initialise the builtn LED to blink and indicate activity */
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  /* While running but not connected, leave LED on */
  digitalWrite(LED_BUILTIN, HIGH);
  BLECentral central = blePeripheral.central();

  while (central.connected()) {
    /* Blink the LED while connected*/
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }

