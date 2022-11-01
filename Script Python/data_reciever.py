# Serial data reciever

import serial
import csv

arduino_port = "/dev/ttyACM1" #serial port of Arduino
baud = 115200 #arduino uno runs at 9600 baud
fileName="data.csv" #name of the CSV file generated

ser = serial.Serial(arduino_port, baud)
file = open(fileName, "w")

samples = 5000 #how many samples to collect
print_labels = False
line = 0 #start at 0 because our header is 0 (not real data)

while line <= samples:
    getData=ser.readline()
    dataString = getData.decode('utf-8')
    data=dataString[0:][:-2]
    print(data)
    line = line+1

    file = open(fileName, "a")
    file.write(data)

print("Data collection complete!")
file.close()