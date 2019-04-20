/**
   ----------------------------------------------------------------------------
   This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
   for further details and other examples.

   NOTE: The library file MFRC522.h has a lot of useful info. Please read it.

   Released into the public domain.
   ----------------------------------------------------------------------------
   This sample shows how to read and write data blocks on a MIFARE Classic PICC
   (= card/tag).

   BEWARE: Data will be written to the PICC, in sector #1 (blocks #4 to #7).


   Typical pin layout used:
   -----------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin
   -----------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
   SPI SS      SDA(SS)      10            53        D10        10               10
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15

*/

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

/**
   Initialize.
*/
void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

/**
   Main loop.
*/
void loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent())
    return;

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
    return;

  char[] inputBuffer;
  byte numBytes = Serial.readBytesUntil('\n', buffer, 1024)

                  int sector = 1;
  int block = 4;
  int trailerBlock = 7;
  byte[16] toWrite;

  //wites full block from the input
  for (int i = 0; i < numBytes ; i += 16) {
    toWrite = {inputBuffer[i], inputBuffer[i + 1], inputBuffer[i + 2], inputBuffer[i + 3], inputBuffer[i + 4],
               inputBuffer[i + 5], inputBuffer[i + 6], inputBuffer[i + 7], inputBuffer[i + 8], inputBuffer[i + 9],
               inputBuffer[i + 10], inputBuffer[i + 11], inputBuffer[i + 12], inputBuffer[i + 13], inputBuffer[i + 14],
               inputBuffer[i + 15]
              };

    writeToCard(sector, block, toWrite, trailerBlock);

    block++;
    sector = block / 4;
    trailer = (sector * 4) + 3;
  }

  //extra chars at the end of the input
  extra = (numBytes % 16);
  for (int i = 0; i < 16; i++) {
    if (i < extra) {
      toWrite[i] = inputBuffer[(inputBuffer - extra) + i];
    }
    else {
      toWrite[i] = 0x00;
    }
  }
  writeToCard(sector, block, toWrite, trailerBlock);
  block++;
  if(block == trailer) { block++; }
  sector = block / 4;
  trailer = (sector * 4) + 3;

  //test
  byte dataBlock[]    = {
    0x73, 0x70, 0x6f, 0x74, //  s,  p,   o,  t,
    0x69, 0x66, 0x79, 0x3a, //  i,  f,   y,  :,
    0x00, 0x00, 0x00, 0x00, //  null, null, null, null,
    0x00, 0x00, 0x00, 0x00  // null, null, null, null
  };
  writeToCard(1, 4, dataBlock, 7);

}
void writeToCard(byte sector, byte blockAddr, byte[16] dataBlock, byte trailerBlock) {
  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  // Authenticate using key A
  Serial.println(F("Authenticating using key A..."));
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Authenticate using key B
  Serial.println(F("Authenticating again using key B..."));
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write data to the block
  Serial.print(F("Writing data into block ")); Serial.print(blockAddr);
  Serial.println(F(" ..."));
  dump_byte_array(dataBlock, 16); Serial.println();
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println();

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}

/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
