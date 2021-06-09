#!/usr/bin/env python3

import sys
import os
from os import path
import matplotlib.pyplot as plt

x = [0,5,10,20,30,40,50]

y1 = [0,17,30,89,99,100,100]

y2 = [0,10,20,80,95,98,100]

y3 = [0,9,17,68,85,98,100]

plt.xlabel('Proportion of Byzantin node (in %)')
plt.ylabel('Proportion of Byzantin samples (in %)')

plt.title('Varying the Proportion of Byzantine nodes')
plt.plot(x,y1, 'ro')
plt.plot(x,y1, 'r', label='Brahms')
plt.plot(x,y2, 'bo')
plt.plot(x,y2, 'b', label='Brahms-sgx 1%')

plt.plot(x,y3, 'go')
plt.plot(x,y3, 'g', label='Brahms-sgx 5%')

plt.legend()
plt.grid()
plt.show()
