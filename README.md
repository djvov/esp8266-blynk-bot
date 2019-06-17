- [ESP8266 and BLYNK 4WD Robot](#esp8266-and-blynk-4wd-robot)
  * [PINOUT](#pinout)
  * [First firmware update of ESP8266](#first-firmware-update-of-esp8266)
  * [ESP8266 to breadboard](#esp8266-to-breadboard)
  * [Full Schema](#full-schema)
  * [POWER](#power)
  * [MOTOR](#motor)
  * [CONNECTION](#connection)
  * [BLYNK APP](#blynk-app)
  * [Robot setup](#robot-setup)

# ESP8266 and BLYNK 4WD Robot

![](https://github.com/djvov/esp8266-blynk-bot/blob/master/real/small/vk_sYRZaswefbQ.jpg?raw=true)

[https://github.com/djvov/esp8266-blynk-bot/blob/master/real/vk_sYRZaswefbQ.jpg?raw=true]

[![Watch the video](https://github.com/djvov/esp8266-blynk-bot/blob/master/real/small/youtube_screenshot.jpg?raw=true)](https://www.youtube.com/watch?v=UbAJtE57SPo)

## PINOUT

![](https://github.com/djvov/esp8266-blynk-bot/blob/master/schema/ESP8266_12Epinout.jpg?raw=true)

## First firmware update of ESP8266

In Arduino IDE setup you need to put "http://arduino.esp8266.com/stable/package_esp8266com_index.json" at the line "Extend links to board manager".
Then go to the Board Manager and install ESP8266 community.
In the board menu choose your chip, my is Generic 8266.
Then your need to put esp8266 and FTDI programmator together as shown below.

![](https://github.com/djvov/esp8266-blynk-bot/blob/master/schema/esp2866%20first.jpg?raw=true)

Open Arduino OTA sketch from examples and write it.

Because in this robot I took all PINs to some tasks, we need to write firmware Over The Air. And first step is load OTA firmware to esp8266.

## ESP8266 to breadboard

I'll buy native esp8266 chip and white development board.

![](https://github.com/djvov/esp8266-blynk-bot/blob/master/real/small/esp8266_breakout_board.jpg?raw=true)

In the Fritzing schema I didn't find white pcb, so I put what I found, some black.
But, as white pcb, It's has some width as breadboard width. So we need to extend it by wire to another row. From 1 row to 9 row, from 2 row to 10 row, etc.

![](https://github.com/djvov/esp8266-blynk-bot/blob/master/schema/esp8266%20bread1.jpg?raw=true)

The yellow lines just to show hot to each  pin goes to its row after pcb.

![](https://github.com/djvov/esp8266-blynk-bot/blob/master/schema/esp8266%20bread2.jpg?raw=true)

## Full Schema

![](https://raw.githubusercontent.com/djvov/esp8266-blynk-bot/master/schema/robot_bb_small.jpg?raw=true)

[https://github.com/djvov/esp8266-blynk-bot/blob/master/schema/robot_bb.jpg?raw=true]


## POWER

First of all, two 18650 with serial connection to get 8.4v. With BMS plate to prevent overdischarge, and charge it with balance.
From battery block we need to build TWO special power lines: 3.3v for ESP8266, 5v for servo and HC–SR04 sonar. And raw 8.4v power for motor drivers.
3.3v we get from LM1117DT-3.3, 5v we get from LM7805. In both case two ceramic 104 capacitor for IN and OUT to ground.
**IMPORTANT!** It is necessary to connect **ALL GROUNDS**

![](https://github.com/djvov/esp8266-blynk-bot/blob/master/real/small/vk_VSyZD3ZBPP4.jpg?raw=true)

## MOTOR

I use MX1508 dual motors driver, analog of L298N. 1.5A per channel, 2.5A peak. With no heat sink. Becarefurl with VCC **+** and **-**.

![](https://github.com/djvov/esp8266-blynk-bot/blob/master/schema/MX1508motor.jpg?raw=true)

I use two MX1508 dual motors drivers, becase  I have four wheels. One MX1508 drives two motors from same side. So we need to put IN1 and IN3 together, also IN2 and IN4, for same direction of two motors. This schema can use only two pins of ESP8266 instead of four pins.

Here is table of signal for direction, standby and brakes. Standby different from brake. Standby ends of power to motor, and motor stops by itself, it tooks some time. Brake stops motor immediately.

![](https://github.com/djvov/esp8266-blynk-bot/blob/master/schema/MX1508motorSCHEMA.jpg?raw=true)

## CONNECTION

White board for ESP8266 already has pull-up 10k resistors for CH_PD and RESET, and pull-down 10k resistor for GPIO15. If you use native ESP8622 chip you must use put this resistor manually. Also, we has to pull-up GPIO2 and GPIO0 with 10k resistors. And I put ceramic 104 capacitor between VCC and GND of ESP8266.

```c
//motors
// left
const int in1 = 12;
const int in2 = 14;
// right
const int in3 = 5;
const int in4 = 4;
```
HC–SR04 sonar use 5v power and 5v TTL logic. But it know about 3.3v TTL for **TRIG** pin (HC–SR04 input). **BUT ECHO** pin (HC–SR04 output) needs to be connect to ESP8266 through voltage divider


![](https://github.com/djvov/esp8266-blynk-bot/blob/master/schema/voltage_divider.jpg?raw=true)

```c
// HC–SR04 sonar
const int trigPin = 15;
const int echoPin = 13; // through voltage divider!
```
LED uses TX and RX pins. THATS WHY FIRST OF ALL WE FLSAH OTA sketch.

```c
// LED
const int led = 1;
const int led1 = 3;
```

Servo SG90 use 5v power and 5v TTL logic. But it know about 3.3v TTL for SG90 input.

```c
//servo
const int servoPin = 16;
```

## BLYNK APP

![](https://github.com/djvov/esp8266-blynk-bot/blob/master/blynk/small/blynk1_small.jpg?raw=true) ![](https://github.com/djvov/esp8266-blynk-bot/blob/master/blynk/small/blynk2_small.jpg?raw=true)

```
V0  slider, 0-180, decimals 0, send on release Off. Servo. Just for fun.
    In time the robot walks - servo is worked automatic.
V1-V4 push type buttons, Forward, Revers, Right, Left.
V5  switch type button. LEDs.
V6  slider, 300-600, decimals 0, send on release On. Speed.
V7  switch type button. Automatic walk.
V8  terminal
```

## Robot setup

Default speed. Slowest 300. If speed < 300, motor has not enough power to move. Fastest is 1023. But I set tup fastest is 600 because if speed > 600, robot could not stop before it crashes the wall ).

```c
// скорость 1023 max
int spd = 300;
// скорость автопргулки
int autoSpd = 300;
```

**timerStrob** is for timer that runs stroboscope effect with leds.

```c
int timerStrob;
```
Movement flags to know where the robot is going.

```c
// флаги движения
int V1_forv = 0;
int V2_back = 0;
int V3_right = 0;
int V4_left = 0;
```