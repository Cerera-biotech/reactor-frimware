

esp-idf based firmware that allows to control lamp over the network

## Setup
to setup the frimware device should be turned on with config button/jumper
when device is turned on esp smart config app can be used to pass the credentials to the device.

## Control
to change the channel brightness call `PUT http://{ip}/channels?channel={channel number, int8}&duty={duty int10}

EXAMPLE: PUT http://192.168.1.235/channels?channel=2&duty=1023

## OTA

curl ${ESP32_IP}:8032 --data-binary @- < .pio/build/lolin32/firmware.bin

## TODO 

- make it work with 10 channels 
- implement ota via web page
