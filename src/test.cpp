#include <iostream>
#include <chrono>
#include <thread>
#include <getopt.h>
#include <string.h>
#include <signal.h>
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

std::string outputFile="out.txt";

int tsleep_write = 50;

bool quit=false;
void cleanup(int signum)
{ quit=true; }

void usage(char *argv[])
{
  std::cout << "Usage: "<< argv[0] << " [-o output file]"<< std::endl;
  std::cout << "   defaults: -c " << outputFile << std::endl;
}

int main(int argc, char** argv) 
{  
  if (argc < 1)
    {
      usage(argv);
      return 1;
    }
  int c;
  while (1)
    {
      int option_index = 0;
      static struct option long_options[] = {
        {"output",     required_argument, 0,  'o' },
        {0,          0,                 0,   0  }
      };
      c = getopt_long(argc, argv, "o", long_options, &option_index);
      if (c == -1)
        {
          break;
        }
      switch (c)
        {
          case 'o':
            outputFile = optarg;
            break;
          default:
            std::cerr << "Invalid option '" << c << "' supplied. Aborting." << std::endl;
            std::cerr << std::endl;
            usage(argv);
            return 1;
        }
    }

  signal(SIGINT, cleanup);
  std::shared_ptr<FT232H> ft232;
    try {
      ft232 = std::make_shared<FT232H>(MPSSEChip::Protocol::SPI0, MPSSEChip::Speed::FOUR_HUNDRED_KHZ, MPSSEChip::Endianness::MSBFirst, "","pebbles");
      std::cout << "Initialized FT232H device: " << ft232->to_string() << std::endl;
    } catch(std::exception& e) {
      std::cout << "Failed to initialize FT232H device: " << e.what() << std::endl;
      return 1;
    }

 // mode SPI2: https://en.wikipedia.org/wiki/Serial_Peripheral_Interface#Mode_numbers
 // connection: SCL, SDI, DSO and LATCH are connected to FT232H pins AD0, AD1, AD2 and AD3 respectively
 //
 //  to verify MICROWIRE programming, set the LD_MUX = 0
 //  (Low) and then toggle the LD_TYPE register between 3 (Output, push-pull) and 4 (Output inverted, pushpull).
 //  The result will be that the Ftest/LD pin will toggle from low to high.
 //
 //
 

  uint32_t R12_type3 = (0x00 << 27) | (0x03 << 24) | (0 << 23 ) | (0b000110000000000011 << 5) | 12;
  uint32_t R12_type4 = (0x00 << 27) | (0x04 << 24) | (0 << 23 ) | (0b000110000000000011 << 5) | 12;

  std::shared_ptr<SPIFTDICom> com(new SPIFTDICom(ft232));
  std::shared_ptr<LMK03806> clock(new LMK03806(com, ft232));

  for(int reg =0; reg < 31; reg++)
  {
      clock->read(reg);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  //step 1: reset R0
  uint32_t R0 = (0 << 18) | (1 << 17) | 0;
  clock->write(R0);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));
  //step 2: R0 to R5:
  R0 = (0 << 18) | (0 << 17) | (9 << 5) | 0; // use 66 for 40MHz; 3 for 833 MHz, 2 for 1.25 GHz
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

  /*
  for(int reg =0; reg < 31; reg++)
  {
      clock->read(reg);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  */

  /*
  uint32_t R3 = (0 << 18) | (0x01 << 5) | 3;
  std::cout<<"Setting R3 to :"<<std::endl;
  std::cout<<R3<<" : "<<std::bitset<32>(R3)<<std::endl;
  clock->write(R3);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

   
  uint32_t R10 = (0 << 30 ) | (1 << 28) | (0x01 << 24) | (0b0100 << 20) | (0 << 16) | (1 << 14) | 10;
  std::cout<<"Enable OSCout 0 ..."<<std::endl;
  std::cout<<R10<<" : "<<std::bitset<32>(R10)<<std::endl;
  clock->write(R10);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  uint32_t R29 = (0 << 24) | (1 << 22) | (48 << 5) | 29;
  std::cout<<"Setting R29 to :"<<std::endl;
  std::cout<<R29<<" : "<<std::bitset<32>(R29)<<std::endl;
  clock->write(R29);
  std::this_thread::sleep_for(std::chrono::milliseconds(10000));
  */
  /* 
  for(int i=0; i<2; i++){
      clock->write(R12_type3);
      std::this_thread::sleep_for(std::chrono::milliseconds(3000));
      clock->write(R12_type4);
      std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  }
  */

  //
  //
  
  return 0;
}
