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

import paho.mqtt.client as mqtt
from time import sleep
import random
import csv

broker="iot.eie.ucr.ac.cr"
topic_pub='v1/devices/me/telemetry'

client = mqtt.Client()

client.username_pw_set("l4gy9675")
client.connect(broker)

f= open("data.csv")
reader = csv.reader(f)

for row in reader:
    print (row)

xn = 0
yn = 1
zn = 2
bn = 3
n=0

while n<1:
    index = (xn, yn, zn, bn)
    datos = [row[i] for i in index]
    print(datos)

    battery = datos[bn]

    msgx = '{"x":"'+ datos[xn] + '"}'
    msgy = '{"y":"'+ datos[yn] + '"}'
    msgz = '{"z":"'+ datos[zn] + '"}'

    if (battery<="50"):
        msgbattery = '{"Battery level":"Low level"}'
    elif (battery>="50"):
        msgbattery = '{"Battery level":"High level"}'

    client.publish(topic_pub, msgx)
    client.publish(topic_pub, msgy)
    client.publish(topic_pub, msgz)
    client.publish(topic_pub, msgbattery)
    sleep(0.1)

    xn = xn+4
    yn = yn+4
    zn = zn+4
    bn = bn+4
    n = n+1
