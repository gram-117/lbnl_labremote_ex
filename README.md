- check out the code:
`git clone --recursive https://gitlab.cern.ch/zhicaiz/ldrd_28nm_testing.git`
- compile:

```
mkdir build
cd build
cmake3 ..
make
```

- run:
```
cd run
../build/testAFE 6
```
metarockino code must be flashed onto Arduino UNO with the following steps: upload sketch to arduino (this is needed only if you want to update communication with arduino, which is almost never needed):

/home/zhicai/arduino-1.8.19/arduino --upload --board arduino:sam:arduino_due_x --port /dev/ttyACM0 arduino/metarock/metarock.ino

testpam4 code needs to do these following steps in order to be run:

cd ldrd_28nm_testing/
cd build/
make
cd ..
cd run/
../build/testPAM4 270 (integer is for the frequency you want to run the clock at, right now its configured for 10mhz)

