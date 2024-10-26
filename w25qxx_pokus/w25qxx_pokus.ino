#include <SPI.h>

#define PIN_SPI_SS              PA4
#define PIN_SPI_MOSI            PA7
#define PIN_SPI_MISO            PA6
#define PIN_SPI_SCK             PA5


void csHIGH() {
    digitalWrite(PIN_SPI_SS, HIGH);
}

void csLOW() {
    digitalWrite(PIN_SPI_SS, LOW);    
}

void W25Q_Reset (void)
{
    uint8_t tData[2];
    tData[0] = 0x66;  // enable Reset
    tData[1] = 0x99;  // Reset
    csLOW();
    SPI.transfer(tData, 2);
    csHIGH();
    delay(100);
}

uint32_t W25Q_Read_JEDEC_ID (void)
{
    uint8_t tData = 0x9F;  // Read JEDEC ID
    uint8_t rData[3];
    csLOW();
    SPI.transfer(tData);
    rData[0] = SPI.transfer(0);
    rData[1] = SPI.transfer(0);
    rData[2] = SPI.transfer(0);
    csHIGH();
    return ((rData[0]<<16)|(rData[1]<<8)|rData[2]);
}

uint32_t W25Q_Manufacturer_Device_ID (void) {
    uint8_t tData = 0x90; // device ID
    uint8_t rData[2];
    csLOW();
    SPI.transfer(tData);
    SPI.transfer(0);
    SPI.transfer(0);
    SPI.transfer(0);
    rData[0] = SPI.transfer(0);
    rData[1] = SPI.transfer(0);
    csHIGH();
    return (rData[0]<<8 | rData[1]);
}

uint32_t jedec_id = 0;
uint32_t manufacturer_id = 0;
void setup() {
  Serial.begin(9600);

  // start the SPI library:
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);

  // initalize the  data ready and chip select pins:
  pinMode(PIN_SPI_SS, OUTPUT);
  W25Q_Reset();
  jedec_id = W25Q_Read_JEDEC_ID();
  manufacturer_id = W25Q_Manufacturer_Device_ID();
  Serial.println(jedec_id);
}

void loop() {
  // put your main code here, to run repeatedly:
    Serial.println(jedec_id);
    Serial.println(manufacturer_id);
    delay(1000);
}




