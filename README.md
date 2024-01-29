- check out the code:
`git clone --recursive https://gitlab.cern.ch/zhicaiz/ldrd_28nm_testing.git`

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

where 6 is the divider for the output clock (from 2640MHz)

- upload sketch to arduino:
```
arduino --upload --board arduino:sam:arduino_due_x --port /dev/ttyACM0 arduino/metarock/metarock.ino
```
