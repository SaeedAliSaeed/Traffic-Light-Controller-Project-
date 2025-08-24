#define controlPin PORTB.B0

#define westRed PORTA.B0
#define westYellow PORTA.B1
#define westGreen PORTA.B2

#define southRed PORTE.b0
#define southYellow PORTE.b1
#define southGreen PORTE.b2

#define street PORTB.B7

#define westDisplay PORTD
#define southDisplay PORTC

int westCounter = 0;
int southCounter = 0;
const int SEC = 100;
int countDown[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23};

void initialConfig(){
TRISB = 0; TRISC = 0; TRISD = 0;  trisA=0; trisE=0; //Set PORT B,C,D As Output
ADCON1 = 7;   //Make PortA & PortE R/W Analog Signal
TRISB.B0 = 1; //Make interrupt pin as input
GIE_Bit = 1;  //Global interrupt enable
INTE_Bit = 1; //B0 interrupt enable
INTEDG_Bit = 1; //Interrupt On Rising Edge
}

void setWest(int Red,int Yellow,int Green){
     westRed = Red;
     westYellow = Yellow;
     westGreen = Green;
}

void setSouth(int Red,int Yellow,int Green){
     southRed = Red;
     southYellow = Yellow;
     southGreen = Green;
}

void firstPhase(){    //West is Green and South is Red for 20s
setWest(0,0,1);
setSouth(1,0,0);
for (westCounter = 20, southCounter = 23 ; westCounter > 0 ; westCounter--, southCounter--) {
     westDisplay = countDown[westCounter];
     southDisplay = countDown[southCounter];
     delay_ms(SEC);
}
}

void secondPhase(){   //West is Yellow and South is Red for 3s
setWest(0,1,0);
setSouth(1,0,0);
for (westCounter = 3, southCounter = 3 ; westCounter > 0 ; westCounter--, southCounter-- ) {
    westDisplay = countDown[westCounter];
    southDisplay = countDown[southCounter];
    delay_ms(SEC);
}
}

void thirdPhase(){    //West is Red and South is Green for 12s
setWest(1,0,0);
setSouth(0,0,1);
for (westCounter = 15 ,southCounter = 12 ; southCounter > 0 ; westCounter--,southCounter--) {
    westDisplay = countDown[westCounter];
    southDisplay = countDown[southCounter];
    delay_ms(SEC);
}

}

void fourthPhase(){   //West is Red and South is Yellow for 3s
setWest(1,0,0);
setSouth(0,1,0);
for (westCounter = 3, southCounter = 3 ; southCounter > 0 ; westCounter--, southCounter--) {
    westDisplay = countDown[westCounter];
    southDisplay = countDown[southCounter];
    delay_ms(SEC);
}
}

void interrupt(){
if(INTF_Bit == 1){
    INTF_Bit = 0; //Reset Flag
    while (controlPin == 1){ //Still in manual mode
    
          if (street == 0){ //West Street
                westRed = 0; westYellow = 0; westGreen = 1;
                southRed = 1; southYellow = 0; southGreen = 0;
          }
          else if (street == 1){ //South Street
                westRed = 1; westYellow = 0; westGreen = 0;
                southRed = 0; southYellow = 0; southGreen = 1;
          }
          westDisplay = 0;
          southDisplay = 0;
    }
}
}

void main() {

initialConfig();

while(1) {

    firstPhase();
    secondPhase();
    thirdPhase();
    fourthPhase();

}
}