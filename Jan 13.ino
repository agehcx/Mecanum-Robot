#include <POP32.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

#define pw 40
#define pws 25

#define xr 11
#define xl 21
#define xf 22

#define cr 9
#define cl 8
#define cf 8

#define mid 5
#define turn_r 19
#define turn_l 19
#define turn_b 40

#define R 0
#define F 1
#define L 2
#define B 3

//int refL = 0,refR = 0,refBL = 0,refBR = 0;
#define refL 1300
#define refR 1000
#define refBL 1500
#define refBR 1300

int deploy[4] = {0,0,0,0};

int count = 0;
int output = 0;

const int WHITE_REF[]    = { 78,   82,   82 };
const int BLACK_REF[]    = { 103,  72,   63 };
const int RED_REF[]      = { 151,  51,   50 };
const int GREEN_REF[]    = { 81,   97,   65 };
const int BLUE_REF[]     = { 64,   66,   117 };
const int YELLOW_REF[]   = { 124,  84,   32 };

/*float dt = 0.1;
float kp = 2.5;
float ki = 0.1;
float kd = 0.01;
float last_error = 0;
float integral = 0;*/

void doCounter() { 
  count++;
}

void reset_enc(){
  count=0;
  return count;
}

/*int update(float error, float dt) {//PID CALCULATION
    integral += error * dt;
    float derivative = (error - last_error) / dt;
    float output = kp * error + ki * integral + kd * derivative;
    last_error = error;
    output = (int)output;
    return output;
}*/

void backward(int dis){ 
    while(count<dis) {  
        motor(-pw,-pw,-pw,-pw);
        /*
        if ( analog(2) < refBL && analog(3) < refBR)
            {
              AO();
              delay(100);
              break;
            }
        else if( analog(2) < refBL && analog(3) > refBR)
            {
             motor(pw,-pw,pw,-pw);
            }
        else if( analog(2) > refBL && analog(3) < refBR)
            {
             motor(-pw,pw,-pw,pw);
            }
        */
    }
    AO();
    count=0;
}

void forward(int dis){ // FORWARD MOVEMENT
      while(count<dis){  
        motor(pw,pw,pw,pw);
        if ( analog(0) < refL && analog(1) < refR)
            {
              AO();
              delay(100);
              break;
            }
        else if( analog(0) < refL && analog(1) > refR)
            {
             motor(-pw,pw,-pw,pw);
            }
        else if( analog(0) > refL && analog(1) < refR)
            {
             motor(pw,-pw,pw,-pw);
            }
      }
      AO();
      count=0;
}

void slide_R(int dis){
        while(count<dis){
            motor(pws,-pws,-pws,pws);
            if ( analog(1) < refR || analog(3) < refBR)
            {
              AO();
              delay(100);
              break;
            }
          }
          AO(); 
          count=0;
      }

void slide_L(int dis){
        while(count<dis){
            motor(-pws,pws,pws,-pws);
            if(analog(0)<refL || analog(2)<refBL)
            {
              AO();
              delay(100);
              break;
            }
          }
          AO(); 
          count=0;
}

void turn_R(){
        while(count<(turn_r)){
           motor(pws,-pws,pws,-pws);
        }
        AO(); 
        count=0;
      }

void turn_L(){
        while(count<(turn_l)){
            motor(-pws,pws,-pws,pws);
          }
          AO(); 
          count=0;
}

void turn_B(){
    while(count<(turn_b)){
      motor(-pws,pws,-pws,pws);
      delay(10);
    }
  AO(); 
  count=0;
}  


void setR() {
  while(1) {
    if(analog(1)>refR && analog(3)>refBR){
      motor(pws,-pws,-pws,pws);
    }
    else if(analog(1)<refR && analog(3)<refBR){
      AO();
      delay(100);
      break;
    }
    else if(analog(1)<refR){
      motor(-pws,pws,-pws,pws);
    }
    else if(analog(3)<refBR){
      motor(pws,-pws,pws,-pws);
    }
  }
  count=0;
}

void setL() {
  while(1) {
    if(analog(0)>refL && analog(2)>refBL){
      motor(-pws,pws,pws,-pws);
    }
    else if(analog(0)<refL && analog(2)<refBL){
      AO();
      delay(100);
      break;
    }
    else if(analog(0)<refL){
      motor(pws,-pws,pws,-pws);
    }
    else if(analog(2)<refBL){
      motor(-pws,pws,-pws,pws);
    }
  }
  count=0;
}

void deploy(int value) {
  AO();
  delay(300);
  turn_B();
  if( (value==0) && (deployed[0]==0) ) {
    //servo(1,180);
    delay(1000);
    //servo(1,90); 
    oled.text(1,1,"RED DEPLOYED");
    oled.show();
    deployed[0] = 1;
  }
  else if( (value==1) && (deployed[1]==0) ) {
    //servo(1,0);
    delay(1000);
    //servo(1,90); 
    oled.text(1,1,"BLUE DEPLOYED");
    oled.show();
    deployed[1] = 1;
  }
  else if( (value==2) && (deployed[2]==0) ) {
    //servo(0,180);
    delay(1000);
    //servo(0,90); 
    oled.text(1,1,"GREEN DEPLOYED");
    oled.show();
    deployed[2] = 1;
  }
  else if( (value==3) && (deployed[3]==0) ) {
    //servo(0,0);
    delay(1000);
    //servo(0,90); 
    oled.text(1,1,"YELLOW DEPLOYED");
    oled.show();
    deployed[3] = 1;
  }
  /*while(SW_A()==0){AO();}*/
  count = 0;
  turn_B();
  // maybe added forward here //
}

void check_color(uint16_t _rr, uint16_t _gg, uint16_t _bb) {
  if ((_rr <= RED_REF[0] + GAP && _rr >= RED_REF[0] - GAP) &&
      (_gg <= RED_REF[1] + GAP && _gg >= RED_REF[1] - GAP) &&
      (_bb <= RED_REF[2] + GAP && _bb >= RED_REF[2] - GAP))
    {
      AO();
      oled.text(6,1,"IT'S RED !!");
      oled.show();
      deploy(0);
      delay(100);
      /*while(SW_A()==0){AO();}*/
    }
  else if ((_rr <= GREEN_REF[0] + GAP && _rr >= GREEN_REF[0] - GAP) &&
           (_gg <= GREEN_REF[1] + GAP && _gg >= GREEN_REF[1] - GAP) &&
           (_bb <= GREEN_REF[2] + GAP && _bb >= GREEN_REF[2] - GAP))
    {
      AO();
      oled.text(6,1,"IT'S GREEN !!");
      oled.show();
      deploy(1);
      delay(100);
      /*while(SW_A()==0){AO();}*/
    }
  else if ((_rr <= BLUE_REF[0] + GAP && _rr >= BLUE_REF[0] - GAP) &&
           (_gg <= BLUE_REF[1] + GAP && _gg >= BLUE_REF[1] - GAP) &&
           (_bb <= BLUE_REF[2] + GAP && _bb >= BLUE_REF[2] - GAP))
    {
      AO();
      oled.text(6,1,"IT'S BLUE !!");
      oled.show();
      deploy(2);
      delay(100);
      /*while(SW_A()==0){AO();}*/
    }
  else if ((_rr <= YELLOW_REF[0] + GAP && _rr >= YELLOW_REF[0] - GAP) &&
           (_gg <= YELLOW_REF[1] + GAP && _gg >= YELLOW_REF[1] - GAP) &&
           (_bb <= YELLOW_REF[2] + GAP && _bb >= YELLOW_REF[2] - GAP))
    {
      forward(10);
      AO();
      delay(500);
      while(1) {
        uint16_t clear, red, green, blue;
        tcs.getRawData(&red, &green, &blue, &clear);
        uint32_t sum = clear;
        float r, g, b;
        r = red;
        r /= sum;
        g = green;
        g /= sum;
        b = blue;
        b /= sum;
        r *= 256;
        g *= 256;
        b *= 256;

        if ((r <= YELLOW_REF[0] + GAP && r >= YELLOW_REF[0] - GAP) &&
            (g <= YELLOW_REF[1] + GAP && g >= YELLOW_REF[1] - GAP) &&
            (b <= YELLOW_REF[2] + GAP && b >= YELLOW_REF[2] - GAP))
            {
              AO();
              oled.text(6,1,"IT'S YELLOW !!");
              oled.show();
              deploy(3);
              count = 0;
              delay(100);
            }                    
            else {
              AO();
              delay(100);
              break;
            }
        }
      }
}

void setup() {
  pinMode(22, INPUT);
  attachInterrupt(22, doCounter, HIGH);
  /* Set all servo to 90 and turn it off 
  servo(0,90);
  servo(1,90);
  servo(2,90);
  servo(0,-1);
  servo(1,-1);
  servo(2,-1);
  */
  
  while(SW_A()==0){
    AO();
  }

  // refL = (analog(0)*3)/5; refR = (analog(1)*3)/5; 
  // refBL = (analog(2)*3)/5; refBR = (analog(3)*3)/5;

  delay(500);
  while ( (deploy[0]!= 1) || (deploy[1]!=1) || (deploy[2]!=1) || (deploy[3]!=1) ) {

    // Get RGB value //
    uint16_t clear, red, green, blue;
    tcs.getRawData(&red, &green, &blue, &clear);
    uint32_t sum = clear;
    float r, g, b;
    r = red;
    r /= sum;
    g = green;
    g /= sum;
    b = blue;
    b /= sum;
    r *= 256;
    g *= 256;
    b *= 256;

    slide_R(xr);
    if(analog(1) > refR && analog(3) > refBR) { 
      oled.text(1,1,"Can go R" );
      oled.show();
      slide_R(19);
      delay(100); 
      turn_R();
      // check_color((uint16_t)r,(uint16_t)g,(uint16_t)b);
      continue;
    }
    if(analog(1)<refR || analog(3)<refBR) {
      oled.text(1,1,"Cant go R");
      oled.show();
      delay(100);
      slide_L(mid);
      setR();
      slide_L(mid);
      forward(cf);
      if(analog(0)>refL && analog(1)>refR){
        oled.text(1,1,"Can go F");
        oled.show();
        forward(xf-3);
        // check_color((uint16_t)r,(uint16_t)g,(uint16_t)b);
        continue;
      }
      else if(analog(0)<refL || analog(1)<refR){
        oled.text(1,1,"Cant go F");
        oled.show();
        sound(200,200);
        backward(4);
        delay(50);
        slide_L(cl);
        if(analog(0)>refL && analog(2)>refBL){
          slide_L(xl);
          oled.clearDisplay();
          oled.text(1,1,"count=%d",count);
          oled.show();          
          turn_L();
          // check_color((uint16_t)r,(uint16_t)g,(uint16_t)b);      
          continue;
        }
        else if(analog(0)<refL || analog(2)< refBL){
          sound(200,200);
          delay(100);
          slide_R(mid);
          delay(100);
          turn_B();
          delay(100);
          forward(30);
          // check_color((uint16_t)r,(uint16_t)g,(uint16_t)b);
          continue;
        }
      }
    }
  }
  // servo(2,0);
  oled.text(5,1,"Mission complete!");
  oled.show();

}

void loop() {
  // put your main code here, to run repeatedly:
}
