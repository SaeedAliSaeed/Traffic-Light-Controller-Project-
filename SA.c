// Saeed Ali Saeed Mohamed
// Traffic Light Project - PIC16F877A
// Auto + Manual Control with restart from first phase

#define controlPin PORTB.B0   // Pin to switch between Auto/Manual mode (input)
#define westRed PORTA.B0      // West street Red LED
#define westYellow PORTA.B1   // West street Yellow LED
#define westGreen PORTA.B2    // West street Green LED

#define southRed PORTE.b0     // South street Red LED
#define southYellow PORTE.b1  // South street Yellow LED
#define southGreen PORTE.b2   // South street Green LED

#define street PORTB.B1       // Pin to choose street in Manual mode (input)

#define westDisplay PORTD     // 7-seg display output for West street counter
#define southDisplay PORTC    // 7-seg display output for South street counter

int westCounter = 0;          // Countdown timer for West street
int southCounter = 0;         // Countdown timer for South street
const int delay = 100;          // Delay constant ~1 second (100 x 10ms)
int countDown[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
                   0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
                   0x20,0x21,0x22,0x23}; // Lookup table values for 7-seg display

int restart = 0;   // (restart auto cycle)

// ================= Initialization =================
void initialConfig(){
    TRISB = 0;    // Set PortB as output (LEDs)
    TRISC = 0;    // Set PortC as output (7-seg display)
    TRISD = 0;    // Set PortD as output (7-seg display)
    TRISA = 0;    // Set PortA as output (LEDs)
    TRISE = 0;    // Set PortE as output (LEDs)

    ADCON1 = 7;   // Disable analog inputs, configure PortA & PortE as digital
    TRISB.B0 = 1; // RB0 (controlPin) as input (external interrupt)
    TRISB.B1 = 1; // RB1 (street select in manual mode) as input
    GIE_Bit = 1;  // Enable global interrupts
    INTE_Bit = 1; // Enable external interrupt on RB0
    INTEDG_Bit = 1; // Trigger interrupt on rising edge
}

// ================= LED Control Functions =================
void controlWest(int Red,int Yellow,int Green){
     westRed = Red;        // Control West Red LED
     westYellow = Yellow;  // Control West Yellow LED
     westGreen = Green;    // Control West Green LED
}

void controlSouth(int Red,int Yellow,int Green){
     southRed = Red;        // Control South Red LED
     southYellow = Yellow;  // Control South Yellow LED
     southGreen = Green;    // Control South Green LED
}

// ================= Traffic Light Phases =================
void firstPhase(){    // Phase 1: West Green (20s), South Red (23s)
    controlWest(0,0,1);   // Activate West Green
    controlSouth(1,0,0);  // Activate South Red
    for (westCounter = 20, southCounter = 23 ; westCounter > 0 ; westCounter--, southCounter--) {
         if(restart) return;   // If manual mode triggered, exit loop
         westDisplay = countDown[westCounter];   // Update West countdown
         southDisplay = countDown[southCounter]; // Update South countdown
         delay_ms(delay); // Wait ~1 second
    }
}

void secondPhase(){   // Phase 2: West Yellow (3s), South Red (3s)
    controlWest(0,1,0);   // Activate West Yellow
    controlSouth(1,1,0);  // Keep South Red (with Yellow)
    for (westCounter = 3, southCounter = 3 ; westCounter > 0 ; westCounter--, southCounter-- ) {
        if(restart) return;
        westDisplay = countDown[westCounter];   // Update West countdown
        southDisplay = countDown[southCounter]; // Update South countdown
        delay_ms(delay);
    }
}

void thirdPhase(){    // Phase 3: West Red (15s), South Green (12s)
    controlWest(1,0,0);   // Activate West Red
    controlSouth(0,0,1);  // Activate South Green
    for (westCounter = 15 ,southCounter = 12 ; southCounter > 0 ; westCounter--,southCounter--) {
        if(restart) return;
        westDisplay = countDown[westCounter];   // Update West countdown
        southDisplay = countDown[southCounter]; // Update South countdown
        delay_ms(delay);
    }
}

void fourthPhase(){   // Phase 4: West Red (3s), South Yellow (3s)
    controlWest(1,1,0);   // Activate West Red + Yellow
    controlSouth(0,1,0);  // Activate South Yellow
    for (westCounter = 3, southCounter = 3 ; southCounter > 0 ; westCounter--, southCounter--) {
        if(restart) return;
        westDisplay = countDown[westCounter];   // Update West countdown
        southDisplay = countDown[southCounter]; // Update South countdown
        delay_ms(delay);
    }
}

// ================= Interrupt code =================

int i;
unsigned short prevStreet = 0xFF;

void interrupt(){
    if (INTF_bit == 1) {
        INTF_bit = 0;

        while (controlPin == 1) {
            if (street == 0) {   // West
                if (street != prevStreet && prevStreet != 0xFF) {

                    westRed = 0; westYellow = 1; westGreen = 0;
                    southRed = 0; southYellow = 1; southGreen = 0;
                    for (i = 3; i > 0; i--) {
                        westDisplay = countDown[i];
                        southDisplay = countDown[i];
                        delay_ms(delay);
                    }
                }
                westRed = 0; westYellow = 0; westGreen = 1;
                southRed = 1; southYellow = 0; southGreen = 0;
            }
            else {   // South
                if (street != prevStreet && prevStreet != 0xFF) {
                    westRed = 0; westYellow = 1; westGreen = 0;
                    southRed = 0; southYellow = 1; southGreen = 0;

                    for (i = 3; i > 0; i--) {
                        westDisplay = countDown[i];
                        southDisplay = countDown[i];
                        delay_ms(delay);
                    }
                }
                westRed = 1; westYellow = 0; westGreen = 0;
                southRed = 0; southYellow = 0; southGreen = 1;
            }
            prevStreet = street;

            westDisplay = 0;
            southDisplay = 0;
        }

        restart = 1;
    }
}



// ================= Main Program =================
void main() {
    initialConfig();  // Configure ports & enable interrupts
    while(1) {        // Infinite loop
        if(restart){   // If returned from manual mode
            restart = 0;   // Clear flag
            continue;      // Restart auto cycle from firstPhase
        }
        firstPhase();   // Execute Phase 1
        secondPhase();  // Execute Phase 2
        thirdPhase();   // Execute Phase 3
        fourthPhase();  // Execute Phase 4 (then loop again)
    }
}