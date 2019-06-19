#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif


class IT8951 {
  enum PREAMBLE {
    COMMAND    = 0x6000,
    WRITE_DATA = 0x0000,
    READ_DATA  = 0x1000
  };
  
  enum COMMANDS {
      SYS_RUN      = 0x0001,
      STANDBY      = 0x0002,
      SLEEP        = 0x0003,
      REG_RD       = 0x0010,
      REG_WR       = 0x0011,
      MEM_BST_RD_T = 0x0012,
      MEM_BST_RD_S = 0x0013,
      MEM_BST_WR   = 0x0014,
      MEM_BST_END  = 0x0015,
      LD_IMG       = 0x0020,
      LD_IMG_AREA  = 0x0021,
      LD_IMG_END   = 0x0022,
  
      DPY_AREA     = 0x0034,
      GET_DEV_INFO = 0x0302,
      DPY_BUF_AREA = 0x0037
  };

  enum REGISTER {
    //Base Address of Basic LUT Registers
    LUT0EWHR  = 0x1000,   //LUT0 Engine Width Height Reg
    LUT0XYR   = 0x1040,   //LUT0 XY Reg
    LUT0BADDR = 0x1080,   //LUT0 Base Address Reg
    LUT0MFN   = 0x10C0,   //LUT0 Mode and Frame number Reg
    LUT01AF   = 0x1114,   //LUT0 and LUT1 Active Flag Reg

    //Update Parameter Setting Register
    UP0SR = 0x1134,      //Update Parameter0 Setting Reg

    UP1SR     = 0x1138,  //Update Parameter1 Setting Reg
    LUT0ABFRV = 0x113C,  //LUT0 Alpha blend and Fill rectangle Value
    UPBBADDR  = 0x117C,  //Update Buffer Base Address
    LUT0IMXY  = 0x1180,  //LUT0 Image buffer X/Y offset Reg
    LUTAFSR   = 0x1224,  //LUT Status Reg (status of All LUT Engines)

    BGVR      = 0x1250,  //Bitmap (1bpp) image color table

    //Address of System Registers
    I80CPCR   = 0x0004,

    //-------Memory Converter Registers----------------
    MCSR      = 0x0200,
    LISAR_L   = 0x0208,
    LISAR_H   = 0x020A,
  };

  union DevInfo {
    struct {
      uint16_t width;
      uint16_t height;
      uint16_t imgBufAddrL;
      uint16_t imgBufAddrH;
      char FWVersion[16];
      char LUTVersion[16];
    };
    uint16_t data[20];
  };

 public:
  IT8951(int8_t cspin, int8_t resetpin, int8_t readypin);
  bool begin(uint32_t speed = 500000);
  void active();
  void standby();
  void sleep();
  uint16_t width();
  uint16_t height();
  void setImageBuffer(uint32_t addr);

 private:
  void waitUntilReady();
  void command(enum COMMANDS cmd);
  void write(uint16_t data[], size_t len);
  void read(uint16_t data[], size_t len);
  DevInfo getDevInfo();
  uint16_t readRegister(enum REGISTER reg);
  void writeRegister(enum REGISTER reg, uint16_t value);
  void waitForDisplay();

  int8_t _cs, _rst, _hrdy;
  DevInfo info;
  uint32_t SPIspeed; 

};
