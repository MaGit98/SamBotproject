/*
 * spi.c
 *
 *  Created on: 27 mars 2020
 *      Author: olivier
 */


#include <msp430.h>
#include <intrinsics.h>
#include <string.h>

/*
 * Definitions
 */
#define CMDLEN  15
#define RELEASE "\r\t\tSPI-rIII162018"
#define PROMPT  "\r\nmaster>"

#define TRUE    1
#define FALSE   0

#define LF      0x0A            // line feed or \n
#define CR      0x0D            // carriage return or \r
#define BSPC    0x08            // back space
#define DEL     0x7F            // SUPRESS
#define ESC     0x1B            // escape

#define _CS         BIT4            // chip select for SPI Master->Slave ONLY on 4 wires Mode
#define SCK         BIT5            // Serial Clock
#define DATA_OUT    BIT6            // DATA out
#define DATA_IN     BIT7            // DATA in

#define LED_R       BIT0            // Red LED on Launchpad
#define LED_G       BIT6            // Green LED

/*
 * Variables globales
 */
// static const char spi_in = 0x37;
unsigned char cmd[CMDLEN];      // tableau de caracteres lie a la commande user
unsigned char car = 0x30;       // 0
unsigned int  nb_car = 0;
unsigned char intcmd = FALSE;


/*
 * Variables globales
 */
volatile unsigned char RXDta;
unsigned int recording_on=0;
unsigned char cmd[CMDLEN];
unsigned int position=0;
/*
 * main.c
 */
void initSPI( void )
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

    // 3 wire, mode Clk&Ph / 14.2.3 p400
    // SDI-SDO-SCLK - LSB First - Output Enable - Transp. Latch
    USICTL0 |= (USIPE7 | USIPE6 | USIPE5 | USILSB | USIOE | USIGE );
    // Slave Mode SLAU144J 14.2.3.2 p400
    USICTL0 &= ~(USIMST); //slave mod
    USICTL1 |= USIIE;
    USICTL1 &= ~(USICKPH | USII2C);

    USICKCTL = 0;           // No Clk Src in slave mode
    USICKCTL &= ~(USICKPL | USISWCLK);  // Polarity - Input ClkLow

    USICNT = 0;
    USICNT &= ~(USI16B | USIIFGCC ); // Only lower 8 bits used 14.2.3.3 p 401 slau144j
    USISRL = 0x23;  // hash, just mean ready; USISRL Vs USIR by ~USI16B set to 0
    USICNT = 0x08;

    // Wait for the SPI clock to be idle (low).
    while ((P1IN & BIT5)) ;

    USICTL0 &= ~USISWRST;

    __bis_SR_register(LPM4_bits | GIE); // general interrupts enable & Low Power Mode
}
    void init_USCI( void )
    {
        // Waste Time, waiting Slave SYNC
        __delay_cycles(250);

        // SOFTWARE RESET - mode configuration
        UCB0CTL0 = 0;
        UCB0CTL1 = (0 + UCSWRST*1 );

        // clearing IFg /16.4.9/p447/SLAU144j
        // set by setting UCSWRST just before
        IFG2 &= ~(UCB0TXIFG | UCB0RXIFG);

        // Configuration SPI (voir slau144 p.445)
        // UCCKPH = 0 -> Data changed on leading clock edges and sampled on trailing edges.
        // UCCKPL = 0 -> Clock inactive state is low.
        //   SPI Mode 0 :  UCCKPH * 1 | UCCKPL * 0
        //   SPI Mode 1 :  UCCKPH * 0 | UCCKPL * 0  <--
        //   SPI Mode 2 :  UCCKPH * 1 | UCCKPL * 1
        //   SPI Mode 3 :  UCCKPH * 0 | UCCKPL * 1
        // UCMSB  = 1 -> MSB premier
        // UC7BIT = 0 -> 8 bits, 1 -> 7 bits
        // UCMST  = 0 -> CLK by Master, 1 -> CLK by USCI bit CLK / p441/16.3.6
        // UCMODE_x  x=0 -> 3-pin SPI,
        //           x=1 -> 4-pin SPI UC0STE active high,
        //           x=2 -> 4-pin SPI UC0STE active low,
        //           x=3 -> i²c.
        // UCSYNC = 1 -> Mode synchrone (SPI)
        UCB0CTL0 |= ( UCMST | UCMODE_0 | UCSYNC );
        UCB0CTL0 &= ~( UCCKPH | UCCKPL | UCMSB | UC7BIT );
        UCB0CTL1 |= UCSSEL_2;

        UCB0BR0 = 0x0A;     // divide SMCLK by 10
        UCB0BR1 = 0x00;

        // SPI : Fonctions secondaires
        // MISO-1.6 MOSI-1.7 et CLK-1.5
        // Ref. SLAS735G p48,49
        P1SEL  |= ( SCK | DATA_OUT | DATA_IN);
        P1SEL2 |= ( SCK | DATA_OUT | DATA_IN);

        UCB0CTL1 &= ~UCSWRST;                                // activation USCI
    }

    /* ----------------------------------------------------------------------------
     * Fonction d'emission d'une chaine de caracteres
     * Entree : pointeur sur chaine de caracteres
     * Sorties:  -
     */
    void envoi_msg_UART(unsigned char *msg)
    {
        unsigned int i = 0;
        for(i=0 ; msg[i] != 0x00 ; i++)
        {
            while(!(IFG2 & UCA0TXIFG));    //attente de fin du dernier envoi (UCA0TXIFG à 1 quand UCA0TXBUF vide)
            UCA0TXBUF=msg[i];
        }
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
    }
    else if (RXDta == 0x30)
    {
        P1OUT &= ~BIT0; //turn off LED
    }
    else if (RXDta == 0x23)
    {
        recording_on=1;
    }
    else if (RXDta == 0x21)
    {
        recording_on=0;
        postion=0;
    }
    else if (recording_on==1)
    {
        RXDta=cmd[position];
        position+=1;
    }
    USISRL = RXDta;
    USICNT &= ~USI16B;  // re-load counter & ignore USISRH
    USICNT = 0x08;      // 8 bits count, that re-enable USI for next transfert
}
//------------------------------------------------------------------ End ISR


