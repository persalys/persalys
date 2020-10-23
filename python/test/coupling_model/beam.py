#!/usr/bin/python3
import time

def main():
    time.sleep(2)
    F = 1.00e+00
    E = 3.00e+04
    L = 5.00e+00
    I = 1.10e-03
    dev = F * L**3 / (3 * E * I)
    with open('beam_output.txt', 'w') as f:
        f.write("deviation="+str(dev)+"\n")
    return dev

if __name__ == '__main__':
    main()
