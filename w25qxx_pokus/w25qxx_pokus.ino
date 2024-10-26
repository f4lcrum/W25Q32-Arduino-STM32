#include <SPI.h>

#define PIN_SPI_SS              PA4
#define PIN_SPI_MOSI            PA7
#define PIN_SPI_MISO            PA6
#define PIN_SPI_SCK             PA5

#define numBLOCK 32  // number of total blocks for 16Mb flash, 32x16x16 pages and 32x16x16x256 Bytes

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

void W25Q_Read (uint32_t startPage, uint8_t offset, uint32_t ln, uint8_t *rData)
{
	uint8_t tData[5];
	uint32_t memAddr = (startPage*256) + offset;

	if (numBLOCK<512)   // Chip Size<256Mb
	{
		tData[0] = 0x03;  // enable Read
		tData[1] = (memAddr>>16)&0xFF;  // MSB of the memory Address
		tData[2] = (memAddr>>8)&0xFF;
		tData[3] = (memAddr)&0xFF; // LSB of the memory Address
	}
	else  // we use 32bit memory address for chips >= 256Mb
	{
		tData[0] = 0x13;  // Read Data with 4-Byte Address
		tData[1] = (memAddr>>24)&0xFF;  // MSB of the memory Address
		tData[2] = (memAddr>>16)&0xFF;
		tData[3] = (memAddr>>8)&0xFF;
		tData[4] = (memAddr)&0xFF; // LSB of the memory Address
	}

	csLOW();  // pull the CS Low
	if (numBLOCK<512)
	{
		SPI.transfer(tData, 4);  // send read instruction along with the 24 bit memory address
	}
	else
	{
		SPI.transfer(tData, 5);  // send read instruction along with the 32 bit memory address
	}

    for (int i = 0; i < ln; i++) {
        rData[i] = SPI.transfer(0);    
    }
	// SPI.transfer((uint8_t*)rData, NULL, ln);  // Read the data
	csHIGH();  // pull the CS High
}


uint32_t jedec_id = 0;
uint32_t manufacturer_id = 0;
uint8_t RxData[4608];
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
  Serial.println(manufacturer_id);
  W25Q_Read(0, 0, 25, RxData);


}

void loop() {
  // put your main code here, to run repeatedly:
    Serial.println(jedec_id);
    Serial.println(manufacturer_id);
    for (int i = 0; i < 25; i++) {
        Serial.println(RxData[i]);
    }
    Serial.println("-------------------");
    delay(1000);
}




