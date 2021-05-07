

esp-idf based firmware that allows to control lamp over the network

## Setup
to setup the frimware device should be turned on with config button/jumper
when device is turned on esp smart config app can be used to pass the credentials to the device.

## Control
to change the channel brightness call `PUT http://{ip}/channels?channel={channel number, int8}&duty={duty int10}

EXAMPLE: PUT http://192.168.1.235/channels?channel=2&duty=1023

## OTA

`curl 192.168.1.235:1234 --data-binary "@.pio/build/lamp/firmware.bin"`

## dump flashing image

`esptool --baud 460800 read_flash 0x0 0x400000 ./test.bin`

### fix for io 20

At least v4.1 does not take into account that ESP32 chips can have an usable GPIO20 yet. We'll see if we can add/backport this. For now, you may be able to make this change to make it work: in esp-idf/components/soc/soc/esp32/gpio_periph.c, search for 'IO_MUX_GPIO19_REG' and change the line under it from '0,'to 'IO_MUX_GPIO20_REG,'. 


## references 
 project uses https://github.com/UncleRus/esp-idf-lib.git
 
## TODO 

- think about conifg mode
- enter config mode if there is not wifi config.
- blink until config received.
