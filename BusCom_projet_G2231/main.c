/*
 * -----------------------------------------------------
 *  P R O G R A M M A T I O N   S P I   E S I G E L E C
 *
 * Lauchpad v1.5 support
 *
 * M S P 4 3 0 G 2 2 3 1   -   SPI/SLAVE 3 Wires
 *
 * (c)-Yann DUCHEMIN / ESIGELEC - r.III162018 for CCS
 * --------------------------------------------------------------
 * La carte Launchpad est raccordée en SPI via l'USI B0
 *      SCLK : P1.5 / SCLK
 *      SIMO : P1.7 / SDI
 *      MOSI : P1.6 / SDO
 *
 *  @Version 1.2  # Bleinc Olivier
 *
 */

#include <msp430.h>
#include <intrinsics.h>
#include <string.h>

/*
 * Definitions
 */
#define CMDLEN  15


void Send_message_SPI (void);

/*
 * Variables globales
 */
unsigned char TXDta;
volatile unsigned char RXDta;
unsigned int recording_on=0;
unsigned int position=0;
unsigned char cmd[CMDLEN];

void Send_message_SPI (void)
{

    if(strcmp((const char *)cmd, "LED test") == 0)
    {
        TXDta = 0x23;
        USISRL = TXDta;
        USICNT &= ~USI16B;  // re-load counter & ignore USISRH
        USICNT = 0x08;      // 8 bits count, that re-enable USI for next transfert

        //P1OUT |=BIT0;

        if(P1OUT|0==0)//Led Off
        {
            TXDta=0x31;
            USICNT &= ~USI16B;  // re-load counter & ignore USISRH
            USICNT = 0x08;
        }
        if(P1OUT&1==1)//Led On
        {
            TXDta=0x30;
            USICNT &= ~USI16B;  // re-load counter & ignore USISRH
            USICNT = 0x08;
        }
        USISRL = TXDta;
        USICNT &= ~USI16B;  // re-load counter & ignore USISRH
        USICNT = 0x08;      // 8 bits count, that re-enable USI for next transfert

        TXDta = 0x21;
        USISRL = TXDta;
        USICNT &= ~USI16B;  // re-load counter & ignore USISRH
        USICNT = 0x08;      // 8 bits count, that re-enable USI for next transfert
    }
           TXDta = 0x23;
           USISRL = TXDta;
           USICNT &= ~USI16B;  // re-load counter & ignore USISRH
           USICNT = 0x08;
           TXDta = RXDta;
           USISRL = TXDta;
           USICNT &= ~USI16B;  // re-load counter & ignore USISRH
           USICNT = 0x08;
           TXDta = 0x21;
           USISRL = TXDta;
           USICNT &= ~USI16B;  // re-load counter & ignore USISRH
           USICNT = 0x08;
}
/*
void Send_text_SPI(unsigned char  text[])
{
    unsigned char *p;
    p = text;
    Send_char_SPI(0x23);  //all communication begin by "#" and end by "!"
    while(*p != 0)
    {
        USISRL = *p;
        p++;
    }
    Send_char_SPI(0x21);
}
*/

/*
 * main.c
 */
void main( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW | WDTHOLD;

    if(CALBC1_1MHZ==0xFF || CALDCO_1MHZ==0xFF)
    {
        __bis_SR_register(LPM4_bits);
    }
    else
    {
        // Factory Set.
        DCOCTL = 0;
        BCSCTL1 = CALBC1_1MHZ;
        DCOCTL = (0 | CALDCO_1MHZ);
    }

    //--------------- Secure mode
    P1SEL = 0x00;        // GPIO
    P1DIR = 0x00;         // IN

    // led
    P1DIR |=  BIT0;
    P1OUT &= ~BIT0;

    // USI Config. for SPI 3 wires Slave Op.
    // P1SEL Ref. p41,42 SLAS694J used by USIPEx
    USICTL0 |= USISWRST;
    USICTL1 = 0;

    //USIOE = 0;//clearing the USIOE if there is only one slave, disable the SDO pin

    // 3 wire, mode Clk&Ph / 14.2.3 p400
    // SDI-SDO-SCLK - LSB First - Output Enable - Transp. Latch
    USICTL0 |= (USIPE7 | USIPE6 | USIPE5 | USILSB | USIOE | USIGE );
    // Slave Mode SLAU144J 14.2.3.2 p400
    USICTL0 &= ~(USIMST);
    USICTL1 |= USIIE;
    USICTL1 &= ~(USICKPH | USII2C);

    USICKCTL = 0;           // No Clk Src in slave mode
    USICKCTL &= ~(USICKPL | USISWCLK);  // Polarity - Input ClkLow

    USICNT = 0;
    USICNT &= ~(USI16B | USIIFGCC ); // Only lower 8 bits used 14.2.3.3 p 401 slau144j
    USISRL = 0x23;  // hash, just mean ready; USISRL Vs USIR by ~USI16B set to 0
    USICNT = 0x08;

    BCSCTL1 = CALBC1_1MHZ;
      DCOCTL = CALDCO_1MHZ;                     // submainclock 1mhz
      WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

      CCTL0 = CCIE;                             // CCR0 interrupt enabled
      CCR0 = 1000;                  // 1ms at 1mhz
      TACTL = TASSEL_2 + MC_1;                  // SMCLK, upmode

      P1IFG  = 0x00;                //clear all interrupt flags
      P1DIR |= 0x01;                            // P1.0 as output for LED
      P1OUT &= ~0x01;                           // turn LED off

      _BIS_SR(GIE);                         // global interrupt enable

        P1IE &= ~0x01;          // disable interupt
        P1DIR |= 0x02;          // trigger pin as output
        P1OUT |= 0x02;          // generate pulse
        __delay_cycles(10);             // for 10us
        P1OUT &= ~0x02;                 // stop pulse
        P1DIR &= ~0x04;         // make pin P1.2 input (ECHO)
            P1IFG = 0x00;                   // clear flag just in case anything happened before
        P1IE |= 0x04;           // enable interupt on ECHO pin
        P1IES &= ~0x04;         // rising edge on ECHO pin
            __delay_cycles(30000);          // delay for 30ms (after this time echo times out if there is no object detected)
            distance = sensor/58;           // converting ECHO lenght into cm
            if(distance < 20 && distance != 0) P1OUT |= 0x01;  //turning LED on if distance is less than 20cm and if distance isn't 0.
            else P1OUT &= ~0x01;

    // Wait for the SPI clock to be idle (low).
    while ((P1IN & BIT5)) ;

    USICTL0 &= ~USISWRST;

    __bis_SR_register(LPM4_bits | GIE); // general interrupts enable & Low Power Mode
}

// --------------------------- R O U T I N E S   D ' I N T E R R U P T I O N S

/* ************************************************************************* */
/* VECTEUR INTERRUPTION USI                                                  */
/* ************************************************************************* */
#pragma vector=USI_VECTOR
__interrupt void universal_serial_interface(void)
{
    while( !(USICTL1 & USIIFG) );   // waiting char by USI counter flag
    RXDta = USISRL;

    if (RXDta == 0x31) //if the input buffer is 0x31 (mainly to read the buffer)
    {
        P1OUT |= BIT0; //turn on LED
        //TXDta=0x0F;
    }
    else if (RXDta == 0x30)
    {
        P1OUT &= ~BIT0; //turn off LED
        TXDta=0x0A;
    }
    else if (RXDta == 0x23)
    {
        recording_on=1;
    }
    else if (RXDta == 0x21)
    {
        recording_on=0;
        position=0;
    }
    else if (recording_on==1)
    {
        RXDta=cmd[position];
        position+=1;
    }


    //USISRL = 0x0F;

    Send_message_SPI();
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if(P1IFG&0x04)  //is there interrupt pending?
        {
          if(!(P1IES&0x04)) // is this the rising edge?
          {
            TACTL|=TACLR;   // clears timer A
            miliseconds = 0;
            P1IES |= 0x04;  //falling edge
          }
          else
          {
            sensor = (long)miliseconds*1000 + (long)TAR;    //calculating ECHO lenght

          }
    P1IFG &= ~0x04;             //clear flag
    }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
  miliseconds++;
}

//------------------------------------------------------------------ End ISR
