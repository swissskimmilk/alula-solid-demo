#include <Ra01S.h>
#include <map>
#include <list>
#include <vector>

#define INFO_STRING_ID 1
#define GNSS_ID 2

#define BARO_ID 3
#define SH2_ROT_ID 4
#define SH2_ACCEL_ID 5
#define UBX_ID 6
#define SATS_ID 7
#define TIME_ID 8
#define SH2_GYRO_ID 9
#define RESET_ID 10
#define ACCEL_ID 11

// int: 1, float: 2, long: 3
std::map<int,std::vector<int>> IDDataTypes {{BARO_ID, {2, 2}}, {ACCEL_ID, {2, 2, 2}}, {SH2_GYRO_ID, {2, 2, 2}}, {SH2_ROT_ID, {2, 2, 2}}, {SH2_ACCEL_ID, {2, 2, 2}}, {RESET_ID, {1}}, {TIME_ID, {3}}, {SATS_ID, {1}}, {UBX_ID, {1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}}};
std::map<int,std::vector<String>> IDStrings {{BARO_ID, {"Pressure: ", " | Temperature: "}}, {ACCEL_ID, {"X accel: ", " | Y accel: ", " | Z accel: "}}, {SH2_GYRO_ID, {"Gyro X: ", " | Gyro Y: ", " | Gyro Z: "}}, {SH2_ROT_ID, {"Yaw: ", " | Pitch: ", " | Roll: "}}, {SH2_ACCEL_ID, {"NG accel X: ", " | NG accel Y: ", " | NG accel Z: "}}, {RESET_ID, {"Reset: "}}, {TIME_ID, {"Curr micros: "}}, {SATS_ID, {"Num satellites: "}}, {UBX_ID, {"", ":", ":", ":", "| Latitude: ", " | Longitude: ", " | Altitude: ", " | North vel: ", " | East val: ", " | Down vel: ", " | Groundspeed: ", " | Vert acc: ", " | HorAcc: ", " | SpeedAcc: "}}};

//#define RF_FREQUENCY                                433000000 // Hz  center frequency
//#define RF_FREQUENCY                                866000000 // Hz  center frequency
#define RF_FREQUENCY                                915000000 // Hz  center frequency
#define TX_OUTPUT_POWER                             22        // dBm tx output power
#define LORA_SPREADING_FACTOR                       7         // spreading factor [SF5..SF12]
#define LORA_BANDWIDTH                              4         // bandwidth
                                                              // 2: 31.25Khz
                                                              // 3: 62.5Khz
                                                              // 4: 125Khz
                                                              // 5: 250KHZ
                                                              // 6: 500Khz 
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]

#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_PAYLOAD_LENGTH                          0         // 0: Variable length packet (explicit header)
                                                              // 1..255  Fixed length packet (implicit header)
SX126x  lora(15,               //Port-Pin Output: SPI select
             21,               //Port-Pin Output: Reset 
             39               //Port-Pin Input:  Busy
             );

void setup() 
{
  delay(1000);
  Serial.begin(115200);

  //lora.DebugPrint(true);

  int16_t ret = lora.begin(RF_FREQUENCY,              //frequency in Hz
                           TX_OUTPUT_POWER);          //tx power in dBm
  if (ret != ERR_NONE) while(1) {delay(1);}

  lora.LoRaConfig(LORA_SPREADING_FACTOR, 
                  LORA_BANDWIDTH, 
                  LORA_CODINGRATE, 
                  LORA_PREAMBLE_LENGTH, 
                  LORA_PAYLOAD_LENGTH, 
                  true,               //crcOn  
                  false);             //invertIrq
}

// int numTransmissions = 0;
// int currSize;
// long int t0 = 0;

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t rxData[255];
  uint8_t rxLen = lora.Receive(rxData, 255);
  if ( rxLen > 0 )
  {
    Serial.print("Receive rxLen: ");
    Serial.println(rxLen);
    int id = rxData[0];
    int i = 1;
    while (i < rxLen) {
      //Serial.println("we do be looping");
      for(int j = 0; j < IDDataTypes[id].size(); j++) {
        Serial.print(IDStrings[id][j]);
        int dataTypeID = IDDataTypes[id][j];
        // try {
        //   dataTypeID = IDDataTypes[id][j];
        //   // Block of code to try
        //   //throw exception; // Throw an exception when a problem arise
        // }
        // catch (...) {
        //   Serial.println("MALFORMED DATA! NO ID FOUND!");
        // }
        if (dataTypeID == 1) {
          Serial.print(rxData[i]);
          i++;
        }
        else if (dataTypeID == 2) {
          float f;
          memcpy(&f, rxData + i, 4);
          Serial.print(f);
          i += 4;
          //int* tmp = reinterpret_cast<int*>(buffer + 4);
        }
        else if (dataTypeID == 3) {
          long l;
          memcpy(&l, rxData + i, 4);
          Serial.print(l);
          i += 4;
        }
        else {
          Serial.println("INVALID DATATYPE MAPPING!");
        }
      }
      Serial.println();
      id = rxData[i];
      i++;
    }
    Serial.println();
    


    // if (numTransmissions == 0) {
    //   t0 = millis();
    // }
    // numTransmissions++;
    // currSize = rxLen;
    
    // for(int i=0;i< rxLen;i++) {
    //   Serial.print(rxData[i], HEX);
    //   Serial.print(" ");
    // }
    // Serial.println();
    // for(int i=0;i< rxLen;i++) {
    //   if (rxData[i] > 0x1F && rxData[i] < 0x7F) {
    //     char myChar = rxData[i];
    //     Serial.print(myChar);
    //   } else {
    //     Serial.print("?");
    //   }
    // }
    // Serial.println();

    // int8_t rssi, snr;
    // lora.GetPacketStatus(&rssi, &snr);
    // Serial.print("rssi: ");
    // Serial.print(rssi, DEC);
    // Serial.println(" dBm");
    // Serial.print("snr: ");
    // Serial.print(snr, DEC);
    // Serial.println(" dB");
    
    // if (numTransmissions == 100) {
    //   numTransmissions = 0;
    //   long int t1 = millis();
    //   float dataRate = (numTransmissions * currSize) / ((t1 - t0) / 1000);
    //   Serial.print(dataRate); Serial.println(" bytes per second");
    // }
  }
  delay(10);
}
