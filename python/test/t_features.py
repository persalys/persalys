#! /usr/bin/env python

import os

width = 40

# check that python can load the module
print('1: Python module load'.ljust(width), end=' ')
try:
    import persalys as otg
    print('OK')
except:
    print('no')

# check for Morris method
print('2: Morris method (otmorris)'.ljust(width), end=' ')
try:
    res = otg.MorrisResult()
    print('OK')
except:
    print('no')

# check for FMI support
print('3: FMI (otfmi)'.ljust(width), end=' ')
try:
    fmu = otg.FMUInfo()
    print('OK')
except:
    print('no')

# check for YACS support
print('4: YACS (salome)'.ljust(width), end=' ')
try:
    fmu = otg.YACSPhysicalMode()
    print('OK')
except:
    print('no')
