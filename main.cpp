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
int dick = 0;
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
        dick++;
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
        if(dick < 215){                       //go forward to start mission1       
            servo_control_LEFT(93);
            servo_control_RIGHT(-87);            
        }else if(dick >= 215 && dick < 228){  //turn left
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 1;
            } 
            servo_control_LEFT(-70);
            servo_control_RIGHT(-70);             
        }else if(dick >= 228 && dick <321){   //go forward to mission 1
            if(stop == 1){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 0;
            } 
            servo_control_LEFT(93);
            servo_control_RIGHT(-87);            
        }else if(dick >= 321 && dick < 336){  //turn right
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 1;
            } 
            servo_control_LEFT(70);
            servo_control_RIGHT(70);             
        }else if(dick >= 336 && dick < 399){  //go backward
            if(stop == 1){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              wait(1);
              stop = 0;
            }
            servo_control_LEFT(-93);
            servo_control_RIGHT(87);            
        }else if(dick >= 399 && dick < 425){  //go forward
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 1;
            } 
            servo_control_LEFT(93);
            servo_control_RIGHT(-87);      
        }else if(dick >= 425 && dick < 438){  //turn left
            if(stop == 1){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 0;
            } 
            servo_control_LEFT(-70);
            servo_control_RIGHT(-70);             
        }else if(dick >= 438 && dick < 469){  //go forward
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 1;
            } 
            servo_control_LEFT(93);
            servo_control_RIGHT(-87);             
        }else if(dick >= 469 && dick < 483){  //turn right and take a picture
            if(stop == 1){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 0;
            }
            servo_control_LEFT(70);
            servo_control_RIGHT(70);
        }else if(dick >= 483 && dick < 511){  //go forward
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              wait(6);
              stop = 1;
            } 
            if(uart.readable()){
                char recv = uart.getc();
                xbee.putc(recv);
            }
            servo_control_LEFT(93);
            servo_control_RIGHT(-87);                          
        }else if(dick >= 511 && dick < 524){  //turn right
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 1;
            }
            servo_control_LEFT(70);
            servo_control_RIGHT(70);             
        }else if(dick >= 524 && dick < 628){  //go forward to leave mission1
            if(stop == 1){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 0;
            }
            servo_control_LEFT(93);
            servo_control_RIGHT(-87);           
        }else if(dick >= 628 && dick < 641){  //turn right
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 1;
            }
            servo_control_LEFT(70);
            servo_control_RIGHT(70);             
        }else if(dick >= 641 && dick < 835){  //go forward to start mission2
            if(stop == 1){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                wait(2);
                stop = 0;
            }
            servo_control_LEFT(93);
            servo_control_RIGHT(-87);             
        }else if(dick >= 835 && dick < 850){  //turn right
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 1;
            }
            servo_control_LEFT(70);
            servo_control_RIGHT(70);            
        }else if(dick >= 850 && dick < 925){  //go forward to miossion2
            if(stop == 1){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 0;
            }
            servo_control_LEFT(93);
            servo_control_RIGHT(-87);           
        }else if(dick >= 925 && dick < 937){  //turn right
            if(stop == 0){
              servo_control_LEFT(0);
              servo_control_RIGHT(0);
              stop = 1;
            }
            servo_control_LEFT(70);
            servo_control_RIGHT(70);   
        }else if(dick >= 937 && dick < 979){  //go forward
            if(stop == 1){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                wait(2);
                stop = 0;
            }
            servo_control_LEFT(93);
            servo_control_RIGHT(-87);           
        }else if(dick >= 979 && dick < 984){  //turn right scan
            if(stop == 0){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                wait(2);
                stop = 1;
            }
            servo_control_LEFT(70);
            servo_control_RIGHT(70);
        }else if(dick >= 984 && dick < 990){  //turn left scan
            if(stop == 1){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                wait(2);
                stop = 0;
            }
            servo_control_LEFT(-70);
            servo_control_RIGHT(-70);   
        }else if(dick >= 990 && dick < 1022){ //go backward
            if(stop == 0){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                stop = 1;
            }
            servo_control_LEFT(-93);
            servo_control_RIGHT(87);            
        }else if(dick >= 1022 && dick < 1037){ //turn left 
            if(stop == 1){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                wait(2);
                stop = 0;
            }
            servo_control_LEFT(-70);
            servo_control_RIGHT(-70);   
        }else if(dick >= 1037 && dick < 1125){ //go forward to leave mission2
            if(stop == 0){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                wait(2);
                stop = 1;
            }
            servo_control_LEFT(93);
            servo_control_RIGHT(-87);           
        }else if(dick >= 1125 && dick < 1140){ //turn right
            if(stop == 1){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                stop = 0;
            }
            servo_control_LEFT(70);
            servo_control_RIGHT(70);   
        }else if(dick >= 1140 && dick < 1400){  //go forward to finish 
            if(stop == 0){
                servo_control_LEFT(0);
                servo_control_RIGHT(0);
                stop = 1;
            }
            servo_control_LEFT(93);
            servo_control_RIGHT(-87);             
        }else{
            servo_control_LEFT(0);
            servo_control_RIGHT(0);
        }

        wait(0.01);
    }
}

void getLog(Arguments *in, Reply *out){
    if(dick < 215){                         
        xbee.printf("go forward to start mission1\r\n");          
    }else if(dick >= 215 && dick < 228){  
        xbee.printf("turn left\r\n");     
    }else if(dick >= 228 && dick <321){   
        xbee.printf("go forward to mission1\r\n");    
    }else if(dick >= 321 && dick < 336){  
        xbee.printf("turn right\r\n");         
    }else if(dick >= 336 && dick < 399){  
        xbee.printf("go backward\r\n");    
    }else if(dick >= 399 && dick < 425){  
        xbee.printf("go forward\r\n");  
    }else if(dick >= 425 && dick < 438){  
        xbee.printf("turn left\r\n");    
    }else if(dick >= 438 && dick < 469){  
        xbee.printf("go forward\r\n");    
    }else if(dick >= 469 && dick < 483){  
        xbee.printf("turn right and take a picture\r\n");
    }else if(dick >= 483 && dick < 511){  
        xbee.printf("go forward\r\n");                 
    }else if(dick >= 511 && dick < 524){ 
        xbee.printf("turn right\r\n");    
    }else if(dick >= 524 && dick < 628){ 
        xbee.printf("go forward to leave mission1\r\n");      
    }else if(dick >= 628 && dick < 641){ 
        xbee.printf("turn right\r\n");         
    }else if(dick >= 641 && dick < 835){  
        xbee.printf("go forward to start mission2\r\n");    
    }else if(dick >= 835 && dick < 850){  
        xbee.printf("turn right\r\n");      
    }else if(dick >= 850 && dick < 925){  
        xbee.printf("go forward to mission2\r\n");  
    }else if(dick >= 925 && dick < 937){  
        xbee.printf("turn right\r\n");
    }else if(dick >= 937 && dick < 979){  
        xbee.printf("go forward\r\n");   
    }else if(dick >= 979 && dick < 984){  
        xbee.printf("turn right scan\r\n");
    }else if(dick >= 984 && dick < 990){  
        xbee.printf("turn left scan\r\n");
    }else if(dick >= 990 && dick < 1022){ 
        xbee.printf("go backward\r\n");    
    }else if(dick >= 1022 && dick < 1037){  
        xbee.printf("turn left\r\n");
    }else if(dick >= 1037 && dick < 1125){ 
        xbee.printf("go forward to leave mission2\r\n");
    }else if(dick >= 1125 && dick < 1140){ 
        xbee.printf("turn right\r\n");
    }else if(dick >= 1140 && dick < 1400){   
        xbee.printf("go forward to finish\r\n");   
    }else{
        xbee.printf("no action\r\n");
    }
}

void mission2(){
  xbee.printf("%f\r\n", (float) ping);
  /*

  ...

  */
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
  static int i = 0;
  static char buf[100] = {0};
  while(xbee.readable()){
    char c = xbee.getc();
    if(c!='\r' && c!='\n'){
      buf[i] = c;
      i++;
      buf[i] = '\0';
    }else{
      i = 0;
      pc.printf("Get: %s\r\n", buf);
      xbee.printf("%s", buf);
    }
  }
  wait(0.1);
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