#include <CurieBLE.h>
#include <CurieIMU.h>

BLEPeripheral blePeripheral;
BLEService accelerometerService("20A10010-E8F2-537E-4F6C-D104768A1214");
BLECharacteristic accelerometerCharacteristic("20A10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 6);
unsigned long readingInterval = 75;  // Time between readings when logging

int ax, ay, az; // Accelerometer values

char accelerometerByteArray[6];

void setup() {
  
  /* Initialise and calibrate the IMU */
  CurieIMU.begin();

  CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 1);

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
    
    CurieIMU.readAccelerometer(ax, ay, az);
    accelerometerByteArray[0] = lowByte(ax);
    accelerometerByteArray[1] = highByte(ax);
    accelerometerByteArray[2] = lowByte(ay);
    accelerometerByteArray[3] = highByte(ay);
    accelerometerByteArray[4] = lowByte(az);
    accelerometerByteArray[5] = highByte(az);
    
    accelerometerCharacteristic.setValue(accelerometerByteArray);
    delay(readingInterval);
  }
}

