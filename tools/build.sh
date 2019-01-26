#!/bin/bash
./mkspiffs.exe -c ../webapp/build -b 4096 -p 256 -s 0x100000 ../build/spiffs.bin
$IDF_PATH/components/esptool_py/esptool/esptool.py --chip esp32 --port com5 --baud 115200 write_flash -z 0x110000 ../build/spiffs.bin

