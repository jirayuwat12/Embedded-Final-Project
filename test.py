# connect mqtt to localhost and subscribe to topic

import paho.mqtt.client as mqtt
import time
import random
# The callback for when the client receives a CONNACK response from the server.


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("WaterAmount")


def on_publish(client, userdata, mid):
    return 

def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))


client = mqtt.Client('python300')
client.on_message = on_message
client.on_publish = on_publish
client.connect("34.126.157.245", 1883, 60)

client.loop_start()
temp = random.randint(1, 100)
humid = random.randint(1, 100)
while True:
    id = str(random.randint(0, 2))
    client.publish("RFID",'0'*(10-len(id)) + id)
    temp = temp + random.randint(-1, 1)
    client.publish("Temperature",str(temp))
    humid = humid + random.randint(-1, 1)
    client.publish("Humidity",str(humid))
    print("Message Sent")
    time.sleep(5)
