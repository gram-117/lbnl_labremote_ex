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
#include <vector>

#include <FT232H.h>
#include <HYT271.h>
#include <ComIOException.h>
#include <SPIFTDICom.h>
#include <LMK03806INO.h>
#include <METAROCKINO.h>
#include <TextSerialCom.h>
#include <SerialCom.h>
#include "Logger.h"
#include <bitset>

#include "PowerSupplyChannel.h"
#include "EquipConf.h"


std::string equipConfigFile = "equip_testbench.json";



float F_VCO = 2695.0;//VCO frequency, measured with scope
int nDivide = 6; // dT = 2.2263
int nDivide_in = 6;
int nDivide_40MHz = 66;
std::string outFileName = "out.csv";

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


  uint32_t R5 = (0 << 18) | (nDivide_40MHz << 5) | 5; // 40 MHz clock CLKout10/11, dT=24.49ns
  clock->write(R5);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));
  clock->write(R5);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));


  // R6 to R8
  uint32_t R6 = (0x01 << 29) | (0x01 << 24) | (0x01 << 20) | (0x01 << 16) | 6;
  clock->write(R6);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  uint32_t R7 = (0x01 << 29) | (0x01 << 24) | (0x01 << 20) | (0x01 << 16) | 7;
  clock->write(R7);
  std::this_thread::sleep_for(std::chrono::milliseconds(tsleep_write));

  uint32_t R8 = (0x01 << 29) | (0x06 << 24) | (0x01 << 20) | (0x01 << 16) | 8;
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

void calibrateTDC(std::shared_ptr<LMK03806INO> clock, std::shared_ptr<METAROCKINO> metarock, uint32_t cfgin){

  int nDivide_back = nDivide;
  nDivide = 4;

  configureClock(clock);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  metarock->calibrateTDC(cfgin, 1000.0/(F_VCO/nDivide), 1000);

  nDivide = nDivide_back;

  configureClock(clock);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

}
int main(int argc, char** argv) 
{  
  if(argc > 1)
  {
    nDivide_in = std::stoi(argv[1]);
    if (nDivide_in > 0) nDivide = nDivide_in;
  }

  if(argc > 2)
  {
    outFileName = argv[2];
  }

  EquipConf hw;
  hw.setHardwareConfig(equipConfigFile);

  std::shared_ptr<PowerSupplyChannel> ps_vbp_iref = hw.getPowerSupplyChannel("VBP_IREF");
  ps_vbp_iref->setVoltageProtect(0.9);
  ps_vbp_iref->setCurrentLevel(-15e-6);
  ps_vbp_iref->turnOn();
  //
  std::shared_ptr<PowerSupplyChannel> ps_vaf = hw.getPowerSupplyChannel("VAF");
  ps_vaf->setVoltageLevel(0.168);
  ps_vaf->turnOn();


  /* 
  std::shared_ptr<PowerSupplyChannel> ps_VchargeP2 = hw.getPowerSupplyChannel("VchargeP2");
  ps_VchargeP2->setVoltageProtect(0.9);
  ps_VchargeP2->setCurrentLevel(-15e-9);
  ps_VchargeP2->turnOn();

  std::shared_ptr<PowerSupplyChannel> ps_VchargeN2 = hw.getPowerSupplyChannel("VchargeN2");
  ps_VchargeN2->setVoltageProtect(0.9);
  ps_VchargeN2->setCurrentLevel(30e-9);
  ps_VchargeN2->turnOn();
  */

  std::shared_ptr<PowerSupplyChannel> ps_vcal = hw.getPowerSupplyChannel("VCAL");
  ps_vcal->setVoltageLevel(0.0);
  ps_vcal->turnOn();

  std::shared_ptr<PowerSupplyChannel> ps_vff = hw.getPowerSupplyChannel("VFF");
  ps_vff->setVoltageLevel(0.131);
  ps_vff->turnOn();


  //std::shared_ptr<PowerSupplyChannel> ps_vthcomp = hw.getPowerSupplyChannel("VthComp");
  //ps_vthcomp->setVoltageLevel(0.7);
  //ps_vthcomp->turnOn();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  //logger(logINFO) << "vbp_iref voltage [V]: "<<ps_vbp_iref->measureVoltage();
  //logger(logINFO) << "vbp_iref current [A]: "<<ps_vbp_iref->measureCurrent();

  //logger(logINFO) << "VchargeP2 voltage [V]: "<<ps_VchargeP2->measureVoltage();
  //logger(logINFO) << "VchargeP2 current [A]: "<<ps_VchargeP2->measureCurrent();
  //logger(logINFO) << "VchargeN2 voltage [V]: "<<ps_VchargeN2->measureVoltage();
  //logger(logINFO) << "VchargeN2 current [A]: "<<ps_VchargeN2->measureCurrent();

  logger(logINFO) << "vff voltage [V]: "<<ps_vff->measureVoltage();
  logger(logINFO) << "vff current [A]: "<<ps_vff->measureCurrent();

  logger(logINFO) << "vaf voltage [V]: "<<ps_vaf->measureVoltage();
  logger(logINFO) << "vaf current [A]: "<<ps_vaf->measureCurrent();

  //logger(logINFO) << "vthcomp voltage [V]: "<<ps_vthcomp->measureVoltage();
  //logger(logINFO) << "vthcomp current [A]: "<<ps_vthcomp->measureCurrent();


  std::shared_ptr<TextSerialCom> com(new TextSerialCom("/dev/ttyACM0", SerialCom::BaudRate::Baud115200));
  com->setTermination("\n");
  com->init();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  //clock
  std::shared_ptr<LMK03806INO> clock(new LMK03806INO(com));
  if (nDivide_in > 0) {
    configureClock(clock);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }


  std::cout<<"======== Configure METAROCK chip:"<<std::endl;
  std::shared_ptr<METAROCKINO> metarock(new METAROCKINO(com, 1000.0/(F_VCO/nDivide)));
  metarock->writeGPIO("LOOPNK_EN", 1);

  float vcal = 0.0;
  ps_vcal->setVoltageLevel(vcal);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  logger(logINFO) << "vcal voltage [V]: "<<ps_vcal->measureVoltage();
  logger(logINFO) << "vcal current [A]: "<<ps_vcal->measureCurrent();

  int ch = 11;
  uint32_t inj = 10;
  std::cout<<"Injecting on channel: "<<ch<<std::endl;
  float Qinj = 10000.0*(inj*0.9 + vcal)*1.46/(5*1.6);

  std::cout<<"Injection config: "<<inj<<", injection charge = "<<Qinj<<" electrons"<<std::endl;

  uint32_t inj_reversed =
        ((inj & 0b0001) << 3) |
        ((inj & 0b0010) << 1) |
        ((inj & 0b0100) >> 1) |
        ((inj & 0b1000) >> 3);
  uint32_t cfgin = (0b1 << (31-ch)) | (inj_reversed << 10);
  std::cout<<"cfgin: "<<std::bitset<32>(cfgin)<<std::endl;

  //calibrate TDC
  //calibrateTDC(clock, metarock, cfgin);  

  //metarock->scanHitsVsInj(ch, ps_vcal, outFileName, 50, 500, 2000, 10, false);//ch11, thr 2.80

  metarock->doScan(cfgin, 10, outFileName, true);
 
  /*
  for(float vthcomp_temp =0.88; vthcomp_temp<0.92; vthcomp_temp += 0.01){
  std::cout<<"vthcomp: "<<vthcomp_temp<<std::endl;
  ps_vthcomp->setVoltageLevel(vthcomp_temp);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  metarock->scanTimeVsInj(ch, ps_vcal, outFileName, 100, 10.0, 13.0, true, 3.0);
  }
  */
  //metarock->scanTimeVsInj(ch, ps_vcal, outFileName, 100, 10.0, 13.0, true, 3.0);


  //metarock->scanTimeVsInj(ch, ps_vcal, outFileName, 200, 5.0, 13.0, true, 5.0);
  //for(float VchargeN2_temp =1.0; VchargeN2_temp<15.0; VchargeN2_temp += 0.5){
  /*for(float VchargeP2_temp =10.0; VchargeP2_temp<40.0; VchargeP2_temp += 1.0){
      std::cout<<"=========================================================================:"<<std::endl;
      std::cout<<"VchargeP2: "<<VchargeP2_temp<<" nA"<<std::endl;
      //std::cout<<"VchargeN2: "<<VchargeN2_temp<<" nA"<<std::endl;
      ps_VchargeP2->setCurrentLevel(-1e-9*VchargeP2_temp);
      //ps_VchargeN2->setCurrentLevel(1e-9*VchargeN2_temp);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));

      logger(logINFO) << "VchargeP2 voltage [V]: "<<ps_VchargeP2->measureVoltage();
      logger(logINFO) << "VchargeP2 current [A]: "<<ps_VchargeP2->measureCurrent();
      logger(logINFO) << "VchargeN2 voltage [V]: "<<ps_VchargeN2->measureVoltage();
      logger(logINFO) << "VchargeN2 current [A]: "<<ps_VchargeN2->measureCurrent();
      //std::cout<<"=== 40MHz:"<<std::endl;
      //nDivide_40MHz  = 66;
      //configureClock(clock);
      //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      metarock->scanTimeVsInj(ch, ps_vcal, outFileName, 100, 5.0, 13.0, true, 5.0);

     }
  //}
   */
 
  /*
  for(int id = 1; id < 3; id++) {
    nDivide_40MHz  = 66 * id;
    std::cout<<"clock dT = "<<1000.0/(F_VCO/nDivide_40MHz)<<std::endl;
    configureClock(clock);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    metarock->scanTimeVsInj(ch, ps_vcal, outFileName, 200, 5.0, 13.0, true, 5.0);
  }
  */
  

  return 0;
}
