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
#include <LMK03806.h>
#include <PEBBLES.h>


int nDivide = 9;

int tsleep_write = 50;

void configureClock(std::shared_ptr<LMK03806> clock){
  for(int reg =0; reg < 31; reg++)
  {
      clock->read(reg);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
}

int main(int argc, char** argv) 
{  
  if(argc > 1)
  {
    nDivide = std::stoi(argv[1]);
  }

  std::shared_ptr<FT232H> ft232;
    try {
      ft232 = std::make_shared<FT232H>(MPSSEChip::Protocol::SPI0, MPSSEChip::Speed::FOUR_HUNDRED_KHZ, MPSSEChip::Endianness::MSBFirst, "","pebbles");
      std::cout << "Initialized FT232H device: " << ft232->to_string() << std::endl;
    } catch(std::exception& e) {
      std::cout << "Failed to initialize FT232H device: " << e.what() << std::endl;
      return 1;
    }

 // clock chip connection: SCL, SDI, DSO and LATCH are connected to FT232H pins D0, D1, D2 and D4 respectively
 // pebbles connection:
 // GPIO pins: 0 <-> D4, 1 <-> D5, 2 <-> D6, 3 <-> D7, 4 <-> C0, 5 <-> C1, 6 <-> C2, 7 <-> C3, 8 <-> C4, 9 <-> C5, 10 <-> C6, 11 <-> C7


  std::shared_ptr<SPIFTDICom> com(new SPIFTDICom(ft232));

  std::shared_ptr<LMK03806> clock(new LMK03806(com, ft232));
  configureClock(clock);

  std::shared_ptr<PEBBLES> pebbles(new PEBBLES(com, ft232, 10));

  std::cout<<"make clock on pin 1 and data on pin 2"<<std::endl;
  for(int i=0; i<1000; i++){

  ft232->gpio_write(2, 1);
  ft232->gpio_write(1, 1);
  ft232->gpio_write(1, 0);
  ft232->gpio_write(2, 0);
  ft232->gpio_write(1, 1);
  ft232->gpio_write(1, 0);

  }
  //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  //ft232->read(128);
  /*
  pebbles->read(0);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  pebbles->read(0);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  pebbles->read(0);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  pebbles->read(0);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  pebbles->read(0);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  pebbles->read(0);
  */


  return 0;
}
