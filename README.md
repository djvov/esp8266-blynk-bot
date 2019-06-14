[TOC]

# ESP8266 and BLYNK 4WD Robot

![](https://github.com/djvov/esp8266-blynk-bot/blob/master/real/small/vk_sYRZaswefbQ.jpg?raw=true)

[https://github.com/djvov/esp8266-blynk-bot/blob/master/real/vk_sYRZaswefbQ.jpg?raw=true]

## First firmware update of ESP8266

In Arduino IDE setup you need to put "http://arduino.esp8266.com/stable/package_esp8266com_index.json" at the line "Extend links to board manager".
Then go to the Board Manager and install ESP8266 community.
In the board menu choose your chip, my is Generic 8266.
Then your need to put esp8266 and FTDI programmator together as shown below.

![](https://github.com/djvov/esp8266-blynk-bot/blob/master/schema/esp2866%20first.jpg?raw=true)

Open Arduino OTA sketch from examples and write it.

Because in this robot I took all PINs to some tasks, we need to write firmware Over The Air. And first step is load OTA firmware to esp8266.

## ESP8266 to breadboard

I'll buy native esp8266 chip and white development borad. In the Fritzing schema I didn't find white pcb, so I put what I found, some black.
But, as white pcb, It's has some width as breadboard width. So we need to extend it by wire to another row. From 1 row to 9 row, from 2 row to 10 row, etc.

![](https://github.com/djvov/esp8266-blynk-bot/blob/master/schema/esp8266%20bread1.jpg?raw=true)

The yellow lines just to show hot to each  pin goes to its row after pcb.

![](https://github.com/djvov/esp8266-blynk-bot/blob/master/schema/esp8266%20bread2.jpg?raw=true)

## Full Schema

![](https://raw.githubusercontent.com/djvov/esp8266-blynk-bot/master/schema/esp_robot.jpg)

### POWER

First of all, two 18650 with serial connection to get 8.4v. With BMS plate to prevent overdischarge, and charge it with balance.
From battery block we need to build TWO special power lines: 3.3v for ESP8266, 5v for servo and HC–SR04 sonar. And raw 8.4v power for motor drivers.
3.3v we get from LM1117DT-3.3, 5v we get from LM7805. In both case two ceramic 104 capacitor for IN and OUT to ground.
**IMPORTANT!** It is necessary to connect **ALL GROUNDS**