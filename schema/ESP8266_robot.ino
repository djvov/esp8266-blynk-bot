/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP8266 chip.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
// #define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>

#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <BlynkSimpleEsp8266.h>
#include <Servo.h>


// You should get Auth Token in the Blynk App.
char auth[] = "daba40ff91354db085800bb97092ec8b";

// Your WiFi credentials.
char ssid[] = "djvov";
char pass[] = "g0tb44r?";

// использованные пины 1 3 5 4 15      13 12 14 16

// моторы
const int in1 = 12;
const int in2 = 14;

const int in3 = 5;
const int in4 = 4;

// скорость 1023 max
int spd = 300; 

// датчик расстояние
const int trigPin = 15;
const int echoPin = 13; // через делитель!

// фара
const int led = 3;
const int led1 = 1;
int strob = 0;
int timerStrob;

// флаги движения
int V1_forv = 0;
int V2_back = 0;
int V3_right = 0;
int V4_left = 0;

// авто прогулка
int autO = 0;

// расстояние
int duration, cm;


// для автопрогулки

// Углы поворота Серовпривода
int angleF = 90;
int angleR = 50;
int angleL = 140;
// дистанции остановки
int distSTOP = 30; // дистанция СТОП!
int distCaution = 50; // дистанция ОСТОРОЖНО! скорость 300 
int distAttention = 70; // дистанция ВНИМАНИЕ! скорость 400
//  Задача переменных Сонара
int distanceSonar;
int distanceSonarSON_F = 0; // дистанция СОНАР впереди
int distanceSonarSON_R = 0; // дистанция СОНАР справа
int distanceSonarSON_L = 0; // дистанция СОНАР слева
int delay_time = 150; // время задержки для стабилизации сервопривода
int angle = 0;
// скорость автопргулки
int autoSpd = 300;

//таймер запуска датчика расстояния
BlynkTimer timer;
int timerStena;
//таймер автопрогулки
int timerAuto;

//servo
const int servoPin = 16;
int srv = 0;
Servo servo1;

WidgetTerminal terminal(V8);

void strobo() {
  digitalWrite(led, HIGH);
  digitalWrite(led1, LOW);
  delay(100);
  digitalWrite(led, LOW);
  digitalWrite(led1, HIGH);  
}

void stp(int f) { // стоп
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  terminal.println(String("stop"));
  terminal.flush();
  timer.disable(timerStena);
  delay(f);
}

void checkrasst() { // проверка расстояния в ручном режиме поездки и остановка если мало
  int cm = distanceMeasuringSonar (angleF);  
  if (cm<distSTOP) {
    stp(0);
    servo1.write(70);
    delay(500);
    servo1.write(110);
    delay(500);
    servo1.write(90);
  } 
}

void go_forv(int a, int sa) {
  int spd1 = 0;
  if (autO==1) spd1=sa;
  else spd1=spd;
  if (autO==0) {
    timer.enable(timerStena);
  }
  analogWrite(in1, spd1);
  digitalWrite(in2, LOW);
  analogWrite(in3, spd1);
  digitalWrite(in4, LOW);  
  terminal.println(String("go_forv"));
  terminal.flush();
  
  if (autO==1) {
    delay (a);
  }
}

void go_back(int g, int sg) {
  int spd1 = 0;
  if (autO==1) spd1=sg;
  else spd1=spd;
  digitalWrite(in1, LOW);
  analogWrite(in2, spd1);
  digitalWrite(in3, LOW);
  analogWrite(in4, spd1);
  terminal.println(String("go_back"));
  terminal.flush();
  if (autO==1) {
    delay (g);
  }
}

void go_left(int e, int se) {
  int spd1 = 0;
  if (autO==1) spd1=se;
  else spd1=spd;
  digitalWrite(in1, LOW);
  analogWrite(in2, spd1);
  analogWrite(in3, spd1);
  digitalWrite(in4, LOW);
  terminal.println(String("go_left"));
  terminal.flush();
  if (autO==1) {
    delay (e);
  }
}

void go_right(int d, int sd) {
  int spd1 = 0;
  if (autO==1) spd1=sd;
  else spd1=spd;
  analogWrite(in1, spd1);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  analogWrite(in4, spd1);
  terminal.println(String("go_right"));
  terminal.flush();
  if (autO==1) {
    delay (d);
  }
}

// *************** Функция определения Расстояния СОНАР ******************
int distanceMeasuringSonar (int angle)// функция — измерение расстояния при определенном угле поворота сервопривода
{
  servo1.write(angle); // поворот сервопривода 1
  delay (delay_time); // подождать стабилизацию сервопривода
  digitalWrite (trigPin, LOW); // подать напряжение на сонар LOW в течении 2¦Мs
  delayMicroseconds (2);
  digitalWrite (trigPin, HIGH); // подать напряжение HIGH на сонар в течении 10¦Мs
  delayMicroseconds (10);
  digitalWrite (trigPin, LOW); // подать напряжение LOW на сонар
  int distance = pulseIn(echoPin, HIGH, 30000); // считываем данные с Сонара
  if (distance == 0)// если расстояние "0" — ошибка измерения (вне предела/завис)
  { // делаем перезагрузку сонара подачей 0 на вывод ECHO Сонара
    pinMode(echoPin, OUTPUT);
    digitalWrite(echoPin, 0);
    pinMode(echoPin, INPUT);
    // в случае ошибки измерения выводим расстояние "400 см" — расстояние вне предела измерения датчика
    distance = 400;
  } else {
    distanceSonar = distance / 58; // переводим "время" в сантиметры
  }
  terminal.print(String("DISTANCE "));
  terminal.println(distanceSonar);
  terminal.flush();
  return (distanceSonar); // возвращаем расстояние
}


void go_auto() {
  if (autO==1){
    distanceSonarSON_F = distanceMeasuringSonar (angleF); // запускаем функцию измерения расстояния сонаром
    
    // логика — если растояние меньше СТОП!
    if (distanceSonarSON_F < distSTOP) // если расстояние впереди меньше distSTOP — двигаемся назад с разворотом // stop back
    {
      stp(50); // стоп
      go_back(100, autoSpd); // двигаться назад (время, скорость)
      go_left(50, autoSpd); // поворот (время, скорость)
    }
    // логика — если растояние больше ОСТОРОЖНО
    if (distanceSonarSON_F > distCaution && distAttention > distanceSonarSON_F) // если расстояние впереди больше distCaution но меньше distAttention — двигаться ВПЕРЕД с низкой скоростьюю // forv low
    {
      go_forv (20, autoSpd); // normal Forward (время, скорость)     
    }
    // логика — если растояние больше ВНИМАНИЕ
    if (distanceSonarSON_F > distAttention) // если расстояние впереди больше distAttention — двигаться ВПЕРЕД с высокой скоростью // forv high
    {
      go_forv (50, 400); // normal Forward (время, скорость)
    }
    
    // измеряем расстояние СПРАВА
    distanceSonarSON_R = distanceMeasuringSonar (angleR); // запускаем функцию измерения расстояния сонаром
    
    if (distanceSonarSON_R < distCaution) // если расстояние впереди меньше
    {
      go_left (60, autoSpd); // поворот (время, скорость)
    }
    
    // измеряем расстояние ПРЯМО
    distanceSonarSON_F = distanceMeasuringSonar (angleF); // запускаем функцию измерения расстояния сонаром
    
    // логика — если растояние меньше СТОП!
    if (distanceSonarSON_F < distSTOP) // если расстояние впереди меньше distSTOP — двигаемся назад с разворотом
    {
      stp (50); // стоп
      go_back (100, autoSpd); // двигаться назад (время, скорость)
      go_left (20, autoSpd); // поворот (время, скорость)
    }
    // логика — если растояние больше ОСТОРОЖНО
    if (distanceSonarSON_F > distCaution && distAttention > distanceSonarSON_F) // если расстояние впереди больше distCaution но меньше distAttention — двигаться ВПЕРЕД с низкой скоростью
    {
      go_forv (20, autoSpd); // normal Forward (время, скорость)
    }
    // логика — если растояние больше ВНИМАНИЕ
    if (distanceSonarSON_F > distAttention) // если расстояние впереди больше distAttention — двигаться ВПЕРЕД с высокой скоростью
    {
      go_forv (50, 400); // normal Forward (время, скорость)
    }
    // измеряем расстояние СЛЕВА
    distanceSonarSON_L = distanceMeasuringSonar (angleL); // запускаем функцию измерения расстояния сонаром    
    if (distanceSonarSON_L < distCaution) // если расстояние впереди меньше distCaution
    {
      go_left (50, autoSpd); // поворот (время, скорость)
    }
  } else {
    stp(0);
  }
}

BLYNK_WRITE(V5) // ручной режим зажечь фару
{
  int snd = param.asInt(); // assigning incoming value from pin V5 to a variable
  if (snd == 1) {
    timer.enable(timerStrob);
    terminal.println(String("Strobo ON"));  
    terminal.flush();
  } else {
    timer.disable(timerStrob);
    terminal.println(String("Strobo OFF"));  
    terminal.flush();
    digitalWrite(led, LOW);
    digitalWrite(led1, LOW);  
  }
}

BLYNK_WRITE(V0)
{
  srv = param.asInt(); // ручной режим покрутить головой
  terminal.println(String("Servo ") + String(srv));
  terminal.flush();
  servo1.write(srv);
}

BLYNK_WRITE(V7)
{
  autO = param.asInt(); // авто прогулка
  if (autO==1) {
    timer.enable(timerAuto);
    timer.enable(timerStrob);
    terminal.println(String("AUTO ON"));  
    terminal.flush();
  } else {
    stp(0);
    terminal.println(String("AUTO OFF"));  
    terminal.flush();
    timer.disable(timerAuto);
    timer.disable(timerStrob);
    digitalWrite(led, LOW);
    digitalWrite(led1, LOW);  
  }
  
}

BLYNK_WRITE(V6)
{
  spd = param.asInt(); // ручной режим установка скорости
  terminal.println(String("Speed ") + String(spd));
  terminal.flush();
}

BLYNK_WRITE(V1)
{
  V1_forv = param.asInt(); // ручной режим вперед 
  if (V1_forv == 1) {
    go_forv(0,spd);
  }
  if (V1_forv == 0 && V2_back == 0 && V4_left == 0 && V3_right==0) {
    stp(0);
  }
}

BLYNK_WRITE(V2)
{
  V2_back = param.asInt(); // ручной режим назад
  if (V2_back == 1) {
    go_back(0,spd);
  }
  if (V1_forv == 0 && V2_back == 0 && V4_left == 0 && V3_right==0) {
    stp(0);
  }
}

BLYNK_WRITE(V4) // ручной режим влево
{
  V4_left = param.asInt(); // assigning incoming value from pin V4 to a variable  
  if (V4_left == 1) {
    go_left(0,spd);
  }
  if (V1_forv == 0 && V2_back == 0 && V4_left == 0 && V3_right==0) {
    stp(0);
  }
}

BLYNK_WRITE(V3) // ручной режим вправо
{
  V3_right = param.asInt(); 
  if (V3_right == 1) {
    go_right(0,spd);
  }
  if (V1_forv == 0 && V2_back == 0 && V4_left == 0 && V3_right==0) {
    stp(0);
  }
}

void setup()
{ 
  // моторы
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  stp(0);
  
  // фара
  pinMode(led, OUTPUT);
  pinMode(led1, OUTPUT);
  digitalWrite(led, LOW);
  digitalWrite(led1, LOW);
  
  //датчик расстояния
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // увидел ли стену в ручном режиме
  timerStena = timer.setInterval(13L, checkrasst);
  timer.disable(timerStena);
  
  // автопрогулка
  timerAuto = timer.setInterval(1L, go_auto);
  timer.disable(timerAuto);

  timerStrob = timer.setInterval(200L, strobo);
  timer.disable(timerStrob);
  
  // задаем серву
  servo1.attach(servoPin);
  servo1.write(90);
  // по воздуху
  ArduinoOTA.begin(); 
  Blynk.begin(auth, ssid, pass);
  
  terminal.clear();
  terminal.println(String("Speed ") + String(spd));
  terminal.println(String("Servo 90"));
  terminal.print(String("Local IP"));  
  terminal.println(WiFi.localIP());
  terminal.flush();
}

void loop()
{
  ArduinoOTA.handle();
  Blynk.run();
  timer.run();
}
