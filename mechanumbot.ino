#include <POP32.h>
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


int direction[4] = {0,0,0,0};
int deployed[4] = {0,0,0,0};
int cango = 0;

int count = 0;
int output = 0;

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

void setup() {
  pinMode(22, INPUT);
  attachInterrupt(22, doCounter, HIGH);
  // put your setup code here, to run once:
  while(SW_A()==0){
    AO();
  }
  delay(100);
  // refL = (analog(0)*3)/5; refR = (analog(1)*3)/5; 
  // refBL = (analog(2)*3)/5; refBR = (analog(3)*3)/5;

  delay(500);
  while(1){
    slide_R(xr);
    if(analog(1) > refR && analog(3) > refBR) { 
      oled.text(1,1,"Can go R" );
      oled.show();
      slide_R(19);
      delay(100); 
      turn_R();
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
          continue;
        }
      }
    }
  }
   
  // oled.text(1,1,"Completed");
  // oled.show();

}

void loop() {
  // put your main code here, to run repeatedly:
}
