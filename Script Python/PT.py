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

broker="iot.eie.ucr.ac.cr"
topic_pub='v1/devices/me/telemetry'

client = mqtt.Client()

client.username_pw_set("l4gy9675")
client.connect(broker)

battery = 1

for i in range(5):
    x = random.randrange(-100, 100)
    y = random.randrange(-100, 100)
    z = random.randrange(-100, 100)
    msgx = '{"x":"'+ str(x) + '"}'
    msgy = '{"y":"'+ str(y) + '"}'
    msgz = '{"z":"'+ str(z) + '"}'

    if battery==0:
        msgbattery = '{"Battery level":"High level"}'
    elif battery==1:
        msgbattery = '{"Battery level":"Low level"}'

    client.publish(topic_pub, msgx)
    client.publish(topic_pub, msgy)
    client.publish(topic_pub, msgz)
    client.publish(topic_pub, msgbattery)
    sleep(0.1)