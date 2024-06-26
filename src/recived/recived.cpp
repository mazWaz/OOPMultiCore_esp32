#include "recived.h"

#include <iostream>

recived::recived() {}
void recived::loraconnect() {
    LoRa.setPins(ss, rst, dio0);
    if (!LoRa.begin(433E6)) {
        while (true) {
            Serial.println("Starting LoRa failed!");
        }
    }
}

String recived::getValue(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void recived::onRecive(uint64_t numId) {
    mqttx mqttx;
    packetSize = LoRa.parsePacket();
    if (packetSize == 0) return;
    memset(cadena, 0,
           sizeof(cadena));  // memset deletes the contents of the array
                             // "cadena" from position 0 to the end sizeof
    // read packet header bytes:
    int recipient = LoRa.read();       // recipient address
    byte sender = LoRa.read();         // sender address
    byte incomingMsgId = LoRa.read();  // incoming msg ID
    // byte incomingLength = LoRa.read();  // incoming msg length
    // read packet
    // received a packet
    // Serial.print("Received packet... ");
    while (LoRa.available()) {
        char dedos = (char)LoRa.read();
        Serial.print(dedos);
        cadena[posicion] = dedos;  // Read a character from the string "cadena"
                                   // from "posicion", then read the next
                                   // character with "posicion++"
        posicion++;
    }
    posicion = 0;
    String mkrwan = String(sender, HEX);
    String esp32 = String(recipient, HEX);
    Serial.println("Received from: 0x" + mkrwan);
    Serial.println("Sent to: 0x" + esp32);
    Serial.println("Message ID: " + String(incomingMsgId));
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));

    temp = getValue(cadena, ',', 1);
    latitude = getValue(cadena, ',', 2);
    longitude = getValue(cadena, ',', 3);
    speed = getValue(cadena, ',', 4);
    voltage = getValue(cadena, ',', 5);
    current = getValue(cadena, ',', 6);

    if (recipient == localAddress && sender == destination) {
        if (millis() - sending >= 1000) {
            sending = millis();
            mqttx.sendtemp(numId, temp);
            mqttx.sendlong(numId, longitude);
            mqttx.sendlat(numId, latitude);
            mqttx.sendspeed(numId, speed);
            mqttx.sendVolatge(numId, voltage);
            mqttx.sendCurrent(numId, current);
        }
    } else {
        Serial.println("tidak ada data");
        // return;
    }
}