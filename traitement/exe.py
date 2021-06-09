#!/usr/bin/env python3
#10% de noeud byzantin
import sys
import os
from os import path
import matplotlib.pyplot as plt

# Define a filename.
filename = "data.csv"

if not os.path.isfile(filename):
    print('File does not exist.')
else:
# Open the file as f.
# The function readlines() reads the file.
with open(filename) as f:
    content = f.read().splitlines()

# Show the file contents line by line.
# We added the comma to print single newlines and not double newlines.
# This is because the lines contain the newline character '\n'.
for line in content:
    print(line)





x = [10,50,100,150,200]
y1 = [100,98,95,40,25]

y2 = [100,97,90,37,25]

y3 = [100,92,80,35,25]

plt.xlabel('View size')
plt.ylabel('Proportion of Byzantin samples (in %)')

plt.title('Varying the view size')
plt.plot(x,y1, 'ro')
plt.plot(x,y1, 'r', label='Brahms')
plt.plot(x,y2, 'bo')
plt.plot(x,y2, 'b', label='Brahms-sgx 1%')

plt.plot(x,y3, 'go')
plt.plot(x,y3, 'g', label='Brahms-sgx 5%')

plt.legend()
plt.grid()
plt.show()
