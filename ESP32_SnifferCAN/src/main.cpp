#include <Arduino.h>
#include <SPI.h>
#include <mcp_can.h>

// Variaveis MCP2515
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128]; // Array to store serial string
String msg = "";

//Setup de pino
#define canLed 2 // Set LED to pin 2
#define CAN0_INT 36 // Set INT to pin 36
MCP_CAN CAN0(5);    // Set CS to pin 5

// Variaveis extraidas da FT
unsigned long oilPressure, fuelPressure, waterPressure, gear;
unsigned long tps, mapFT, airTemp, engineTemp;
unsigned long exhaustO2, rpm, oilTemp, pitLimit;

void setup(){
    Serial.begin(115200);
    pinMode(canLed, OUTPUT);
    Serial.println("Sniffer CAN FT 2.0 - Apuama Racing 2022");
    delay(500);
    // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
    if (CAN0.begin(MCP_ANY, CAN_1000KBPS, MCP_8MHZ) == CAN_OK) //
        Serial.println("MCP2515 Inicializado com sucesso!");
    else
        Serial.println("Error ao Inicializar MCP2515...");

    delay(500);
    CAN0.setMode(MCP_NORMAL); // Set operation mode to normal so the MCP2515 sends acks to received data.

    pinMode(CAN0_INT, INPUT); // Configuring pin for /INT input
}

void loop() {
    digitalWrite(canLed, LOW); // Turn LED low
    if (!digitalRead(CAN0_INT)) {            // If CAN0_INT pin is low, read receive buffer
        CAN0.readMsgBuf(&rxId, &len, rxBuf); // Read data: len = data length, buf = data byte(s)

        if ((rxId & 0x80000000) == 0x80000000) { // Determine if ID is standard (11 bits) or extended (29 bits)
            sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
            digitalWrite(canLed, HIGH); // Turn LED high
            // Serial.println(rxId);
            if (rxId == 2483553792) {                           // 600
                tps = ((rxBuf[0] << 8) | rxBuf[1]) / 10;        // Convert to unsigned long
                mapFT = ((rxBuf[2] << 8) | rxBuf[3]) / 1000;    // Convert to unsigned long
                airTemp = ((rxBuf[4] << 8) | rxBuf[5]) / 10;    // Convert to unsigned long
                engineTemp = ((rxBuf[6] << 8) | rxBuf[7]) / 10; // Convert to unsigned long
                // Serial.println(tps);
                // Serial.println(mapFT);
                // Serial.println(airTemp);
                // Serial.println(engineTemp);
                msg = "{\"TPS\":" + String(tps) + "\"MAP:\"" + String(mapFT) + "\"AirTemp:\"" + String(airTemp) + "\"EngineTemp:\"" + String(engineTemp) + "}";
            }
            if (rxId == 2483553793) { // 601
                // Serial.println("Broadcast message");
                // sprintf(msgString,"oilPressure: 0x%.2X%.2X", rxBuf[0], rxBuf[1]);
                // Serial.println(msgString);

                oilPressure = ((rxBuf[0] << 8) | rxBuf[1]) / 1000;   // Convert to unsigned long
                fuelPressure = ((rxBuf[2] << 8) | rxBuf[3]) / 1000;  // Convert to unsigned long
                waterPressure = ((rxBuf[4] << 8) | rxBuf[5]) / 1000; // Convert to unsigned long
                gear = ((rxBuf[6] << 8) | rxBuf[7]);                 // Convert to unsigned long
                // Serial.println(oilPressure);
                // Serial.println(fuelPressure);
                // Serial.println(waterPressure);
                // Serial.println(gear);
                msg = "{\"OilPressure\":" + String(oilPressure) + "\"FuelPressure:\"" + String(fuelPressure) + "\"WaterPressure:\"" + String(waterPressure) + "\"Gear:\"" + String(gear) + "}";
                // for (byte i = 0; i < len; i++) {
                //   sprintf(msgString, " 0x%.2X", rxBuf[i]);
                //   Serial.print(msgString);
                // }
            }
            if (rxId == 2483553794) {                            // 602
                exhaustO2 = ((rxBuf[0] << 8) | rxBuf[1]) / 1000; // Convert to unsigned long
                rpm = ((rxBuf[2] << 8) | rxBuf[3]);              // Convert to unsigned long
                oilTemp = ((rxBuf[4] << 8) | rxBuf[5]) / 1000;   // Convert to unsigned long
                pitLimit = ((rxBuf[6] << 8) | rxBuf[7]);         // Convert to unsigned long
                // Serial.println(exhaustO2);
                // Serial.println(oilTemp);
                // Serial.println(airTemp);
                // Serial.println(pitLimit);
                msg = "{\"ExhaustO2\":" + String(exhaustO2) + "\"RPM:\"" + String(rpm) + "\"OilTemp:\"" + String(oilTemp) + "\"PitLimit:\"" + String(pitLimit) + "}";
            }
            Serial.println(msg);
            digitalWrite(canLed, LOW); // Turn LED low
        }
    }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
