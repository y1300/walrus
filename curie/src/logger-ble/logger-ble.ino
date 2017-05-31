#include <CurieBLE.h>
#include <CurieIMU.h>

BLEPeripheral blePeripheral;
BLEService accelerometerService("19B10010-E8F2-537E-4F6C-D104768A1214");
BLEFloatCharacteristic axCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead);
BLEFloatCharacteristic ayCharacteristic("19B10100-E8F2-537E-4F6C-D104768A1214", BLERead);
BLEFloatCharacteristic azCharacteristic("19B10101-E8F2-537E-4F6C-D104768A1214", BLERead);

unsigned long readingInterval = 75;  // Time between readings when logging
int ax, ay, az; // Accelerometer values

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
  blePeripheral.addAttribute(axCharacteristic);
  blePeripheral.addAttribute(ayCharacteristic);
  blePeripheral.addAttribute(azCharacteristic);

  axCharacteristic.setValue(0.0);
  ayCharacteristic.setValue(0.0);
  azCharacteristic.setValue(0.0);
  
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
    axCharacteristic.setValue(ax);
    ayCharacteristic.setValue(ay);
    azCharacteristic.setValue(az);
    delay(readingInterval);
  }
}

