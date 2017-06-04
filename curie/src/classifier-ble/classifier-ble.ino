#include <CurieBLE.h>
#include <CurieIMU.h>

#include "network-arduino.hpp"

BLEPeripheral blePeripheral;
BLEService classifierService("20A10010-E8F2-537E-4F6C-D104768A1214");
BLEFloatCharacteristic classifierCharacteristic("20A10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

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
  CurieIMU.attachInterrupt(eventCallback);

  /* Calibrate the IMU's gyrometer */
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

  /* Serial connection, for debugging */
  Serial.begin(9600); // initialize Serial communication
  while(!Serial) ;    // wait for serial port to connect.
  
  /* Initialise the BLE service */
  blePeripheral.setLocalName("ExerciseClassifieracce");
  blePeripheral.setAdvertisedServiceUuid(classifierService.uuid());

  blePeripheral.addAttribute(classifierService);
  blePeripheral.addAttribute(classifierCharacteristic);
  classifierCharacteristic.setValue(0.0);
  
  blePeripheral.begin();

  /* Initialise the builtn LED to blink and indicate activity */
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  /* While running but not connected, leave LED on */
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println(F("Running"));
  BLECentral central = blePeripheral.central();

  while (central.connected()) {
    /* Blink the LED while connected*/
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    if (moving) {
      if (readingsIndex < 49) {
        CurieIMU.readAccelerometer(ax, ay, az);
        float reading = abs(ax) + abs(ay) + abs(az);
        readingsBuffer[readingsIndex] = reading;
        readingsIndex++;
        delay(readingInterval);
      }
      else {
        // Buffer full; reset
        readingsIndex = 0;
      }
    } 
    else {
      // Wait for movement to start
    }
  }
}

/* If there are too many readings, remove some at random */
void cullReadings(int diff) {
  /* Randomly choose some indexes to remove */
  int indexesToRemove[diff];
  int randomIndex;
  int i = 0;
  bool alreadyInIndexes = false;
  
  while(i < diff) {
    randomIndex = int(random(0, readingsIndex +1));
    for(int j = 0; j < diff; j++) {
      if(indexesToRemove[j] == randomIndex) {
        alreadyInIndexes = true; 
      }
    }
    if (!alreadyInIndexes) {
      indexesToRemove[i] = randomIndex;
      i++;
    }
    alreadyInIndexes = false;
  }
  i = 0;
  bool inIndexes = false;
  
  for (int j = 0; j < readingsIndex; j++) {
    for(int k = 0; k < diff; k++) {
      if(indexesToRemove[k] == j) {
        inIndexes = true; 
      }
    }
    if (!inIndexes) {
      normalisedReadings[i] = readingsBuffer[j] / accelerationMultiplier;
      i++;
    }
    inIndexes = false;
  }
}

/* If there are too few readings, duplicate them at random */
void expandReadings(int diff) {
  /* Randomly choose some indexes to duplicate */
  int indexesToCopy[diff];
  int randomIndex;
  int i = 0;
  bool alreadyInIndexes = false;
  
  while(i < diff) {
    randomIndex = int(random(0, readingsIndex +1));
    for(int j = 0; j < diff; j++) {
      if(indexesToCopy[j] == randomIndex) {
        alreadyInIndexes = true; 
      }
    }
    if (!alreadyInIndexes) {
      indexesToCopy[i] = randomIndex;
      i++;
    }
    alreadyInIndexes = false;
  }
  
  i = 0;
  bool inIndexes = false;

  for (int j = 0; j < readingsIndex; j++) {
    for(int k = 0; k < diff; k++) {
      if(indexesToCopy[k] == j) {
        inIndexes = true; 
      }
    }
    if (inIndexes) {
      normalisedReadings[i] = readingsBuffer[j] / accelerationMultiplier;
      i++;
    }
    normalisedReadings[i] = readingsBuffer[j] / accelerationMultiplier;
    i++;
    inIndexes = false;
  }
}

/* Take the current buffer of readings and normalise it. */
void normaliseReadings(int diff) {
  if (diff == 0) {
    for (int i = 0; i < numInputNodes; i++) {
      normalisedReadings[i] = readingsBuffer[i] / accelerationMultiplier;
    }
  } else if(diff > 0) {
    cullReadings(diff);
  } else if(diff < 0) {
    expandReadings(-1 * diff);
  }
}


/* 
 * Take the current buffer of readings and attempt to classify
 * using the network, sending the result over Serial
 */
void classifyMovement() {  
    int diff = readingsIndex +1 - numInputNodes;
  if (diff > numInputNodes / -2) {
    /* Only attempt to classify if there are greater than nin/2 readings */
    normaliseReadings(diff);
    float *result = network->classify(normalisedReadings);
    Serial.print("Classification is: "); 
    for (int i= 0; i < numOutputNodes; i++) {
      Serial.print(result[i]); Serial.print(" ");
    }
    Serial.println("");
    classifierCharacteristic.setValue(result[0]);
    
  } else {
    Serial.println("Too few readings to normalise, not classifying");
  }
}

static void eventCallback(void){
  interruptTime = millis();
  
  if (CurieIMU.getInterruptStatus(CURIE_IMU_MOTION) && !moving && (interruptTime - lastSwitchTime > cooldownTime)) {
    Serial.print("Motion detected after  ");
    Serial.print(interruptTime - lastSwitchTime);
    Serial.println("  milliseconds. Logging...");
    moving = true;
    lastSwitchTime = interruptTime;
    readingsIndex = 0;
  } 
  
  if (CurieIMU.getInterruptStatus(CURIE_IMU_ZERO_MOTION) && moving && (interruptTime - lastSwitchTime > cooldownTime)) {
    Serial.print("Motion ended after  ");
    Serial.print(interruptTime - lastSwitchTime);
    Serial.println("  milliseconds. Logging...");    
    moving = false;
    lastSwitchTime = interruptTime;
    classifyMovement();
    readingsIndex = 0;
  } 

}
