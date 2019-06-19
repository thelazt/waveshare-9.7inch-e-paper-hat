#include "it8951.h"

#include <SPI.h>

IT8951::IT8951(int8_t cspin, int8_t resetpin, int8_t readypin) : _cs(cspin), _rst(resetpin), _hrdy(readypin) {
}

void IT8951::waitUntilReady(){
  while (digitalRead(_hrdy) == LOW);
}

void IT8951::command(enum COMMANDS cmd) {
  SPI.beginTransaction(SPISettings(SPIspeed, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  waitUntilReady();
  SPI.transfer16(COMMAND);
  waitUntilReady();
  SPI.transfer16(cmd);
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}

void IT8951::write(uint16_t data[], size_t len){
  SPI.beginTransaction(SPISettings(SPIspeed, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  waitUntilReady();
  SPI.transfer16(WRITE_DATA);
  waitUntilReady();
  for (size_t i = 0; i < len; i++)
    SPI.transfer16(data[i]);
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();              // release the SPI bus
}

void IT8951::read(uint16_t data[], size_t len) {
  SPI.beginTransaction(SPISettings(SPIspeed, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  waitUntilReady();
  SPI.transfer16(READ_DATA);
  waitUntilReady();
  data[0] = SPI.transfer16(0);
  waitUntilReady();
  for (size_t i = 0; i < len; i++)
    data[i] = SPI.transfer16(0);
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();              // release the SPI bus
}

union IT8951::DevInfo IT8951::getDevInfo(){
  DevInfo info;
  command(GET_DEV_INFO);
  read(info.data, 20);
  return info;
  
}

bool IT8951::begin(uint32_t speed){
  SPIspeed = speed;

  // busy state
  pinMode(_hrdy, INPUT);

  // clock select
  digitalWrite(_cs, HIGH);
  pinMode(_cs, OUTPUT);

  // hardware SPI
  SPI.begin();

  // reset line
  digitalWrite(_rst, LOW);
  pinMode(_rst, OUTPUT);
  delay(100);
  digitalWrite(_rst, HIGH);

  // get Dev Info
  DevInfo info = getDevInfo();
  Serial.print("DevInfo:\n Panel ");
  Serial.print(info.width);
  Serial.print(" x ");
  Serial.println(info.height);
  Serial.print(" Address ");
  Serial.println(info.imgBufAddrL | (info.imgBufAddrH << 16));
  Serial.print(" Firmware ");
  Serial.println(info.FWVersion);
  Serial.print(" Lut ");
  Serial.println(info.LUTVersion);
}

uint16_t IT8951::width(){
  return info.width;
}

uint16_t IT8951::height(){
  return info.height;
}
  
void IT8951::active(){
  command(SYS_RUN);
}

void IT8951::standby(){
  command(STANDBY);
}

void IT8951::sleep(){
  command(SLEEP);
}

uint16_t IT8951::readRegister(enum REGISTER reg){
  command(REG_RD);
  uint16_t value = reg;
  write(&value, 1);
  read(&value, 1);
  return value;
}

void IT8951::writeRegister(enum REGISTER reg, uint16_t value){
  command(REG_WR);
  uint16_t values[2] = { reg, value };
  write(values, 2);
}

void IT8951::waitForDisplay(){
  while(readRegister(LUTAFSR) != 0); // if Reg is 0 then ready
}

void IT8951::setImageBuffer(uint32_t addr){
  //Write LISAR Reg
  writeRegister(LISAR_H, ((addr >> 16) & 0x0000FFFF));
  writeRegister(LISAR_L, addr & 0x0000FFFF);
}
