- check out the code:
`git clone --recursive https://gitlab.cern.ch/zhicaiz/ldrd_28nm_testing.git`

For some reason the network on trumer is not doing the above clone properly. Use another machine such as littleoakhorn or nosehorn to do the clone, then switch back to trumer to do the testing.

- compile:

```
mkdir build
cd build
cmake3 ..
```

- run:
```
cd run
./testAFE 6
```

where 6 is the divider for the output clock (from 2695MHz)
uncomment the last few lines in src/testAFE.cpp to select the test you want to do.

- upload sketch to arduino:
```
arduino --upload --board arduino:sam:arduino_due_x --port /dev/ttyACM0 arduino/metarock/metarock.ino
```
