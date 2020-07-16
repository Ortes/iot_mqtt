# IOT Project

## Install

### ESP32
First refer to [Espressif documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) to install ESP32 SDK
After when the shell has been setup you can run the following:
```shell script
git clone git@github.com:Ortes/iot_mqtt.git
cd iot_mqtt
idf.py flash && idf.py monitor
```

### Server

#### Ubuntu
The port 1883 must be open.
```shell script
sudo apt install mosquitto
mosquitto
```
