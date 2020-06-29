#include "mbed.h"
#include "bbcar.h"
#include "mbed_rpc.h"

//motor start
#define CENTER_BASE 1500
PwmOut servol(D9);
PwmOut servor(D8);
//motot end

//encoder start 
DigitalIn encoder(D3);
Ticker encoder_ticker;
volatile int steps;
//int dick = 0;
volatile int last;
//encoder end

DigitalInOut ping_origin(D10);
parallax_ping  ping(ping_origin);
DigitalOut stop(LED3);

RawSerial xbee(D12, D11);
Serial uart(D1, D0);
Serial pc(USBTX, USBRX);

EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;
int i = 0;
float ping_mission2[10];

void servo_control_LEFT(int speed){
    if (speed > 200)       speed = 200;
    else if (speed < -200) speed = -200;
    servol = (CENTER_BASE + speed)/20000.0f;
}

void servo_control_RIGHT(int speed){
    if (speed > 200)       speed = 200;
    else if (speed < -200) speed = -200;
    servor = (CENTER_BASE + speed)/20000.0f;
}

void encoder_control(){
    int value = encoder;

    if(!last && value){
        steps++;
        //dick++;
        }
    last = value;
}

void getLog(Arguments *in, Reply *out);
RPCFunction rpcLog(&getLog, "getLog");

void xbee_setting(void);
void xbee_rx_interrupt(void);
void xbee_rx(void);
void reply_messange(char *xbee_reply, char *messange);
void check_addr(char *xbee_reply, char *messenger);
void mission2(void);

int main() {
    pc.baud(9600);
    xbee.baud(9600);
    uart.baud(9600);
    xbee_setting();

    servol.period(.02);
    servor.period(.02);

    encoder_ticker.attach(&encoder_control, .001);

    steps = 0;
    last = 0;

    while(1) {
        if(steps < 215){                       //go forward to start mission1       
            servo_control_LEFT(93);
            servo_control_RIGHT(-90);            
        }else if(steps >= 215 && steps < 228){  //turn left
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 1;
            } 
            servo_control_LEFT(-70);
            servo_control_RIGHT(-70);             
        }else if(steps >= 228 && steps <321){   //go forward to mission 1
            if(stop == 1){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 0;
            } 
            servo_control_LEFT(93);
            servo_control_RIGHT(-90);            
        }else if(steps >= 321 && steps < 336){  //turn right
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 1;
            } 
            servo_control_LEFT(70);
            servo_control_RIGHT(70);             
        }else if(steps >= 336 && steps < 399){  //go backward
            if(stop == 1){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              wait(1);
              stop = 0;
            }
            servo_control_LEFT(-93);
            servo_control_RIGHT(90);            
        }else if(steps >= 399 && steps < 430){  //go forward
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 1;
            } 
            servo_control_LEFT(93);
            servo_control_RIGHT(-90);      
        }else if(steps >= 430 && steps < 443){  //turn left
            if(stop == 1){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 0;
            } 
            servo_control_LEFT(-70);
            servo_control_RIGHT(-70);             
        }else if(steps >= 443 && steps < 474){  //go forward
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 1;
            } 
            servo_control_LEFT(93);
            servo_control_RIGHT(-90);             
        }else if(steps >= 474 && steps < 488){  //turn right and take a picture
            if(stop == 1){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 0;
            }
            servo_control_LEFT(70);
            servo_control_RIGHT(70);
        }else if(steps >= 488 && steps < 511){  //go forward
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              wait(6);
              stop = 1;
            } 
            servo_control_LEFT(93);
            servo_control_RIGHT(-90);                          
        }else if(steps >= 511 && steps < 524){  //turn right
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 1;
            }
            servo_control_LEFT(70);
            servo_control_RIGHT(70);             
        }else if(steps >= 524 && steps < 628){  //go forward to leave mission1
            if(stop == 1){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 0;
            }
            servo_control_LEFT(93);
            servo_control_RIGHT(-90);           
        }else if(steps >= 628 && steps < 641){  //turn right
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 1;
            }
            servo_control_LEFT(70);
            servo_control_RIGHT(70);             
        }else if(steps >= 641 && steps < 835){  //go forward to start mission2
            if(stop == 1){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                wait(2);
                stop = 0;
            }
            servo_control_LEFT(93);
            servo_control_RIGHT(-90);             
        }else if(steps >= 835 && steps < 848){  //turn right
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 1;
            }
            servo_control_LEFT(70);
            servo_control_RIGHT(70);            
        }else if(steps >= 848 && steps < 923){  //go forward to miossion2
            if(stop == 1){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 0;
            }
            servo_control_LEFT(93);
            servo_control_RIGHT(-90);           
        }else if(steps >= 923 && steps < 935){  //turn right
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 1;
            }
            servo_control_LEFT(70);
            servo_control_RIGHT(70);   
        }else if(steps >= 935 && steps < 977){  //go forward
            if(stop == 1){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                wait(2);
                stop = 0;
            }
            servo_control_LEFT(93);
            servo_control_RIGHT(-90);           
        }else if(steps >= 977 && steps < 982){  //turn right scan
            if(stop == 0){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                wait(2);
                stop = 1;
            }
            servo_control_LEFT(70);
            servo_control_RIGHT(70);
        }else if(steps >= 982 && steps < 989){  //turn left scan
            if(stop == 1){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                wait(2);
                stop = 0;
            }
            servo_control_LEFT(-70);
            servo_control_RIGHT(-70);  
        }else if(steps >= 989 && steps < 1022){ //go backward
            if(stop == 0){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                stop = 1;
            }
            servo_control_LEFT(-93);
            servo_control_RIGHT(90);            
        }else if(steps >= 1022 && steps < 1037){ //turn left 
            if(stop == 1){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                wait(2);
                stop = 0;
            }
            servo_control_LEFT(-70);
            servo_control_RIGHT(-70);   
        }else if(steps >= 1037 && steps < 1125){ //go forward to leave mission2
            if(stop == 0){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                wait(2);
                stop = 1;
            }
            servo_control_LEFT(93);
            servo_control_RIGHT(-90);           
        }else if(steps >= 1125 && steps < 1140){ //turn right
            if(stop == 1){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                stop = 0;
            }
            servo_control_LEFT(70);
            servo_control_RIGHT(70);   
        }else if(steps >= 1140 && steps < 1400){  //go forward to finish 
            if(stop == 0){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                stop = 1;
            }
            servo_control_LEFT(93);
            servo_control_RIGHT(-90);             
        }else{
            servo_control_LEFT(0);
            servo_control_RIGHT(0);
        }
/*
        if(steps >= 979 && steps < 990){  //turn right scan, turn left scan
            mission2();
        }*/

        wait(0.01);
    }
}

void getLog(Arguments *in, Reply *out){
    if(steps < 215){                         
        xbee.printf("go forward to start mission1\r\n");          
    }else if(steps >= 215 && steps < 228){  
        xbee.printf("turn left\r\n");     
    }else if(steps >= 228 && steps <321){   
        xbee.printf("go forward to mission1\r\n");    
    }else if(steps >= 321 && steps < 336){  
        xbee.printf("turn right\r\n");         
    }else if(steps >= 336 && steps < 399){  
        xbee.printf("go backward\r\n");    
    }else if(steps >= 399 && steps < 425){  
        xbee.printf("go forward\r\n");  
    }else if(steps >= 425 && steps < 438){  
        xbee.printf("turn left\r\n");    
    }else if(steps >= 438 && steps < 469){  
        xbee.printf("go forward\r\n");    
    }else if(steps >= 469 && steps < 484){  
        xbee.printf("turn right and take a picture\r\n");
        while(uart.readable()){
            char recv = uart.getc();
            xbee.putc(recv);
        }
    }else if(steps >= 484 && steps < 511){  
        xbee.printf("go forward\r\n"); 
        while(uart.readable()){
            char recv = uart.getc();
            xbee.putc(recv);
        }                
    }else if(steps >= 511 && steps < 524){ 
        xbee.printf("turn right\r\n");    
    }else if(steps >= 524 && steps < 628){ 
        xbee.printf("go forward to leave mission1\r\n");      
    }else if(steps >= 628 && steps < 641){ 
        xbee.printf("turn right\r\n");         
    }else if(steps >= 641 && steps < 835){  
        xbee.printf("go forward to start mission2\r\n");    
    }else if(steps >= 835 && steps < 850){  
        xbee.printf("turn right\r\n");      
    }else if(steps >= 850 && steps < 925){  
        xbee.printf("go forward to mission2\r\n");  
    }else if(steps >= 925 && steps < 937){  
        xbee.printf("turn right\r\n");
    }else if(steps >= 937 && steps < 979){  
        xbee.printf("go forward\r\n");   
    }else if(steps >= 979 && steps < 980){  
        xbee.printf("turn right scan\r\n");
        xbee.printf("%f\r\n", (float) ping);
    }else if(steps >= 980 && steps < 986){  
        xbee.printf("turn left scan\r\n");
        xbee.printf("%f\r\n", (float) ping);
    }else if(steps >= 986 && steps < 1018){
        xbee.printf("Equal Lateral Triangle!!\r\n"); 
        xbee.printf("go backward\r\n");    
    }else if(steps >= 1018 && steps < 1037){  
        xbee.printf("turn left\r\n");
    }else if(steps >= 1037 && steps < 1125){ 
        xbee.printf("go forward to leave mission2\r\n");
    }else if(steps >= 1125 && steps < 1140){ 
        xbee.printf("turn right\r\n");
    }else if(steps >= 1140 && steps < 1400){   
        xbee.printf("go forward to finish\r\n");   
    }else{
        xbee.printf("no action\r\n");
    }
}

void mission2(){
    ping_mission2[i] = float(ping);
    i++;
    if(steps == 990){
  //xbee.printf("%f\r\n", (float) ping);
        if(ping_mission2[0] > ping_mission2[5] && ping_mission2[6] < ping_mission2[10]) { //triangle
            xbee.printf("Equal Lateral Triangle!!\r\n");
        }else if(ping_mission2[0] == ping_mission2[5] && ping_mission2[5] == ping_mission2[6] && ping_mission2[6] == ping_mission2[10]) { //rectangle
            xbee.printf("Square!!\r\n");
        }else if(ping_mission2[0] > ping_mission2[5] && ping_mission2[5] > ping_mission2[6] && ping_mission2[6] > ping_mission2[10]) { //right triangle
            xbee.printf("Right Triangle!!\r\n");
        }else {
            xbee.printf("Seesaw!!\r\n");
        }
    }
}

void xbee_setting(){
  // XBee setting
  char xbee_reply[4];
  xbee.baud(9600);
  xbee.printf("+++");
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  if(xbee_reply[0] == 'O' && xbee_reply[1] == 'K'){
    pc.printf("enter AT mode.\r\n");
    xbee_reply[0] = '\0';
    xbee_reply[1] = '\0';
  }

  xbee.printf("ATMY 0x232\r\n");
  reply_messange(xbee_reply, "setting MY : 0x232");

  xbee.printf("ATDL 0x132\r\n");
  reply_messange(xbee_reply, "setting DL : 0x132");

  xbee.printf("ATID 0x1\r\n");
  reply_messange(xbee_reply, "setting PAN ID : 0x1");

  xbee.printf("ATWR\r\n");
  reply_messange(xbee_reply, "write config");

  xbee.printf("ATMY\r\n");
  check_addr(xbee_reply, "MY");

  xbee.printf("ATDL\r\n");
  check_addr(xbee_reply, "DL");

  xbee.printf("ATCN\r\n");
  reply_messange(xbee_reply, "exit AT mode");
  xbee.getc();

  // start
  pc.printf("start\r\n");
  t.start(callback(&queue, &EventQueue::dispatch_forever));

  // Setup a serial interrupt function to receive data from xbee
  xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
}

void xbee_rx_interrupt(void)
{
  xbee.attach(NULL, Serial::RxIrq); // detach interrupt
  queue.call(&xbee_rx);
}

void xbee_rx(void)
{
  char buf[100] = {0};
  char outbuf[100] = {0};
  while(xbee.readable()){
    for (int i=0; ; i++) {
      char recv = xbee.getc();
      if (recv == '\r') {
        break;
      }
      buf[i] = pc.putc(recv);
    }
    RPC::call(buf, outbuf);
    pc.printf("%s\r\n", outbuf);
    wait(0.1);
  }
  xbee.attach(xbee_rx_interrupt, Serial::RxIrq); // reattach interrupt
}

void reply_messange(char *xbee_reply, char *messange){
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();
  if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){
   pc.printf("%s\r\n", messange);
   xbee_reply[0] = '\0';
   xbee_reply[1] = '\0';
   xbee_reply[2] = '\0';
  }
}

void check_addr(char *xbee_reply, char *messenger){
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();
  xbee_reply[3] = xbee.getc();
  pc.printf("%s = %c%c%c\r\n", messenger, xbee_reply[1], xbee_reply[2], xbee_reply[3]);
  xbee_reply[0] = '\0';
  xbee_reply[1] = '\0';
  xbee_reply[2] = '\0';
  xbee_reply[3] = '\0';
}