# led_test_client.py
# Test client for erpc led server example
# Author: becksteing/Jing-Jia Liou
# Date: 02/13/2022
# Blinks LEDs on a connected Mbed-enabled board running the erpc LED server example
import paho.mqtt.client as paho
import time
import serial
import numpy as np
import matplotlib.pyplot as plt


mqttc = paho.Client()

host = "192.168.28.185"
topic = "Mbed"
sample = np.zeros(50)
t = np.arange(0,5,0.1)
counter = 0

def on_connect(self, mosq, obj, rc):
    print("Connected rc: " + str(rc))

def on_message(mosq, obj, msg):
    print("[Received] Topic: " + msg.topic + ", Message: " + str(msg.payload) + "\n")
    if counter % 4 == 0:
        sample[counter % 4] = [float(x) for x in str(msg.payload).split(', ')]
    if counter < 50:
        counter = counter + 1

def on_subscribe(mosq, obj, mid, granted_qos):
    print("Subscribed OK")

def on_unsubscribe(mosq, obj, mid, granted_qos):
    print("Unsubscribed OK")

mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_subscribe = on_subscribe
mqttc.on_unsubscribe = on_unsubscribe

while(counter<=50):
    mqttc.loop()

fig, ax = plt.subplots(2, 1)
ax[0].plot(t,sample[0])
ax[0].set_xlabel('Time')
ax[0].set_ylabel('Coordinate X')
ax[1].plot(t,sample[1])
ax[1].set_xlabel('Time')
ax[1].set_ylabel('Coordinate Y')
ax[2].plot(t,sample[2])
ax[2].set_xlabel('Time')
ax[2].set_ylabel('Coordinate Z')
plt.show()

