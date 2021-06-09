#!/usr/bin/env python3
#10% de noeud byzantin
import sys
import os
from os import path
import matplotlib.pyplot as plt

viewSize = 12
nbTour = 1
nbNode = 12


result = [];


#recuperation des byzantins
filename ="Result" + "byzantin.csv"

if not os.path.isfile(filename):
    print('File does not exist: byzantin')
else:
    f=open(filename, 'r')
    byzantin = f.read().splitlines()

byzantin = ["10.0.0.6:10301", "10.0.0.7:10300"]
#recuperation du pourcentage de byzantin en fin de cycle

for i in range(nbTour):

    percentByzTour = 0

    for k in range(nbNode):
        nodeByzPercent = 0
        tempnum = k
        tempstring = "%s" %tempnum
        filename ="Result/" + tempstring + "/data/data.csv"
        if not os.path.isfile(filename):
            print('File does not exist:' + tempstring)
        else:
            f=open(filename, 'r')
            content = f.read().splitlines()

        node = content[i].split(",")
        for x in node:
            print(node)
            for y in byzantin:
                if(x == y):
                    nodeByzPercent+=1

        nodeByzPercent /= viewSize
        nodeByzPercent *= 100
        percentByzTour+=nodeByzPercent

    percentByzTour/=nbNode
    result.append(percentByzTour)


temps = [i for i in range(nbTour)]
print(temps)
print(result)
plt.plot(temps,result, 'ro')

x = [10,50,100,150,200]
y1 = [100,98,95,40,25]

y2 = [100,97,90,37,25]

y3 = [100,92,80,35,25]

plt.xlabel('View size')
plt.ylabel('Proportion of Byzantin samples (in %)')

plt.title('Varying the view size')
#plt.plot(x,y1, 'ro')
#plt.plot(x,y1, 'r', label='Brahms')
#plt.plot(x,y2, 'bo')
#plt.plot(x,y2, 'b', label='Brahms-sgx 1%')

#plt.plot(x,y3, 'go')
#plt.plot(x,y3, 'g', label='Brahms-sgx 5%')

#plt.legend()
plt.grid()
plt.show()
