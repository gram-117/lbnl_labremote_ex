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

std::string outputFile="out.txt";

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
      ft232 = std::make_shared<FT232H>(MPSSEChip::Protocol::SPI2, MPSSEChip::Speed::FOUR_HUNDRED_KHZ, MPSSEChip::Endianness::MSBFirst, "","pebbles");
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
 //
  std::shared_ptr<SPIFTDICom> com(new SPIFTDICom(ft232));
  int R12_type3 = (0x00 << 27) | (0x03 << 24) | (0 << 23 ) | (0b000110000000000011 << 5) | 12;
  int R12_type4 = (0x00 << 27) | (0x04 << 24) | (0 << 23 ) | (0b000110000000000011 << 5) | 12;

  std::cout<<"Setting Ftest/LD_Type to 3:"<<std::endl;
  std::cout<<R12_type3<<" : "<<std::bitset<32>(R12_type3)<<std::endl;
  com->write_reg32(R12_type3);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout<<"Setting Ftest/LD_Type to 4:"<<std::endl;
  std::cout<<R12_type4<<" : "<<std::bitset<32>(R12_type4)<<std::endl;
  com->write_reg32(R12_type4);
  
  return 0;
}
