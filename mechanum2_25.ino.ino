#include <I2Cdev.h>


#include <MPU6050.h>

#include <POP32.h>
#include <Wire.h>

#define pw 40
#define pws 25

#define xr 17
#define xl 17
#define xf 24

#define cr 11
#define cl 10
#define cf 10

#define mid 6
#define turn_r 19
#define turn_l 19
#define turn_b 39

#define R 0
#define F 1
#define L 2
#define B 3

//int refL = 0,refR = 0,refBL = 0,refBR = 0;
#define refL 600
#define refR 1800
#define refBL 1200
#define refBR 1300

MPU6050 mpu; //ประกาศตัวแปร mpu
int16_t ax, ay, az;
int16_t gx, gy, gz;
int valgx,valgy,valgz;
int valax,valay,valaz;

int direction[4] = {0,0,0,0};
int deployed[4] = {0,0,0,0};
int cango = 0;

int count = 0;
int output = 0;

int finish = 0;

float dt = 0.1;
float kp = 2.5;
float ki = 0.1;
float kd = 0.01;
float last_error = 0;
float integral = 0;

void doCounter() { 
  count++;
}

void reset_enc(){
  count=0;
  return count;
}

int update(float error, float dt) {//PID CALCULATION
    integral += error * dt;
    float derivative = (error - last_error) / dt;
    float output = kp * error + ki * integral + kd * derivative;
    last_error = error;
    output = (int)output;
    return output;
}

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

void forward(int power,int dis){ // FORWARD MOVEMENT
      while(count<dis){  
        motor(power,power,power,power);
        if ( analog(0) < refL || analog(1) < refR)
            {
              AO();
              delay(100);
              break;
            }
        else if( analog(0) < refL && analog(1) > refR)
            {
             motor(-power,power,-power,power);
            }
        else if( analog(0) > refL && analog(1) < refR)
            {
             motor(power,-power,power,-power);
            }
      }
      AO();
      count=0;
}

void slide_R(int dis){
        while(count<dis){
            motor(pws,-pws,-pws,pws);
            if ( analog(1) < refR && analog(3) < refBR)
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
            if(analog(0)<refL && analog(2)<refBL)
            {
              AO();
              delay(100);
              break;
            }
          }
          AO(); 
          count=0;
}

void turn_B(){
    while(count<(turn_b)){
           motor(pws,-pws,pws,-pws);
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

void turn_R(){
        while(count<(turn_r)){
           motor(pws,-pws,pws,-pws);
           delay(50);           
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

void check_baricade(){
        AO();delay(500);
        count=0;        
        while(count<=13){
            mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
            //motor(pw,pw,pw,pw);  
            valgy = map(gy, -18000, 18000, 0, 180);
            motor(20,20,20,20);
            if(valgy>=103){
              AO();delay(200);break;
            }
            
        }
          int dist = map(analog(4),760,2900,30,0);
          oled.text(5,1,"vision = %d",dist);
          AO();delay(100);
          if(dist<2){
                count=0;
                while(count<56){motor(pw,pw,pw,pw);}
                AO();
                sound(200,200);
                while(count<13){
                  if(analog(0)<700 || analog(1)<1100){
                    AO();
                  }
                }
                while(1){ao();}                
            }
      AO();
}

void setup() {
  pinMode(22, INPUT);
  attachInterrupt(22, doCounter, HIGH);
  // put your setup code here, to run once:
  while(SW_A()==0){
    AO();
  }
  Wire.begin();
  Serial.begin(38400);
 
  Serial.println("Initialize MPU");
  mpu.initialize();
  Serial.println(mpu.testConnection() ? "Connected" : "Connection failed");
  delay(100);
  while(1){
    slide_R(cr);
    if(analog(1) > refR && analog(3) > refBR) {
        slide_L(4);     
        oled.text(1,1,"Can go R" );
        oled.show();
        turn_R();
        ///
        check_baricade();
        ///      
        forward(pw,xf);
        continue;
    }
    if(analog(1)<refR || analog(3)<refBR) {
        oled.text(1,1,"Cant go R");
        oled.show();
        delay(100);
        slide_L(mid);
        setR();
        slide_L(5);
        forward(20,cf);  
        
        if(analog(0)>refL && analog(1)>refR){
          oled.text(1,1,"Can go F");
          oled.show();
          ///
          check_baricade();
          ///
          forward(pw,20);
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
            slide_R(4);
            turn_L();
            ///
            check_baricade();
            ///
            forward(pw,24);     
            continue;
          }
          else if(analog(0)<refL || analog(2)< refBL){
            sound(200,200);
            delay(100);
            slide_R(mid);
            delay(100);
            turn_B();
            delay(100);
            forward(20,cf-2);      
            ///
            check_baricade();
            ///
            forward(pw,23);                    
            continue;
          }      
        }
      }
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
