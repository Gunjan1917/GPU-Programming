import matplotlib.pyplot as plt
import csv
"""
Plot ESF, PSF, MTF
"""
x = []
y = []
a = []
b = []
m = []
n = []

with open('/home/vayalala/workspace2/OpenCLExercise3_Sobel/ValuesESF.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    for row in plots:
        x.append(float(row[0]))
        y.append(float(row[1]))
       
plt.plot(x, y, label='ESF')
plt.legend()
plt.show()

with open('/home/vayalala/workspace2/OpenCLExercise3_Sobel/ValuesPSF.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    for row in plots:
        a.append(float(row[0]))
        b.append(float(row[1]))
       
plt.plot(a, b, label='PSF')
plt.legend()
plt.show()

with open('/home/vayalala/workspace2/OpenCLExercise3_Sobel/ValuesMTF.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    for row in plots:
        m.append(float(row[0]))
        n.append(float(row[1]))
       
plt.plot(m, n, label='MTF')
plt.legend()
plt.show()