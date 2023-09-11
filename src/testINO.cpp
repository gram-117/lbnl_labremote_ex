#include <iostream>
#include <chrono>
#include <thread>
#include <getopt.h>
#include <string.h>
#include <fstream>
#include <string>
#include <iomanip>
#include <sys/stat.h>
#include <map>
#include <bitset>

#include <FT232H.h>
#include <HYT271.h>
#include <ComIOException.h>
#include <SPIFTDICom.h>
#include <LMK03806INO.h>
#include <PEBBLESINO.h>
#include <TextSerialCom.h>
#include <SerialCom.h>
#include "Logger.h"


int nDivide = 9;

int tsleep_write = 10;

void configureClock(std::shared_ptr<LMK03806INO> clock){
  std::cout<<"======== Configure LMK03806 CLOCK:"<<std::endl;
  for(int reg =0; reg < 31; reg++)
  {
      clock->read(reg);
      std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }

  // unlock registers for write
  uint32_t R31 = 0b11111;
  clock->write(R31);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));
  //step 1: reset R0
  uint32_t R0 = (0 << 18) | (1 << 17) | 0;
  clock->write(R0);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));
  //step 2: R0 to R5:
  R0 = (0 << 18) | (0 << 17) | (nDivide << 5) | 0; // use 66 for 40MHz; 3 for 833 MHz, 2 for 1.25 GHz
  clock->write(R0);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));
  clock->write(R0);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  uint32_t R3 = (0 << 18) | (0x01 << 5) | 3;
  clock->write(R3);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  // R6 to R8
  uint32_t R6 = (0x01 << 29) | (0x01 << 24) | (0x01 << 20) | (0x01 << 16) | 6;
  clock->write(R6);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  uint32_t R7 = (0x01 << 29) | (0x01 << 24) | (0x01 << 20) | (0x01 << 16) | 7;
  clock->write(R7);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  uint32_t R8 = (0x01 << 29) | (0x01 << 24) | (0x01 << 20) | (0x01 << 16) | 8;
  clock->write(R8);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  // R9
  uint32_t R9 = (0b010101010101010101010101010 << 5) | 9;
  clock->write(R9);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  // R10
  uint32_t R10 = (0 << 30 ) | (1 << 28) | (0x01 << 24) | (0b1100 << 20) | (0 << 16) | (1 << 14) | 10;
  clock->write(R10);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  // R11
  uint32_t R11 = (0b001101 << 26) | (0 << 20) | (1 << 16 ) | (0 << 12) | (1 << 5) | 11;
  clock->write(R11);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  // R12
  uint32_t R12 = (0x03 << 27) | (0x03 << 24) | (0 << 23 ) | (0b000110000000000011 << 5) | 12;
  clock->write(R12);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  // R13
  uint32_t R13 = (0b00111 << 27) | (3 << 24) | (2 << 16 ) | (1 << 15) | 13;
  clock->write(R13);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  // R14
  uint32_t R14 = (0 << 27) | (2 << 24) | 14;
  clock->write(R14);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  // R16
  uint32_t R16 = (0b1100000101010101 << 16) | 16;
  clock->write(R16);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));
  
  //PLL:
  uint32_t R24 = (0 << 28) | (0 << 24) | (0 << 20) | (0 << 16) | 24;
  clock->write(R24);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  uint32_t R26 = (1 << 31) | (1 << 29) | (3 << 26) | (0b111010 << 20) | (8192 << 5 ) | 26;
  clock->write(R26);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  uint32_t R28 = (2 << 20) | 28;
  clock->write(R28);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  uint32_t R29 = (0 << 24) | (1 << 22) | (25 << 5) | 29;
  clock->write(R29);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  uint32_t R30 = (5 << 24) | (25 << 5) | 30;
  clock->write(R30);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));
  clock->write(R30);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));
 

  // lock all registers - R0 to R30 only readable now 
  R31 = 0b111111;
  clock->write(R31);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));
  
  // read back all registers
  for(int reg =0; reg < 31; reg++)
  {
      clock->read(reg);
      std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }


  //  to verify MICROWIRE programming, set the LD_MUX = 0
  //  (Low) and then toggle the LD_TYPE register between 3 (Output, push-pull) and 4 (Output inverted, pushpull).
  //  The result will be that the Ftest/LD pin will toggle from low to high.

 /* 
  uint32_t R12_type3 = (0x00 << 27) | (0x03 << 24) | (0 << 23 ) | (0b000110000000000011 << 5) | 12;
  uint32_t R12_type4 = (0x00 << 27) | (0x04 << 24) | (0 << 23 ) | (0b000110000000000011 << 5) | 12;
  for(int i=0; i<2; i++){
      clock->write(R12_type3);
      std::this_thread::sleep_for(std::chrono::milliseconds(3000));
      clock->write(R12_type4);
      std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  }
  */
  std::cout<<"======== Configure LMK03806 CLOCK done ============="<<std::endl;
}

int main(int argc, char** argv) 
{  
  if(argc > 1)
  {
    nDivide = std::stoi(argv[1]);
  }

  std::shared_ptr<TextSerialCom> com(new TextSerialCom("/dev/ttyACM0", SerialCom::BaudRate::Baud9600));
  com->setTermination("\n");
  com->init();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  //clock
  std::shared_ptr<LMK03806INO> clock(new LMK03806INO(com));
  configureClock(clock);

  std::cout<<"======== Configure PEBBLES chip:"<<std::endl;
  std::shared_ptr<PEBBLESINO> pebbles(new PEBBLESINO(com));
  pebbles->writeGPIO("LOOPNK_EN", 1);

  // toggle reset
  pebbles->writeGPIO("RST", 2); // signal width is about 2 us 
  // send config for injection and enable
  int CFGIN = (12 << 28) | (0b10 << 10);
  pebbles->writeConfig(CFGIN);
  // send prime signal
  pebbles->writeGPIO("PRIME", 2); // signal width is about 2 us 
  // send mode
  pebbles->writeGPIO("MODE", 1);
  pebbles->readSOUT();


  return 0;
}
