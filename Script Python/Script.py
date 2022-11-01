#
# Copyright © 2016-2022 The Thingsboard Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


# Laboratorio de Microcontroladores
# Laboratorio 4 
# Gabriel Barahona Otoya B70896
# Yenner Gonzalez Araya B83375
# Script Python

import paho.mqtt.client as mqtt
from time import sleep
import random
import csv
import math

broker="iot.eie.ucr.ac.cr"
topic_pub='v1/devices/me/telemetry'

client = mqtt.Client()

client.username_pw_set("l4gy9675")
client.connect(broker)

f= open("data.csv")
reader = csv.reader(f)

print("Datos extraídos del archivo .csv: ")

for row in reader:
    print(row)

print("Enviando información a la plataforma IoT Thingsboard")
print("...")

xn = [0]
yn = [0]
zn = [0]
bn = [0]
n = 0
i = 0

limit = math.floor(len(row)/4)

while n<limit:
    xn = row[i]
    yn = row[i+1]
    zn = row[i+2]
    bn = row[i+3]
    
    datos = [xn, yn, zn, bn]

    battery = bn

    msgx = '{"x":"'+ datos[0] + '"}'
    msgy = '{"y":"'+ datos[1] + '"}'
    msgz = '{"z":"'+ datos[2] + '"}'

    if (int(battery)<=30):
        msgbattery = '{"Battery level":"Low level"}'
    elif (int(battery)>=70):
        msgbattery = '{"Battery level":"High level"}'
    elif (30<int(battery)<70):
    	msgbattery = '{"Battery level":"Mid level"}'

    client.publish(topic_pub, msgx)
    client.publish(topic_pub, msgy)
    client.publish(topic_pub, msgz)
    client.publish(topic_pub, msgbattery)
    sleep(0.1)

    i = i+4
    n = n+1


print("La información se ha subido con éxito")
