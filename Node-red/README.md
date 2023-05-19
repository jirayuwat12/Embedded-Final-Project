# Node Red

## Intallation
1. make sure you have nodejs installed
2. install node-red
```bash
npm install -g --unsafe-perm node-red
```
3. run node-red
```bash
node-red
```
4. install node-red-dashboard
```bash
npm install -g node-red-dashboard
```
5. restart node-red
```bash
node-red
```
6. load flow.json to node-red (import -> clipboard -> paste flow.json -> import)
7. deploy node-red 
8. open browser and go to http://localhost:1880/ui

## Description

This node-red dashboard communicate with IoT devices by MQTT protocol. The dashboard will show the temperature and humidity from IoT devices. The user infomation also show if any RFID card is detected.
