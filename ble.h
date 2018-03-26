/*
   ESP32 FastLED BLE: https://github.com/jasoncoon/esp32-fastled-ble
   Copyright (C) 2018 Jason Coon

   Built upon the amazing FastLED work of Daniel Garcia and Mark Kriegsman:
   https://github.com/FastLED/FastLED

   ESP32 support provided by the hard work of Sam Guyer:
   https://github.com/samguyer/FastLED

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
//#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
//#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLECharacteristic *pCharacteristic;

typedef struct {
  char power[2];
  char pattern[16];
} blefields_a;
blefields_a blefields;


class CharacteristicCallback5 : public BLECharacteristicCallbacks { // path
  void onWrite(BLECharacteristic* pChar){
    sprintf(blefields.power, "%s", pChar->getValue().c_str());
  }
};


class FieldCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
      String uuid2;
      if (rxValue.length() > 0) {
        std::string uuid = pCharacteristic->getUUID().toString();
        Serial.print(uuid.c_str());

        Serial.println("*********");
        Serial.print("Received write for characteristic UUID: ");
        for (int i = 0; i < uuid.length(); i++) {
          Serial.print(uuid[i]);
          uuid2+=uuid[i];
        }
        Serial.println();
        Serial.print("Value: ");

        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
        }

        Serial.println();
//      if (uuid)
        // Do stuff based on the command received from the app
//        if(uuid=="
        if (rxValue.find("push") != -1) {
          Serial.println("Turning ON!");
             Serial.print(String(currentPatternIndex));
             currentPatternIndex++;
        }
        if (rxValue.find("1") != -1) {
          Serial.println("TurningON!");
          power = 1;
        }
        else if (rxValue.find("0") != -1) {
          Serial.println("Turning OFF!");
          power = 0;
        }

        Serial.println();
        Serial.println("*********");
      }
    }
};


void setupBLE() {
  Serial.println("Starting BLE...");
// Create the BLE Device
  BLEDevice::init("Lumos Board"); // Give it a name

  // Create the BLE Server
    BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLEService* pService2 = pServer->createService("a8c27e11-bf97-40a5-8438-3631bb75a7e");
 FieldCallbacks *fieldCallbacks = new FieldCallbacks();

for (uint8_t i = 0; i < fieldCount; i++) {
    Field field = fields[i];

    char uuid[field.uuid.length() + 1];
    memset(uuid, 0, field.uuid.length() + 1);

    for (uint8_t i = 0; i < field.uuid.length(); i++)
      uuid[i] = field.uuid.charAt(i);

    String value = field.getValue();

    Serial.print("Adding characteristic UUID: ");
    Serial.print(uuid);
    Serial.print(", name: ");
    Serial.print(field.name);
    Serial.print(", value: ");
    Serial.println(value);


BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                           SERVICE_UUID,
                                           BLECharacteristic::PROPERTY_READ |
                                           BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->addDescriptor(new BLE2902());

    pCharacteristic->setCallbacks(fieldCallbacks);

    pCharacteristic->setValue(value.c_str());
  }
BLECharacteristic* pCharacteristic5 = pService2->createCharacteristic(      // power
      BLEUUID("a8c27e11-bf97-40a5-8438-3631bb75a7e"),
      BLECharacteristic::PROPERTY_READ  |
      BLECharacteristic::PROPERTY_WRITE
    );
    
  pCharacteristic->addDescriptor(new BLE2902());

pCharacteristic5->setValue(blefields.power);
   // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}
