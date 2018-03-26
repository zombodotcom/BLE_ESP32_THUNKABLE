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




/* 
 Adding characteristic UUID: a8c27e11-bf97-40a5-8438-3631bb75a7e1, name: power, value: 1
Adding characteristic UUID: 672e2d71-af3c-407a-a7dc-c9b7beb229f2, name: brightness, value: 8
Adding characteristic UUID: 7ac75fb4-33ef-45e1-b63e-e399b1c2accb, name: speed, value: 30
Adding characteristic UUID: 75c34563-a7ba-4ff3-8d9f-832fa1cd5a0e, name: pattern, value: 0
Adding characteristic UUID: 55aae8b1-2958-4ff4-bfef-38949fe3a38d, name: autoplay, value: 1
Adding characteristic UUID: 3d78f086-ed5d-4d72-871b-874ba8b84e35, name: autoplayDuration, value: 10
Adding characteristic UUID: 4ea8ae43-f99c-46b9-aec4-749673fdb835, name: palette, value: 0
Adding characteristic UUID: b5d64d7f-a5d1-4f61-bf1c-89152fcc17d3, name: cyclePalettes, value: 0
Adding characteristic UUID: 8b291198-8141-49db-baa1-620cfdf7ca49, name: paletteDuration, value: 10
Adding characteristic UUID: fd8fd729-d189-413a-8cf3-53395931cae4, name: solidColor, value: 0,0,255
Adding characteristic UUID: 77f52711-c619-4b4d-a180-8b18e812a15c, name: cooling, value: 50
Adding characteristic UUID: bb2dbf79-081c-4895-9422-2e6638aeb912, name: sparking, value: 120
Adding characteristic UUID: f1923e5f-a78f-4418-95ce-ee3f4058069b, name: twinkleSpeed, value: 4
Adding characteristic UUID: 018abf88-a753-4d57-b920-24c39042149d, name: twinkleDensity, value: 5

 */

 class CharacteristicCallback1 : public BLECharacteristicCallbacks { // pattern
 void onWrite(BLECharacteristic* pChar){
    sprintf(blefields.pattern, "%s", pChar->getValue().c_str());
  }
};
 class CharacteristicCallback : public BLECharacteristicCallbacks { // power
 void onWrite(BLECharacteristic* pChar){
    sprintf(blefields.power, "%s", pChar->getValue().c_str());
  }
};

class FieldCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        std::string uuid = pCharacteristic->getUUID().toString();
        Serial.print(uuid.c_str());

        Serial.println("*********");
        Serial.print("Received write for characteristic UUID: ");
        for (int i = 0; i < uuid.length(); i++) {
          Serial.print(uuid[i]);
        }
        Serial.println();
        Serial.print("Value: ");

        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
        }

        Serial.println();

        // Do stuff based on the command received from the app
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
    BLEDevice::init("lumos board");
    
    BLEServer* pServer = BLEDevice::createServer();
    
    BLEService* pService = pServer->createService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");

  pCharacteristic->addDescriptor(new BLE2902());
    BLECharacteristic* pCharacteristic = pService->createCharacteristic(      // power
      BLEUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E"),
      BLECharacteristic::PROPERTY_READ  |
      BLECharacteristic::PROPERTY_WRITE
    );

//    pCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    pCharacteristic->setCallbacks(new CharacteristicCallback1());

  pCharacteristic->addDescriptor(new BLE2902());
    BLECharacteristic* pCharacteristic1 = pService->createCharacteristic(     //pattern
      BLEUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E"),
      BLECharacteristic::PROPERTY_READ  |
      BLECharacteristic::PROPERTY_WRITE
    );




    pService->start();

      pCharacteristic->setValue(blefields.power);
      pCharacteristic1->setValue(blefields.pattern);



    BLEAdvertising* pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
/*
    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
    pSecurity->setCapability(ESP_IO_CAP_NONE);
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
*/
    ESP_LOGD(LOG_TAG, "Advertising started!");
    delay(portMAX_DELAY);
  }

