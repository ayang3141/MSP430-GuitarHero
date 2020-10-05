/************** ECE2049 DEMO CODE ******************/
/************** 14 May 2018   **********************/
/***************************************************/

#include <msp430.h>

#include "peripherals.h"
#include "utils/ustdlib.h"
#include "utils/debug_assert.h"

// Include puzzles for lab 0

// Function Prototypes
void swDelay(char numLoops);
void configurePins(void);
char readButtons(void);
void setExternalLeds(char x);
void delaySomeTime(void);

// Declare globals here

unsigned long int timer = 0;

enum State {welcome_screen, hardware, play} state;



// Note that notes are listed with their corresponding FREQUENCIES!!!
#define note_A4 440
#define note_B4_flat 466
#define note_B4 494
#define note_C4 523 // 1
#define note_C4_sharp 554
#define note_D4 587 // 2
#define note_E4_flat 622
#define note_E4 659 // 3
#define note_F4 698 // 4
#define note_F4_sharp 740
#define note_G4 784 // 5
#define note_A5_flat 831
#define note_A5 880

#define song_length 55
#define interrupts_per_sec 200

// Struct definition for a Music Note
// a = pitch, b = duration, c = LED
struct music_note{int pitch; int duration; char led;};

// WHOLE NOTES
struct music_note noteC_whole = {note_C4, 4, BIT3};
struct music_note noteE_whole = {note_E4, 4, BIT1};

// HALF NOTES
struct music_note noteD_half = {note_D4, 2, BIT2};
struct music_note noteE_half = {note_E4, 2, BIT1};
struct music_note noteG_half = {note_G4, 2, BIT3};

// QUARTER NOTES
struct music_note noteC_quarter = {note_C4, 1, BIT3};
struct music_note noteD_quarter = {note_D4, 1, BIT2};
struct music_note noteE_quarter = {note_E4, 1, BIT1};
struct music_note noteF_quarter = {note_F4, 1, BIT0};
struct music_note noteG_quarter = {note_G4, 1, BIT3};

// JINGLE BELLS SONG
struct music_note song[song_length] = {{note_E4, 1, BIT1},
                              {note_E4, 1, BIT1},
                              {note_E4, 2, BIT1},
                              {note_E4, 1, BIT1},
                              {note_E4, 1, BIT1},
                              {note_E4, 2, BIT1},
                              {note_E4, 1, BIT1},
                              {note_E4, 1, BIT1},
                              {note_C4, 1, BIT3},
                              {note_D4, 1, BIT2},
                              {note_E4, 4, BIT1},
                              {note_F4, 1, BIT0},
                              {note_F4, 1, BIT0},
                              {note_F4, 1, BIT0},
                              {note_F4, 1, BIT0},
                              {note_E4, 1, BIT1},
                              {note_E4, 1, BIT1},
                              {note_E4, 1, BIT1},
                              {note_E4, 1, BIT1},
                              {note_D4, 1, BIT2},
                              {note_C4, 1, BIT3},
                              {note_D4, 1, BIT2},
                              {note_E4, 1, BIT1},
                              {note_D4, 2, BIT2},
                              {note_G4, 2, BIT3},
                              {note_E4, 1, BIT1},
                              {note_E4, 1, BIT1},
                              {note_E4, 2, BIT1},
                              {note_E4, 1, BIT1},
                              {note_E4, 1, BIT1},
                              {note_E4, 2, BIT1},
                              {note_E4, 1, BIT1},
                              {note_G4, 1, BIT3},
                              {note_C4, 1, BIT3},
                              {note_D4, 1, BIT2},
                              {note_E4, 1, BIT1},
                              {note_E4, 1, BIT1},
                              {note_E4, 2, BIT1},
                              {note_F4, 1, BIT0},
                              {note_F4, 1, BIT0},
                              {note_F4, 1, BIT0},
                              {note_F4, 1, BIT0},
                              {note_E4, 1, BIT1},
                              {note_E4, 1, BIT1},
                              {note_E4, 1, BIT1},
                              {note_E4, 1, BIT1},
                              {note_G4, 1, BIT3},
                              {note_G4, 1, BIT3},
                              {note_F4, 1, BIT0},
                              {note_D4, 1, BIT2},
                              {note_C4, 4, BIT3}
};

struct music_note test[5] = {{note_C4, 1, BIT3},
                             {note_D4, 1, BIT2},
                             {note_E4, 1, BIT1},
                             {note_F4, 1, BIT0},
                             {note_G4, 1, BIT3}
};




// Main
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer. Always need to stop this!!
                                 // You can then configure it properly, if desired

    // *** System initialization ***
    initLaunchpadButtons();
    initLaunchpadLeds();
    configDisplay();

    // ********* DEMO CODE **********
    // This part of the program demonstrates an actual
    // program that provides some useful functionality with the hardware

    // *** Intro Screen ***

    Graphics_clearDisplay(&g_sContext); // Clear the display

    // Write some text to the display
    int h_center = LCD_HORIZONTAL_MAX / 2;

    // Draw a box around everything because it looks nice
    Graphics_Rectangle box = {.xMin = 5, .xMax = LCD_HORIZONTAL_MAX - 5,
                              .yMin = 5, .yMax = LCD_VERTICAL_MAX - 5 };
    Graphics_drawRectangle(&g_sContext, &box);

    // We are now done writing to the display.  However, if we stopped here, we would not
    // see any changes on the actual LCD.  This is because we need to send our changes
    // to the LCD, which then refreshes the display.
    // Since this is a slow operation, it is best to refresh (or "flush") only after
    // we are done drawing everything we need.
    Graphics_flushBuffer(&g_sContext);

    configurePins();

    // Configure TIMERA2    time interval period is approximately 0.005 seconds
    // actual time passed is 0.0049991608 (i.e. device is slightly fast)
    // accumulated error equals 0.005 after 5958 interrupts or roughly 1.6 hours
    // Therefore, this timer configuration will be sufficient for this lab
    TA2CTL = TASSEL_2 | ID_0 | MC_1;
    TA2CCR0 = 5241;
    TA2CCTL0 = CCIE;
    __enable_interrupt();
    state = welcome_screen;



    while (1)    // Forever loop
    {

        /*BuzzerOn(note_A4);
        swDelay(1);
        BuzzerOn(note_B4_flat);
        swDelay(1);
        BuzzerOn(note_B4);
        swDelay(1);
        BuzzerOn(note_C4);
        swDelay(1);
        BuzzerOn(note_C4_sharp);
        swDelay(1);
        BuzzerOn(note_D4);
        swDelay(1);
        BuzzerOn(note_E4_flat);
        swDelay(1);
        BuzzerOn(note_E4);
        swDelay(1);
        BuzzerOn(note_F4);
        swDelay(1);
        BuzzerOn(note_F4_sharp);
        swDelay(1);
        BuzzerOn(note_G4);
        swDelay(1);
        BuzzerOn(note_A5_flat);
        swDelay(1);
        BuzzerOn(note_A5);
        swDelay(1);
        BuzzerOff();
        swDelay(1);*/
        switch(state)
        {
            case welcome_screen:
            {
                Graphics_drawStringCentered(&g_sContext, "Welcome",  AUTO_STRING_LENGTH, h_center, 15, TRANSPARENT_TEXT);
                Graphics_drawStringCentered(&g_sContext, "to",       AUTO_STRING_LENGTH, h_center, 25, TRANSPARENT_TEXT);
                Graphics_drawStringCentered(&g_sContext, "MSP430 Hero!", AUTO_STRING_LENGTH, h_center, 35, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);

                if(readLaunchpadButtons() != 0)
                {
                    state = hardware;
                    Graphics_clearDisplay(&g_sContext); // Clear the display
                    break;
                }

                break;
            }
            case hardware:
            {
                int i;
                for(i = 0; i < 5; i++)  // Plays a few notes on Buzzer, and lights up respective LEDs
                {
                    unsigned long int start_time = timer;
                    while(timer <= (start_time + (interrupts_per_sec * test[i].duration)))
                    {
                        BuzzerOn(test[i].pitch);
                        setExternalLeds(test[i].led);
                    }

                }
                setExternalLeds(0x00);
                BuzzerOff();
                Graphics_clearDisplay(&g_sContext); // Clear the display

                char x = 0;
                while(x != 0x0F)   // Tests functionality of external buttons
                {
                    Graphics_drawStringCentered(&g_sContext, "Press the",  AUTO_STRING_LENGTH, h_center, 15, OPAQUE_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "LED buttons",  AUTO_STRING_LENGTH, h_center, 25, OPAQUE_TEXT);
                    Graphics_flushBuffer(&g_sContext);
                    x |= readButtons();
                    setExternalLeds(x);
                }
                setExternalLeds(0x00);
                Graphics_clearDisplay(&g_sContext); // Clear the display

                while(readLaunchpadButtons() == 0)
                {
                    Graphics_drawStringCentered(&g_sContext, "Press a Launchpad",  AUTO_STRING_LENGTH, h_center, 15, OPAQUE_TEXT);
                    Graphics_drawStringCentered(&g_sContext, "button",  AUTO_STRING_LENGTH, h_center, 25, OPAQUE_TEXT);
                    Graphics_flushBuffer(&g_sContext);
                }

                state = play;
                Graphics_clearDisplay(&g_sContext); // Clear the display
                break;
            }
            case play:
            {
                int j;
                for(j = 0; j < song_length; j++)  // Plays a few notes on Buzzer, and lights up respective LEDs
                {

                    unsigned long int start_time = timer;
                    while(timer <= (start_time + (interrupts_per_sec * song[j].duration)))
                    {
                        BuzzerOn(song[j].pitch);
                        setExternalLeds(song[j].led);
                        char button_state = readButtons();
                        if(button_state == song[j].led)
                        {
                            P4OUT |= BIT7;  // Green Launchpad LED turns on if correct button is pressed
                            P1OUT &= ~BIT0;
                        }
                        else if((button_state != song[j].led) & (button_state != 0x00))
                        {
                            P4OUT &= ~BIT7;
                            P1OUT |= BIT0;  // Red Launchpad LED turns on if incorrect button is pressed
                        }
                        else    // Turn off LEDs
                        {
                            P4OUT &= ~BIT7;
                            P1OUT &= ~BIT0;
                        }

                    }
                    BuzzerOff();
                    delaySomeTime();


                }
                state = welcome_screen;
                break;
            }




        }


    }  // end while (1)
}


void swDelay(char numLoops)
{
	// This function is a software delay. It performs
	// useless loops to waste a bit of time
	//
	// Input: numLoops = number of delay loops to execute
	// Output: none
	//
	// smj, ECE2049, 25 Aug 2013

	volatile unsigned int i,j;	// volatile to prevent removal in optimization
			                    // by compiler. Functionally this is useless code

	for (j=0; j<numLoops; j++)
    {
    	i = 50000 ;					// SW Delay
   	    while (i > 0)				// could also have used while (i)
	       i--;
    }
}

void configurePins(void)
{
    // Pins 6.0-3 are configured as inputs, pull-down resistors will need to be used
    // Pins 3.4, 3.3, 4.2, and 4.1 are configured as outputs, no internal resistors are needed

    P6SEL &= ~(BIT0 | BIT1 | BIT2 | BIT3);   // Configures digital i/o mode for pins 6.0-3
    P6DIR &= ~(BIT0 | BIT1 | BIT2 | BIT3);   // Configures P6.0-3 as inputs
    P6REN |= (BIT0 | BIT1 | BIT2 | BIT3);    // Enables internal resistor for pins P6.0-3
    P6OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3);   // Configures internal resistors as pull-down for P6.0-3

    P4SEL &= ~(BIT1 | BIT2);                 // Configures digital i/o mode for pins P4.1-2
    P4DIR |= (BIT1 | BIT2);                  // Configures pins P4.1-2 as outputs

    P3SEL &= ~(BIT3 | BIT4);                 // Configures digital i/o mode for pins P3.3-4
    P3DIR |= (BIT3 | BIT4);                  // Configures P3.3-4 as outputs


}

char readButtons(void)
{
    // Bits 7 - 4       Bit 3 (G)      Bit 2 (R)       Bit 1(B)       Bit 0 (Y)
    //      All 0       P6.3            P6.2            P6.1            P6.0
    char x = P6IN;
    return (x & (BIT0|BIT1|BIT2|BIT3));


}

void setExternalLeds(char x)    // Input will be from readButtons(); i.e. only Bits 0-3 will be considered
{
    // Bits 7 - 4       Bit 3 (G)      Bit 2 (R)       Bit 1(B)       Bit 0 (Y)
    //      All 0       P6.3            P6.2            P6.1            P6.0


    // P3.4         P3.3          P4.2          P4.1
    // Green        Red           Blue          Yellow


    if(x & BIT3)    // Green LED
    {
        P3OUT |= BIT4;
    }
    else
    {
        P3OUT &= ~(BIT4);
    }


    if(x & BIT2)    // Red LED
    {
        P3OUT |= BIT3;
    }
    else
    {
        P3OUT &= ~(BIT3);
    }

    if(x & BIT1)    // Blue LED
    {
        P4OUT |= BIT2;
    }
    else
    {
        P4OUT &= ~(BIT2);
    }

    if(x & BIT0)    // Yellow LED
    {
        P4OUT |= BIT1;
    }
    else
    {
        P4OUT &= ~(BIT1);
    }


}

#pragma vector=TIMER2_A0_VECTOR
__interrupt void TIMER_A2_ISR(void) // every 0.005 seconds
{
    timer++;

}

void delaySomeTime(void)
{
    unsigned long int start_time = timer;
    while(timer <= (start_time + 2))
    {
        // useless loop
    }
}

