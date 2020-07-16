# IOT Project MQTT

## Install

### ESP32
First refer to [Espressif documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) to install ESP32 SDK  
After setting up the shell you can run the following:
```shell script
git clone git@github.com:Ortes/iot_mqtt.git
cd iot_mqtt
idf.py flash && idf.py monitor
```
MQTT topics are:
- esp/temp topic where temperature is published as a string
- esp/hum topic where temperature is published as a string
- esp/led topic where led state is published `'0'` is off `'1'` is on

### Server

#### Mosquitto
##### Ubuntu
The port 1883 must be open.
```shell script
sudo apt install mosquitto
mosquitto
```

#### NodeRed
The port 1880 must be open.
```shell script
docker run -it -p 1880:1880 --name mynodered nodered/node-red
```
Given ip_address the ip address of the server where NodeRed is installed.  
To setup the flow open ip_address:1880 then clic on import and import copy and paste the json in nodered/flow-nodered.json

