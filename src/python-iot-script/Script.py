
# Laboratorio de Microcontroladores
# Laboratorio 4 
# Gabriel Barahona Otoya B70896
# Yenner Gonzalez Araya B83375
# Script Python

# IMPORTS

import csv
import math
import serial
import random
from time import sleep
import paho.mqtt.client as mqtt


# Serial link

port = "/dev/ttyACM1" #serial port of stm32 board
baud = 115200 #stm32 runs at 115200 baud


ser = serial.Serial(port, baud)

samples = 5000 #how many samples to collect
line = 0 #start at 0 because our header is 0 (not real data)


# IoT link
broker="iot.eie.ucr.ac.cr"
topic_pub='v1/devices/me/telemetry'
client = mqtt.Client()
client.username_pw_set("l4gy9675")
client.connect(broker)



print("Enviando información a la plataforma IoT Thingsboard")
print("...")
while line <= samples:
    getData=ser.readline()
    dataString = getData.decode('utf-8')
    data=dataString[0:][:-2]

    splitdata = data.split(',')



    msgx = '{"Aceleración en X":"'+ splitdata[0] + '"}'
    msgy = '{"Aceleración en Y":"'+ splitdata[1] + '"}'
    msgz = '{"Aceleración en Z":"'+ splitdata[2] + '"}'
    msgbatteryp = '{"Porcentaje de batería":"'+ splitdata[3] + '"}'

    battery = splitdata[3]

    if (int(battery)<75):
        msgbatterys = '{"Battery level":"Low level"}'
    else:
    	msgbatterys = '{"Battery level":"High level"}'

    client.publish(topic_pub, msgx)
    client.publish(topic_pub, msgy)
    client.publish(topic_pub, msgz)
    client.publish(topic_pub, msgbatterys)
    client.publish(topic_pub, msgbatteryp)

    sleep(0.5)

    print(data)
    line = line+1

print("La información se ha enviado con éxito")
