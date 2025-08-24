#define controlPin PORTB.B0   // Control pin for manual/auto mode
#define westRed PORTA.B0      // West street Red LED
#define westYellow PORTA.B1   // West street Yellow LED
#define westGreen PORTA.B2    // West street Green LED

#define southRed PORTE.b0     // South street Red LED
#define southYellow PORTE.b1  // South street Yellow LED
#define southGreen PORTE.b2   // South street Green LED

#define street PORTB.B1       // Input pin to choose street in manual mode

#define westDisplay PORTD     // 7-seg display for West counter
#define southDisplay PORTC    // 7-seg display for South counter

int westCounter = 0;          // Countdown for West
int southCounter = 0;         // Countdown for South
const int SEC = 100;          // 1 second delay (100 * 10ms approx)
int countDown[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
                   0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
                   0x20,0x21,0x22,0x23}; // Lookup table for display values

void initialConfig(){
    TRISB = 0;    // Set PortB as output
    TRISC = 0;    // Set PortC as output
    TRISD = 0;    // Set PortD as output
    TRISA = 0;    // Set PortA as output
    TRISE = 0;    // Set PortE as output

    ADCON1 = 7;   // Disable analog, make PortA & PortE digital
    TRISB.B0 = 1; // RB0 as input (interrupt pin)
    TRISB.B1 = 1; // RB1 as input (manual street select)
    GIE_Bit = 1;  // Enable global interrupts
    INTE_Bit = 1; // Enable external interrupt (RB0)
    INTEDG_Bit = 1; // Interrupt on rising edge
}

void setWest(int Red,int Yellow,int Green){
     westRed = Red;        // Control West Red LED
     westYellow = Yellow;  // Control West Yellow LED
     westGreen = Green;    // Control West Green LED
}

void setSouth(int Red,int Yellow,int Green){
     southRed = Red;        // Control South Red LED
     southYellow = Yellow;  // Control South Yellow LED
     southGreen = Green;    // Control South Green LED
}

void firstPhase(){    // West Green (20s), South Red (23s)
    setWest(0,0,1);   // West = Green
    setSouth(1,0,0);  // South = Red
    for (westCounter = 20, southCounter = 23 ; westCounter > 0 ; westCounter--, southCounter--) {
         westDisplay = countDown[westCounter];   // Show West countdown
         southDisplay = countDown[southCounter]; // Show South countdown
         delay_ms(SEC); // 1s delay
    }
}

void secondPhase(){   // West Yellow (3s), South Red (3s)
    setWest(0,1,0);   // West = Yellow
    setSouth(1,1,0);  // South = Red + Yellow (just to hold)
    for (westCounter = 3, southCounter = 3 ; westCounter > 0 ; westCounter--, southCounter-- ) {
        westDisplay = countDown[westCounter];   // Show West countdown
        southDisplay = countDown[southCounter]; // Show South countdown
        delay_ms(SEC);
    }
}

void thirdPhase(){    // West Red (15s), South Green (12s)
    setWest(1,0,0);   // West = Red
    setSouth(0,0,1);  // South = Green
    for (westCounter = 15 ,southCounter = 12 ; southCounter > 0 ; westCounter--,southCounter--) {
        westDisplay = countDown[westCounter];   // Show West countdown
        southDisplay = countDown[southCounter]; // Show South countdown
        delay_ms(SEC);
    }
}

void fourthPhase(){   // West Red (3s), South Yellow (3s)
    setWest(1,1,0);   // West = Red + Yellow
    setSouth(0,1,0);  // South = Yellow
    for (westCounter = 3, southCounter = 3 ; southCounter > 0 ; westCounter--, southCounter--) {
        westDisplay = countDown[westCounter];   // Show West countdown
        southDisplay = countDown[southCounter]; // Show South countdown
        delay_ms(SEC);
    }
}

void interrupt(){
    if(INTF_Bit == 1){       // If external interrupt triggered
        INTF_Bit = 0;        // Clear interrupt flag
        while (controlPin == 1){ // Stay in manual mode
              if (street == 0){   // If switch = West
                    westRed = 0; westYellow = 0; westGreen = 1; // West Green
                    southRed = 1; southYellow = 0; southGreen = 0; // South Red
              }
              else if (street == 1){ // If switch = South
                    westRed = 1; westYellow = 0; westGreen = 0; // West Red
                    southRed = 0; southYellow = 0; southGreen = 1; // South Green
              }
              westDisplay = 0;  // Turn off displays in manual mode
              southDisplay = 0;
        }
    }
}

void main() {
    initialConfig();  // Setup ports & interrupts
    while(1) {        // Infinite loop
        firstPhase(); // Run traffic phases in order
        secondPhase();
        thirdPhase();
        fourthPhase();
    }
}