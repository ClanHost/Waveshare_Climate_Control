#include "Wire.h"
#include <SPI.h>
#define I2C_TEMP_ADDRESS 0x40
char dtaUart[15];
char dtaLen = 0;
uint8_t Data[100] = {0};
uint8_t buff[100] = {0};
#define I2C_SLAVE_ADDRESS_CC 0x55
#define QUANTITY_BYTES_TO_RECEIVE 1     // 1 to 16
#define STOP true                       // true - Release the bus / false - keep the connection alive


//// TEMP SENSOR
#define I2C_SLAVE_ADDRESS_FAN 0x66
uint8_t uint8_to_send = 46;             // Send a number - 0 and 255
uint8_t buf[4] = {0};
uint16_t data, data1;
float temp;
float hum;

//// DIGITAL POT 100K
#define CS_PIN 15   // Chip Select Pin
#define MOSI_PIN 23
#define MISO_PIN 19
#define SCK_PIN 18
const int POT0_SEL = 0x11;
const int POT1_SEL = 0x12;
const int BOTH_POT_SEL = 0x13;
const int POT0_SHUTDOWN = 0x21;
const int POT1_SHUTDOWN = 0x22;
const int BOTH_POT_SHUTDOWN = 0x23;
int POT0_Dn = 128;
int POT1_Dn = 128;
int BOTH_POT_Dn = 128;

void setup()
{
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  Wire.begin();
  pinMode(CS_PIN, OUTPUT);   // set the CS_PIN as an output:
  SPI.begin();     // initialize SPI:
}

void loop()
{





//////// WRITE TEMP TO CLIMATE CONTROL BUTTON ////////
  readReg(0x00, buf, 4);
  data = buf[0] << 8 | buf[1];
  data1 = buf[2] << 8 | buf[3];
  temp = ((float)data * 165 / 65535.0) - 40.0;
  hum =  ((float)data1 / 65535.0) * 100;
  Wire.beginTransmission(I2C_SLAVE_ADDRESS_CC);
  Wire.write(temp);
  Wire.write(hum);
  uint8_t CCerror = Wire.endTransmission(true);
//////// WRITE TEMP TO CLIMATE CONTROL BUTTON ////////

//////// RECEIVE FROM CLIMATE CONTROL BUTTON ////////
  uint8_t CCbytesReceived = Wire.requestFrom(I2C_SLAVE_ADDRESS_CC, QUANTITY_BYTES_TO_RECEIVE, STOP);
  uint8_t CCuint8_from_slave = Wire.read(); // Note below..
  //Serial.println(bytesReceived);
  Serial.println(CCuint8_from_slave);
//////// RECEIVE FROM CLIMATE CONTROL BUTTON ////////


//////// WRITE TEMP TO CLIMATE CONTROL BUTTON ////////
  Wire.beginTransmission(I2C_SLAVE_ADDRESS_FAN);
  Wire.write(50);
  uint8_t FANerror = Wire.endTransmission(true);
//////// WRITE TEMP TO CLIMATE CONTROL BUTTON ////////

//////// RECEIVE FROM FAN CONTROL BUTTON ////////
  uint8_t FANbytesReceived = Wire.requestFrom(I2C_SLAVE_ADDRESS_FAN, QUANTITY_BYTES_TO_RECEIVE, STOP);
  uint8_t FANuint8_from_slave = Wire.read(); // Note below..
  //Serial.println(bytesReceived);
  Serial.println(FANuint8_from_slave);
//////// RECEIVE FROM FAN CONTROL BUTTON ////////

      DigitalPotWrite(POT0_SEL, (FANuint8_from_slave*2.55));

  delay(10); //Pause for some time
}


void DigitalPotWrite(int cmd, int val)
{
  // constrain input value within 0 - 255
  val = constrain(val, 0, 255);
  // set the CS pin to low to select the chip:
  digitalWrite(CS_PIN, LOW);
  // send the command and value via SPI:
  SPI.transfer(cmd);
  SPI.transfer(val);
  // Set the CS pin high to execute the command:
  digitalWrite(CS_PIN, HIGH);
}

uint8_t readReg(uint8_t reg, const void* pBuf, size_t size)
{
  if (pBuf == NULL) {
    Serial.println("pBuf ERROR!! : null pointer");
  }
  uint8_t * _pBuf = (uint8_t *)pBuf;
  Wire.beginTransmission(I2C_TEMP_ADDRESS);
  Wire.write(&reg, 1);
  if ( Wire.endTransmission() != 0) {
    return 0;
  }
  delay(20);
  Wire.requestFrom(I2C_TEMP_ADDRESS, (uint8_t) size);
  for (uint16_t i = 0; i < size; i++) {
    _pBuf[i] = Wire.read();
  }
  return size;
}
