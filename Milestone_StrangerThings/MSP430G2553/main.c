/*
 * Sean Prendergast
 * Milestone 1
 */

void Timer(void);    // Declare a function for timers being utilized
void RGB(void);      // Declare function for RGB LED
void UART(void);     // Declare function for UART (universal asynchronous receiver-transmitter)

#include <msp430.h> 

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	Timer();               //Call function for Timers being utilized
	RGB();                  //Call function for RGB LED
	UART();                 //Call function for UART (universal asynchronous receiver-transmitter)

	__bis_SR_register(LPM0_bits + GIE);     // Low-power mode 0 and general interrupt enabled

	return 0;
}

// Function for timers being utilized
void Timer(void)
{
    // Timer A0
    TA0CTL |= TASSEL_2 + MC_1;      // TASSEL_2 selects smclk (sub-main clock) is set at 1 MHz
                                    // MC_1 selects up mode
    TA0CCTL1 |= OUTMOD_7;            // Enables reset/set mode
                                    // The output is reset when the timer counts to the TACCRx value
                                    // It is set when the timer counts to the TACCR0 value
    TA0CCR0 = 255;                  // Set PWM Period in the Timer A0 Capture/Compare 0 Register to 255
    TA0CCR1 = 0;                    // Red PWM is initialized

    // Timer A1
    TA1CTL |= TASSEL_2 + MC_1;      // TASSEL_2 selects smclk (sub-main clock) is set at 1 MHz
                                    // MC_1 selects up mode
    TA1CCTL1 |= OUTMOD_7;           // Enables reset/set mode
                                    // The output is reset when the timer counts to the TACCRx value
    TA1CCTL2 |= OUTMOD_7;           // Enables reset/set mode
                                    // The output is reset when the timer counts to the TACCRx value
    TA1CCR0 = 255;                  // Set PWM Period in the Timer A1 Capture/Compare 0 Register to 255
    TA1CCR1 = 0;                    // Green PWM is initialized
    TA1CCR2 = 0;                    // Blue PWM is initialized
}

// Function for RBG (RED GREEN BLUE) LED
void RGB(void)
{
    P1DIR |= BIT6;                  // Set Red LED Direction Register to the output direction
    P1SEL |= BIT6;                  // PWM is enabled for Red LED
    P2DIR |= BIT1;                  // Set Green LED Direction Register to the output direction
    P2SEL |= BIT1;                  // PWM is enabled for Green LED
    P2DIR |= BIT5;                  // Set Blue LED Direction Register to the output direction
    P2SEL |= BIT5;                  // PWM is enabled for Blue LED
}

// Function for UART (universal asynchronous receiver-transmitter)
void UART(void)
{
    P1SEL |= BIT1 + BIT2;           // P1.1 = RXD P1.2 = TXD
    P1SEL2 |= BIT1 + BIT2;          // P1.1 = RXD P1.2 = TXD

    UCA0CTL1 |= UCSSEL_2;           // smclk (sub-main clock) for 9600 baud rate
    UCA0BR0 = 104;                  // (1 MHz) / (9600 baud rate) = 104
    UCA0BR1 = 0;                    // 1 MHz 9600 baud rate
    UCA0MCTL = UCBRS0;              // Modulation UBRSx = 1
    UCA0CTL1 &= ~UCSWRST;           // USCI State Machine is initialized

    IE2 |= UCA0RXIE;                 // RX interrupt is enabled
}

int length = 0;                     // Length variable is initialized
int counter = 1;                    // Counter variable is initialized
int PWM_red = 0;                    // Red PWM variable is initialized
int PWM_green = 0;                  // Green PWM variable is initialized
int PWM_blue = 0;                   // Blue PWM variable is initialized

#pragma vector=USCIAB0RX_VECTOR     // Interrupt Routine
__interrupt void Test(void)
{
    while (!(IFG2&UCA0TXIFG));        // Wait until USCI_A0 TX buffer is ready

    if(counter==1)                    // If first signal is received
    {
        UCA0TXBUF = UCA0RXBUF-3;        // The output signal = (input signal) - (3 PWM values being used)
        length = UCA0RXBUF-3;           // Set length equal to the output signal = (input signal) - (3 PWM values being used)
    }

    else if(counter<5)                /* As the counter starts at 1, this is else if statement will happen when the counter
                                      reaches values of 2, 3, and 4, which represent the values of the RGB LED*/
    {
      switch(counter)                   // Switch statement implemented to look at the 2nd, 3rd, and 4th byte of input signal
          {
          case 2:                       // Case 2 is the second input signal
              PWM_red = UCA0RXBUF;      // Set byte = PWM_red
              break;                    // Break from this case
          case 3:                       // Case 3 is the third input signal
              PWM_green = UCA0RXBUF;    // Set byte = PWM_green
              break;                    // Break from this case
          case 4:                       // Case 4 is the fourth input signal
              PWM_blue = UCA0RXBUF;     // Set byte = PWM_blue
              break;                    // Break from case
          default:                      // Default case if the 2nd, 3rd, or 4th byte of input signal aren't reached
              break;                    // Break from this case
          }

      }

      else                              // If it is not at the 1st byte or any byte corresponding to any RGB value, the byte is outputted
          UCA0TXBUF = UCA0RXBUF;

      if(counter==length+3)             // If there is the carriage return byte, reset the counter back to 0, as incrementer will set counter back to 1 automatically
      {
          counter = 0;                    // Counter is reset to 0
          TA0CCR1 = PWM_red;              // PWM for red is set to TA0CCR1 timer
          TA1CCR1 = PWM_green;            // PWM for green is set to TA1CCR1 timer
          TA1CCR2 = PWM_blue ;            // PWM for blue is set to TA1CCR2 timer
      }

      counter++;                          // Counter is incremented by 1

}








