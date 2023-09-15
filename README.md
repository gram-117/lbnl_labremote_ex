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
./testINO 7
```

where 7 is the divider for the output clock (from 2640MHz)
