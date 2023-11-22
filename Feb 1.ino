#include <I2Cdev.h>
#include "Adafruit_TCS34725.h"
#include <MPU6050.h>

#include <POP32.h>
#include <Wire.h>

#define checkbr 11

#define pw 40
#define pws 27

#define GAP 5

#define upslope 56
#define stickdist 50//53
#define slopedist 4

#define downslopeR 69
#define downslopeL 69
#define downslopeF 60

#define turnsloper 20
#define turnslopel 20
#define frontslopecheck 8
#define rightslopecheck 8
#define leftslopecheck 16

#define xr 17 //17
#define xl 16 //15
#define xf 10 //9
#define xb 23 

#define cr 9
#define cl 12
#define cf 10

#define mid 6
#define turn_r 18//19
#define turn_l 18//18
#define turn_b 38//38

#define R 0
#define F 1
#define L 2
#define B 3

int refL, refR, refBL, refBR;

int direction[4] = {0,0,0,0};
int deployed[4] = {0,0,0,0};
int cango = 0;

int count = 0;
int output = 0;

int finish = 0;
int barricade = 0;
int wall = 0;
int blackhole = 0;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

const int RED_REF[] = {124, 55, 73};
const int GREEN_REF[] = {52, 100, 91};//
const int BLUE_REF[] = {52, 73, 118};//
const int YELLOW_REF[] = {109, 86, 52};//
const int WHITE_REF[] = {114, 104, 97};
const int BLACK_REF[] = {150, 125, 120};

float dt = 0.1;
float kp = 2.5;
float ki = 0.1;
float kd = 0.01;
float last_error = 0;
float integral = 0;

void doCounter() {
    count++;
}

void reset_enc() {
    count = 0;
    return count;
}

void backward(int dis) {
    while (count < dis) {
        motor(-pw, -pw, -pw, -pw);
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
    count = 0;
}

void forward(int power, int dis) { // FORWARD MOVEMENT
    while (count < dis) {
        if (analog(0) > refL && analog(1) > refR) {
            motor(power, power, power, power);
        } else if (analog(0) < refL || analog(1) < refR)
        {
            AO();
            delay(100);
            break;
        } else if (analog(0) < refL && analog(1) > refR)
        {
            motor(-power, power, -power, power);
        } else if (analog(0) > refL && analog(1) < refR)
        {
            motor(power, -power, power, -power);
        }
        if (in(6) == 0) {
            AO();
            delay(200);
            break;
        }
        if (in(5) == 0) { //hit stick during forward phase
            AO();
            delay(100);
            barricade = 1;
            break;
        }
    }
    AO();
    count = 0;
    return barricade;
}

void forwardmove(int power, int dis) {
    count = 0;
    while (count < dis) {
        motor(power, power, power, power);
        if (in(6) == 0) {
            AO();
            delay(100);
            break;
        }
        if (in(5) == 0) { //hit stick during forward phase
            AO();
            delay(100);
            barricade = 1;
            break;
        }
    }
    AO();
    count = 0;
    return barricade;
}
void forwardmoveslope(int power, int dis) {
    count = 0;
    while (count < dis) {
        motor(power, power, power, power);
        //check black     
    }
    AO();
    count = 0;
}
void slide_R(int dis) {
    while (count < dis) {
        //motor(pws-1,-pws-1,-pws,pws-2);
        motor(pws + 5, -pws - 5, -pws - 5, pws + 5);
        if (analog(1) < refR || analog(3) < refBR)
        {
            AO();
            delay(100);
            break;
        }
    }
    AO(); delay(100);
    count = 0;
}

void slide_L(int dis) {
    while (count < dis) {
        motor(-pws - 5, pws + 5, pws + 5, -pws - 5);
        //motor(-pws,pws-2,pws-1,-pws);
        if (analog(0) < refL || analog(2) < refBL)
        {
            AO();
            delay(100);
            break;
        }
    }
    AO(); delay(100);
    count = 0;
}

void turn_B() {
    while (count < (turn_b)) {
        motor(pws, -pws, pws, -pws);
    }
    AO();
    count = 0;
}

void setR() {
    while (1) {
        if (analog(1) > refR && analog(3) > refBR) {
            motor(pws, -pws, -pws, pws);
        } else if (analog(1) < refR && analog(3) < refBR) {
            break;
            /*AO();
            delay(100);*/

        } else if (analog(1) < refR && analog(3) > refBR) {
            motor(-pws, pws, -pws, pws);
        } else if (analog(1) > refR && analog(3) < refBR) {
            motor(pws, -pws, pws, -pws);
        }
    }
    AO();delay(200);
    count = 0;
}

void setL() {
    while (1) {
        if (analog(0) > refL && analog(2) > refBL) {
            motor(-pws, pws, pws, -pws);
        } else if (analog(0) < refL && analog(2) < refBL) {
            AO();
            delay(100);
            break;
        } else if (analog(0) < refL) {
            motor(pws, -pws, pws, -pws);
        } else if (analog(2) < refBL) {
            motor(-pws, pws, -pws, pws);
        }
    }
    count = 0;
}

void setF() { // FORWARD MOVEMENT
    while (1) {
        if (analog(0) > refL && analog(1) > refR) {
            motor(pws, pws, pws, pws);
        } else if (analog(0) < refL && analog(1) > refR)
        {
            motor(-pws, pws, -pws, pws);
        } else if (analog(0) > refL && analog(1) < refR)
        {
            motor(pws, -pws, pws, -pws);
        } else if (analog(0) < refL && analog(1) < refR)
        {
            AO();
            delay(100);
            break;
        }
    }
    AO();
    count = 0;
}

void setB() { // FORWARD MOVEMENT
    while (1) {
        if (analog(2) > refBL && analog(3) > refBR) {
            motor(-pws, -pws, -pws, -pws);
        } else if (analog(2) < refBL && analog(3) > refBR)
        {
            motor(pws, -pws, pws, -pws);
        } else if (analog(2) > refBL && analog(3) < refBR)
        {
            motor(-pws, pws, -pws, pws);
        } else if (analog(2) < refBL && analog(3) < refBR)
        {
            AO();
            delay(100);
            break;
        }
    }
    AO();
    count = 0;
}

void turn_R() {
    while (count < (turn_r)) {
        motor(pws, -pws, pws, -pws);
    }
    AO();
    count = 0;
}

void turn_L() {
    while (count < (turn_l)) {
        motor(-pws, pws, -pws, pws);
    }
    AO();
    count = 0;
}

void turnslopeR() {
    while (count < (turnsloper)) {
        motor(pws, -pws, pws, -pws);
        delay(50);
    }
    AO();
    count = 0;
}

void turnslopeL() {
    while (count < (turnslopel)) {
        motor(-pws, pws, -pws, pws);
    }
    AO();
    count = 0;
}

void check_baricade() {
    AO();
    //delay(100);
    count = 0;
    while (count < checkbr) {
        motor(pws, pws, pws, pws);
        if (barricade == 1) { // hit da stick bf check barricade stage
            AO();break;
        }
        if (in(5) == 0) { // hit stick
            AO();
            delay(200);
            sound(200, 200);
            barricade = 1;
            break;
        }
        if (in(6) == 0) { // hit da wall
            AO();
            delay(200);
            sound(200, 200);
            delay(200);
            backward(10);
            delay(200);
            turn_B();
            barricade = 0;
            finish = 1;
            break;
        }
        if (analog(0) < refL || analog(1) < refR) { //black hole + fixing position when 
            //the check dis broken
            AO();
            delay(100);
            int previous_count;
            previous_count = count;
            count = 0;
            while (count < previous_count) {
                motor(-pws, -pws, -pws, -pws);
            }
            AO();
            delay(100);
            count = 0;
            setF();
            delay(100);
            backward(4);
            delay(100);
            turn_B();
            delay(100);
            setB();
            while (count < previous_count) {
                motor(pws, pws, pws, pws);
            }
            AO();
            delay(100);
            count = 0;
            barricade = 0;
            finish = 1;
            break;
            /*setF();
            while(count < previous_count){
              motor(-pw,-pw,-pw,-pw);
            }
            AO();
            count=0;
            turn_B();
            delay(100);
            barricade = 0;
            finish = 1;
            break;*/
        }
    }
    AO();
    //delay(50);
    count = 0;
    if (barricade == 1) { //caught slope n stick
        AO();
        //int dist = map(analog(4),760,2900,30,0);
        /*while(count <= checktype) {
          motor(pws,pws,pws,pws);
        }*/
        delay(100);
        //if(dist<slopedist) {
        if (analog(7) > 700) {
            oled.clearDisplay();
            oled.text(1, 1, "slope");
            oled.show();
            //while(1){AO();}
            slope();
            barricade = 0;
            finish = 1;
        } else { // Stick
            oled.clearDisplay();
            oled.text(1, 1, "stick");
            oled.show();
            count = 0;
            /*while(count <= 7) {
                  motor(-pws,-pws,-pws,-pws);
            }

            AO();delay(300);
            count=0;*/
            while (count <= stickdist) {
                motor(pw, pw, pw, pw);
            }
            AO();delay(300);
            //while(1){AO();}
            barricade = 0;
            finish = 1;
        }
    }
    AO();
    //delay(100);
    count = 0;
    return finish;
}

void slope() {
    count = 0;
    while (count <= upslope) {
        motor(pws, pws, pws, pws);
    }
    AO();delay(100);
    count = 0;
    //turn_R();
    // concept //
    /*

    */
    //forward(pws,frontslopecheck+3);
    slide_R(rightslopecheck);
    if (analog(1) > refR && analog(3) > refBR) {
        slide_L(10);
        turnslopeR();
        setL();
        slide_R(6);
        setB();
        //turnslopeR();
        forwardmoveslope(pws, downslopeR);
    } else if (analog(1) < refR || analog(3) < refBR) {
        slide_L(leftslopecheck);
        if (analog(0) > refL && analog(2) > refBL) {
            AO();
            delay(100);
            slide_R(10);
            turnslopeL();
            setR();
            slide_L(6);
            setB();
            //turnslopeR();
            forwardmoveslope(pws, downslopeL);
        } else if (analog(0) < refL || analog(2) < refBL) {
            slide_R(5);
            delay(100);
            setL();
            delay(100);
            slide_R(5);
            forwardmoveslope(pws, downslopeF);
        }
    }
    /*if(analog(0)>refL && analog(1)>refR){
      oled.text(1,1,"Can go FS");
      oled.show();
      // while(1){AO();}
      // //forward(pw,downslope);
      while(count<=downslope) {
        motor(pws,pws,pws,pws);
      }
      AO();delay(100);
      count=0;
    }
    else if((analog(0)<refL || analog(1)<refR)) {
      setF();
      delay(25);
      backward(5);
      turnslopeR();
      forward(pws,frontslopecheck+5);
      if(analog(0)>refL && analog(1)>refR){
        oled.text(1,1,"Can go RS");
        oled.show();
       // while(1){AO();}
        while(count<=downslope) {
          motor(pws,pws,pws,pws);
        }
        AO();delay(100);
        count=0;
      }
      else if((analog(0)<refL || analog(1)<refR)) {
        setF();
        delay(25);
        backward(5);
        turnslopeL();
        forward(pws,frontslopecheck);
        if(analog(0)>refL && analog(1)>refR){
          oled.text(1,1,"Can go LS");
          oled.show();
          //while(1){AO();}
          while(count<=downslope) {
            motor(pws,pws,pws,pws);
          }
          AO();delay(100);
          count=0;
        }
      }
    }*/
    //older version
}

void stick() {

}

void deploy(int value) {
    AO();
    delay(300);
    //turn_B();
    if (value == 0) {
        if (deployed[0] == 0) { // Deploy RED
            //forwardmove(pws,4);
            turn_B();
            setB();
            forwardmove(pws, 4);
            delay(100);
            setR();
            slide_L(5);//
            forwardmove(pws, 6);
            delay(100);
            servo(2, 150);
            delay(1250);
            servo(2, 90);
            delay(100);
            oled.text(1, 1, "RED DEPLOYED");
            oled.show();
            deployed[0] = 1;
            //backward(4);
            delay(100);
            forwardmove(pws, 23);
            /*setR();
            slide_L(6);
            delay(100);
            forwardmove(pws,32);*/
        } else if (deployed[0] == 1) {
            backward(4);
            delay(100);
            turn_B();
            delay(100);
            setB();
            forwardmove(pws, 4);
            setR();
            slide_L(6);
            forwardmove(pws, 14);
        }
    } else if (value == 1) {
        if (deployed[1] == 0) {
            backward(20);
            delay(100);
            setF();
            delay(100);
            turn_B();
            forwardmove(pws, 5);
            setB();
            servo(2, 25);
            delay(1250);
            servo(2, 90);
            oled.text(1, 1, "GREEN DEPLOYED");
            oled.show();
            deployed[1] = 1;
            //forwardmove(pws,12);
            //setB();
            forwardmove(pws, 21);
        } else if (deployed[1] == 1) {
            backward(4);
            delay(100);
            turn_B();
            delay(100);
            setB();
            forwardmove(pws, 4);
            setR();
            slide_L(6);
            forwardmove(pws, 14);
        }
    } else if (value == 2) {
        if (deployed[2] == 0) {
            backward(20);
            delay(100);
            setF();
            delay(100);
            turn_B();
            forwardmove(pws, 5);
            setB();
            servo(1, 150);
            delay(1250);
            servo(1, 90);
            oled.text(1, 1, "BLUE DEPLOYED");
            oled.show();
            deployed[2] = 1;
            //forwardmove(pws,12);
            //setB();
            forwardmove(pws, 20);
        } else if (deployed[2] == 1) {
            backward(4);
            delay(100);
            turn_B();
            delay(100);
            setB();
            forwardmove(pws, 4);
            setR();
            slide_L(6);
            forwardmove(pws, 14);
        }
    } else if (value == 3) {
        if (deployed[3] == 0) {
            turn_B();
            setB();
            forwardmove(pws, 4);
            delay(100);
            setR();
            slide_L(5);//6
            forwardmove(pws, 6);
            delay(100);
            servo(1, 5);
            delay(1250);
            servo(1, 90);
            delay(100);
            oled.text(1, 1, "YELLOW DEPLOYED");
            oled.show();
            deployed[3] = 1;
            //backward(4);
            delay(100);
            forwardmove(pws, 23);
            ///
        } else if (deployed[3] == 1) {
            backward(4);
            delay(100);
            turn_B();
            delay(100);
            setB();
            forwardmove(pws, 4);
            setR();
            slide_L(6);
            forwardmove(pws, 14);
        }
    }
    //while(SW_A()==0){AO();}
    count = 0;
    //turn_B();
    // maybe added forward here //
}

void get_color() {
    AO();
    delay(1000);
    uint16_t clear, red, green, blue;
    tcs.getRawData(&red, &green, &blue, &clear);
    uint32_t sum = clear;
    float r, g, b;
    r = red; r /= sum;
    g = green; g /= sum;
    b = blue; b /= sum;
    r *= 256; g *= 256; b *= 256;
    uint16_t _rr, _gg, _bb;
    _rr = uint32_t(r);
    _gg = uint32_t(g);
    _bb = uint32_t(b);
    // uint16_t _rr = 256*(red/sum);
    // uint16_t _gg = 256*(green/sum);
    // uint16_t _bb = 256*(blue/sum);
    //while(1) {AO();oled.text(1,1,"%d %d %d",_rr,_gg,_bb);oled.show();}
    if ((_rr <= RED_REF[0] + GAP && _rr >= RED_REF[0] - GAP) &&
        (_gg <= RED_REF[1] + GAP && _gg >= RED_REF[1] - GAP) &&
        (_bb <= RED_REF[2] + GAP && _bb >= RED_REF[2] - GAP))
    {
        AO();
        oled.text(6, 1, "COLOR : RED");
        oled.show();
        //while(SW_A()==0){AO();}
        deploy(0);
        delay(100);
    } else if ((_rr <= GREEN_REF[0] + GAP && _rr >= GREEN_REF[0] - GAP) &&
        (_gg <= GREEN_REF[1] + GAP && _gg >= GREEN_REF[1] - GAP) &&
        (_bb <= GREEN_REF[2] + GAP && _bb >= GREEN_REF[2] - GAP))
    {
        AO();
        oled.text(6, 1, "COLOR : GREEN");
        oled.show();
        delay(200);
        // while(SW_A()==0){AO();}
        deploy(1);
        delay(100);
    } else if ((_rr <= BLUE_REF[0] + GAP && _rr >= BLUE_REF[0] - GAP) &&
        (_gg <= BLUE_REF[1] + GAP && _gg >= BLUE_REF[1] - GAP) &&
        (_bb <= BLUE_REF[2] + GAP && _bb >= BLUE_REF[2] - GAP))
    {
        AO();
        oled.text(6, 1, "COLOR : BLUE");
        oled.show();
        //while(SW_A()==0){AO();}
        deploy(2);
        delay(100);
    } else if ((_rr <= YELLOW_REF[0] + GAP && _rr >= YELLOW_REF[0] - GAP) &&
        (_gg <= YELLOW_REF[1] + GAP && _gg >= YELLOW_REF[1] - GAP) &&
        (_bb <= YELLOW_REF[2] + GAP && _bb >= YELLOW_REF[2] - GAP))
    {
        //forward(pws,10);
        AO();
        // while(SW_A()==0){AO();}
        while (1) {
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
                oled.text(6, 1, "COLOR : YELLOW");
                oled.show();
                // while(SW_A()==0){AO();}
                deploy(3);
                count = 0;
                delay(100);
            } else {
                AO();
                delay(100);
                break;
            }
        }
    }
}

void setup() {
    // put your setup code here, to run once:
    pinMode(22, INPUT);
    attachInterrupt(22, doCounter, LOW);
    // put your setup code here, to run once:
    while (SW_A() == 0) {
        AO();
    }
    servo(1, 90);
    servo(2, 90);
    servo(3, 90);
    delay(100);
    servo(1, -1);
    servo(2, -1);
    servo(3, -1);
    count = 0;
    refL = (analog(0) * 3) / 5;
    refR = (analog(1) * 3) / 5;
    refBL = (analog(2) * 3) / 5;
    refBR = (analog(3) * 3) / 5;
    delay(100);
    while ((deployed[0] != 1) || (deployed[1] != 1) || (deployed[2] != 1) || (deployed[3] != 1)) {
        if ((deployed[0] == 1) && (deployed[1] == 1) && (deployed[2] == 1) && (deployed[3] == 1)) {
            AO();
            break;
        }
        get_color();
        slide_R(cr);
        if (analog(1) > refR && analog(3) > refBR) {
            slide_L(7);
            oled.text(1, 1, "Can go R");
            oled.show();
            turn_R();
            ///
            check_baricade();count = 0;
            /// 
            //oled.text(1,1,"finish = %d",finish);oled.show();
            if (finish == 1) { finish = 0;continue; }
            //delay(100);
            forwardmove(pws, xr);
            //get_color();
            //delay(100);
            continue;
        }
        if (analog(1) < refR || analog(3) < refBR) {
            oled.text(1, 1, "Cant go R");
            oled.show();
            delay(100);
            slide_L(mid);
            setR();
            slide_L(5);
            forward(20, cf);
            if (analog(0) > refL && analog(1) > refR) {
                oled.text(1, 1, "Can go F");
                oled.show();
                ///
                check_baricade();
                if (finish == 1) { finish = 0;continue; }
                //delay(100);
                forwardmove(pws, xf);
                //get_color();
                //delay(100);
            } else if (analog(0) < refL || analog(1) < refR) {
                oled.text(1, 1, "Cant go F");
                oled.show();
                sound(200, 200);
                backward(4);
                delay(100);
                setF();
                delay(100);
                backward(4);
                delay(100);
                slide_L(cl);
                if (analog(0) > refL && analog(2) > refBL) {
                    slide_R(6);
                    turn_L();
                    ///
                    check_baricade();
                    if (finish == 1) { finish = 0;continue; }
                    //delay(100);
                    ///
                    forwardmove(pws, xl);
                    //get_color();
                    delay(100);
                    continue;
                } else if (analog(0) < refL || analog(2) < refBL) {
                    sound(200, 200);
                    delay(100);
                    slide_R(6);
                    delay(100);
                    turn_B();
                    delay(100);
                    setB();
                    //forward(20,cf-2);      
                    ///
                    check_baricade();count = 0;
                    if (finish == 1) { finish = 0;continue; }
                    //delay(100);
                    ///
                    forwardmove(pws, xb);
                    //get_color();
                    delay(100);
                    continue;
                }
            }
        }
    }
    AO();
    servo(3, 0);//
    delay(500);
    servo(3, -1);
    delay(100);
    oled.text(1, 1, "Finished");
    oled.show();
}

void loop() {
    // put your main code here, to run repeatedly:
    
}