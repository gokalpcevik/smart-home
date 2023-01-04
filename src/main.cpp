#include <Arduino.h>
#include <Servo.h>
#include <SoftPWM.h>

#include "Enumeration.h"
#include "HomePinDefinitions.h"

Servo door;
Servo window;
uint8_t buffer[8];
bool smartlightOn = false;
bool sunlightOn = false;

bool bedroomPowerOn = false;
bool livingroomPowerOn = false;

uint32_t bedroom_brightness = 100;
uint32_t livingroom_brightness = 100;

int light = 0;

void setup() 
{
  SoftPWMBegin();

  // put your setup code here, to run once:
  Serial.begin(115200);

  SoftPWMSet(LIVING_ROOM_LED2_PIN,0);
  SoftPWMSetFadeTime(13,100,500);

  pinMode(LIVING_ROOM_LED0_PIN,OUTPUT);
  pinMode(LIVING_ROOM_LED1_PIN,OUTPUT);
  pinMode(BED_ROOM_LED0_PIN,OUTPUT);

  pinMode(RGB_LED_R_PIN,OUTPUT);
  pinMode(RGB_LED_G_PIN,OUTPUT);
  pinMode(RGB_LED_B_PIN,OUTPUT);

  door.attach(DOOR_SERVO_PWM_PIN);
  window.attach(WINDOW_SERVO_PWM_PIN);
}


void WriteRGB(uint8_t r,uint8_t g,uint8_t b)
{
  analogWrite(RGB_LED_R_PIN,r);
  analogWrite(RGB_LED_G_PIN,g);
  analogWrite(RGB_LED_B_PIN,b);
}

void loop2()
{
  //door.write(DOOR_CLOSED_ANGLE);
  //delay(1000);
  //door.write(DOOR_OPENED_ANGLE);
  //delay(1000);

  //window.write(WINDOW_CLOSED_ANGLE);
  //delay(1000);
  //window.write(WINDOW_OPENED_ANGLE);
  //delay(1000);


  //analogWrite(RGB_LED_R_PIN,255);
  //analogWrite(RGB_LED_G_PIN,0);
  //analogWrite(RGB_LED_B_PIN,255);
  //
  //analogWrite(LIVING_ROOM_LED0_PIN,255);
  //analogWrite(LIVING_ROOM_LED1_PIN,255);
  //analogWrite(LIVING_ROOM_LED2_PIN,255);



  //delay(1000);


  //Serial.println(window.read());

}

void serialFlush(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}

void loop() 
{

  if(sunlightOn)
  {
    int sensor = analogRead(A0);

    if(sensor > 80)
    {
        livingroomPowerOn = true;
        analogWrite(LIVING_ROOM_LED0_PIN,(int)livingroomPowerOn*(int)((float)livingroom_brightness*2.55f));
        analogWrite(LIVING_ROOM_LED1_PIN,(int)livingroomPowerOn*(int)((float)livingroom_brightness*2.55f));
        SoftPWMSetPercent(LIVING_ROOM_LED2_PIN,(int)livingroomPowerOn*(int)livingroom_brightness);
    }
    else
    {
        livingroomPowerOn = false;
        analogWrite(LIVING_ROOM_LED0_PIN,(int)livingroomPowerOn*(int)((float)livingroom_brightness*2.55f));
        analogWrite(LIVING_ROOM_LED1_PIN,(int)livingroomPowerOn*(int)((float)livingroom_brightness*2.55f));
        SoftPWMSetPercent(LIVING_ROOM_LED2_PIN,(int)livingroomPowerOn*(int)livingroom_brightness);
    }
  }


  if(Serial.available() > 0)
  {
    size_t readBytes = Serial.readBytes(buffer,8);
    if(readBytes != 8) return;

    serialFlush();

    Command command = static_cast<Command>(buffer[0]);
    Room room = static_cast<Room>(buffer[1]);

    switch(command)
    {
      case CMD_POWER:
      {
        if(sunlightOn) return;

        uint8_t powerState = buffer[2];
        if(room==BED_ROOM) 
        {
          bedroomPowerOn = powerState;
          analogWrite(BED_ROOM_LED0_PIN,(int)powerState*(int)((float)bedroom_brightness*2.55f));
        }
        else if(room==LIVING_ROOM) 
        {
          livingroomPowerOn = powerState;
          analogWrite(LIVING_ROOM_LED0_PIN,(int)powerState*(int)((float)livingroom_brightness*2.55f));
          analogWrite(LIVING_ROOM_LED1_PIN,(int)powerState*(int)((float)livingroom_brightness*2.55f));
          SoftPWMSetPercent(LIVING_ROOM_LED2_PIN,(int)livingroomPowerOn*(int)livingroom_brightness);
        }
        break;
      }

      case CMD_SUNLIGHT:
      {
        uint8_t sunlightState = buffer[1];
        sunlightOn = (bool)sunlightState;
        break;
      }

      case CMD_DOOR:
      {
        uint8_t doorState = buffer[1];
        if(doorState)
        {
          // Open the door smoothly here.
          door.write(DOOR_OPENED_ANGLE);
        }
        else
        {
          // Close the door smoothly here.
          door.write(DOOR_CLOSED_ANGLE);
        }

        break;
      }

      case CMD_WINDOW:
      {
        uint8_t windowState = buffer[1];
        if(windowState)
        {
          // Open the window smoothly here.
          window.write(WINDOW_OPENED_ANGLE);
        }
        else
        {
          // Close the window smoothly here.
          window.write(WINDOW_CLOSED_ANGLE);
        }
        break;

      }
      case CMD_SELECT_COLOR:
      {
        COLOR color = static_cast<COLOR>(buffer[1]);
        if(color == Red) WriteRGB(255,0,0);
        else if(color == Green) WriteRGB(0,255,0); 
        else if(color == Blue) WriteRGB(0,0,255); 
        else if(color == Cyan) WriteRGB(0,255,255); 
        break;
      }
      case CMD_BRIGHTNESS:
      {
        if(sunlightOn) return;

        uint32_t brightness = (uint32_t)buffer[2] | (uint32_t)buffer[3] << 8 | (uint32_t)buffer[4] << 16 | (uint32_t)buffer[5] << 24;

        if(room == BED_ROOM) 
        {
          bedroom_brightness = brightness;
          analogWrite(BED_ROOM_LED0_PIN,(int)bedroomPowerOn*(int)((float)bedroom_brightness*2.55f));
        }
        else if(room == LIVING_ROOM) 
        {
          livingroom_brightness = brightness;
          SoftPWMSetPercent(LIVING_ROOM_LED2_PIN,(int)livingroomPowerOn*(int)livingroom_brightness);
          analogWrite(LIVING_ROOM_LED0_PIN,(int)livingroomPowerOn*(int)((float)livingroom_brightness*2.55f));
          analogWrite(LIVING_ROOM_LED1_PIN,(int)livingroomPowerOn*(int)((float)livingroom_brightness*2.55f));
        }
        break;
      }
    }
  }
}