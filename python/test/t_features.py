#! /usr/bin/env python

import persalys

width = 40

# check for Morris method
print('1: Morris method (otmorris)'.ljust(width), end=' ')
try:
    persalys.MorrisResult()
    print('ok')
except:
    print('n/a')

# check for FMI support
print('2: FMI (otfmi)'.ljust(width), end=' ')
try:
    persalys.FMUInfo()
    print('ok')
except:
    print('n/a')

# check for YACS support
print('3: YACS (salome)'.ljust(width), end=' ')
try:
    persalys.YACSPhysicalModel()
    print('ok')
except:
    print('n/a')
