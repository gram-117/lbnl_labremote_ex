- check out the code:
`git clone --recursive https://gitlab.cern.ch/zhicaiz/ldrd_28nm_testing.git`

- compile:

'''
mkdir build
cd build
cmake3 ..
'''

- run:
'''
cd build
./test 9
'''

where 9 is the divider for the output clock (from 2500MHz)
