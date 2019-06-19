#include "it8951.h"

#include <SPI.h>

IT8951::IT8951(int cspin, int resetpin, int readypin) : _cs(cspin), _rst(resetpin), _hrdy(readypin) {
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
  delay(10);
  digitalWrite(_rst, HIGH);
  Serial.println(SPIspeed);
  delay(10);
  // get Dev Info
  updateDeviceInfo();

  // Enable I80 Packed mode
  writeRegister(I80CPCR, 0x0001);
  delay(10);
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
  SPI.endTransaction();
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
  SPI.endTransaction();
}

void IT8951::updateDeviceInfo(){
  command(GET_DEV_INFO);
  read(info.data, 20);
  Serial.print("Device Info:\n Panel ");
  Serial.print(info.width);
  Serial.print(" x ");
  Serial.println(info.height);
  Serial.print(" Address 0x");
  Serial.println(info.imgBufAddrL | (info.imgBufAddrH << 16), HEX);
  Serial.print(" Firmware: ");
  Serial.println(info.FWVersion);
  Serial.print(" LUT: ");
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

void IT8951::memBurstWrite(uint32_t addr, uint32_t size, uint16_t * buf){
    command(MEM_BST_WR);
    
    uint16_t values[4] = { 
      static_cast<uint16_t>(addr & 0x0000FFFF),         //addr[15:0]
      static_cast<uint16_t>((addr >> 16) & 0x0000FFFF), //addr[25:16]
      static_cast<uint16_t>(size & 0x0000FFFF),         //Cnt[15:0]
      static_cast<uint16_t>((size >> 16) & 0x0000FFFF)  //Cnt[25:16]
    };
    write(values, 4);
    
    //Burst Write Data
    /*for(uint32_t i=0; i<size; i++)
        write(&buf[i], 1); // TODO
    */
    write(buf, size);

    //Send Burst End Cmd
    command(MEM_BST_END);
}

void IT8951::memBurstRead(uint32_t addr, uint32_t size, uint16_t * buf){
    command(MEM_BST_RD_T);
    
    uint16_t values[4] = { 
      static_cast<uint16_t>(addr & 0x0000FFFF),         //addr[15:0]
      static_cast<uint16_t>((addr >> 16) & 0x0000FFFF), //addr[25:16]
      static_cast<uint16_t>(size & 0x0000FFFF),         //Cnt[15:0]
      static_cast<uint16_t>((size >> 16) & 0x0000FFFF)  //Cnt[25:16]
    };
    write(values, 4);

    command(MEM_BST_RD_S);
    read(buf, size);

    //Send Burst End Cmd
    command(MEM_BST_END);
}

bool IT8951::loadImage(uint16_t *buf, size_t len, uint16_t x, uint16_t y, uint16_t width, uint16_t height, enum ROTATE rot, enum BPP bpp, enum ENDIAN e, uint32_t addr){
  // Check Dimension
  if (x > info.width || y > info.height)
    return false;
  if (x + width > info.width)
    width = info.width - x;
  if (y + height > info.height)
    height = info.height - y;
  if (width == 0 || height == 0)
    return false;

  size_t req = height * width;
  switch (bpp){
    case BPP_2:
      req /= 8;
      break;
    case BPP_3:
    case BPP_4:
      req /= 4;
      break;
    case BPP_8:
      req /= 2;
  }

  if (req != len)
    return false;

  // Set Image Buffer Address

  Serial.println(addr == 0 ? (info.imgBufAddrL | (info.imgBufAddrH << 16)) : addr, HEX);
  setImageBuffer(addr == 0 ? (info.imgBufAddrL | (info.imgBufAddrH << 16)) : addr);

  waitForDisplay();

  // Calculate attributes
  uint16_t arg = (e << 8) | (bpp << 4) | (rot);

  // Full / partial refresh
  if (x == 0 && y == 0 && width == info.width && height == info.height) {

    command(LD_IMG);
    write(&arg, 1);
  } else {
    command(LD_IMG_AREA);
    uint16_t args[5] = { arg, x, y, width, height };
    Serial.print("arg ");
    Serial.println(arg);
    write(args, 5);
  }

  write(buf, len);

  command(LD_IMG_END);
  return true;
}

bool IT8951::display(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t mode, uint32_t addr){
  // Check Dimension
  if (x > info.width || y > info.height)
    return false;
  if (x + width > info.width)
    width = info.width - x;
  if (y + height > info.height)
    height = info.height - y;
  if (width == 0 || height == 0)
    return false;

  uint16_t args[7] = { x, y, width, height, mode, addr == 0 ? info.imgBufAddrL : (addr & 0x0000ffff), addr == 0 ? info.imgBufAddrH : ((addr >> 16) & 0x0000ffff)};

  waitForDisplay();

  command(DPY_BUF_AREA);
  write(args, 7);


  return true;
}

bool IT8951::clear(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t mode){
  // Check clear
  if (x > info.width || y > info.height)
    return false;
  if (x + width > info.width)
    width = info.width - x;
  if (y + height > info.height)
    height = info.height - y;
  if (width == 0 || height == 0)
    return false;
  
  //Set Display mode to 1 bpp mode - Set 0x18001138 Bit[18](0x1800113A Bit[2])to 1
  writeRegister(UP1SR2, readRegister(UP1SR2) | (1<<2));   
  writeRegister(BGVR, 0x0000);
    
  //Display
  uint16_t args[5] = { x, y, width, height, mode};
  command(DPY_AREA);
  write(args, 5);

  waitForDisplay();
    
  //Restore to normal mode
  writeRegister(UP1SR2, readRegister(UP1SR2) & ~(1<<2));

  return true;
}
