/*
 * uart.c
 *
 *  Created on: 19 mars 2020
 *      Author: Marwane Ould-Ahmed
 */

#include <msp430.h>
#include <string.h>

#define RELEASE "\r\t\tSPI-rIII162018"
#define PROMPT  "\r\nmaster>"
#define CMDLEN  10

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
unsigned char cmd[CMDLEN];

void init_UART( void )
{
    P1SEL  |= (BIT1 | BIT2);                    // P1.1 = RXD, P1.2=TXD
    P1SEL2 |= (BIT1 | BIT2);                    // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSWRST;                        // SOFTWARE RESET
    UCA0CTL1 |= UCSSEL_2;                       // SMCLK (2 - 3)
    UCA0BR0 = 104;                             // 104 1MHz, OSC16, 9600 (8Mhz : 52) : 8 115k - 226/12Mhz
    UCA0BR1 = 0;                                // 1MHz, OSC16, 9600 - 4/12Mhz
    UCA0MCTL = 10;
    UCA0CTL0 &= ~(UCPEN  | UCMSB | UCDORM);
    UCA0CTL0 &= ~(UC7BIT | UCSPB  | UCMODE_3 | UCSYNC); // dta:8 stop:1 usci_mode3uartmode
    UCA0CTL1 &= ~UCSWRST;                       // **Initialize USCI state machine**
    /* Enable USCI_A0 RX interrupt */
    IE2 |= UCA0RXIE;
}

/* ----------------------------------------------------------------------------
 * Fonction d'initialisation de l'USCI POUR SPI SUR UCB0
 * Entree : -
 * Sorties: -
 */
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

void envoi_msg_UART(unsigned char *msg)
{
    unsigned int i = 0;
    for(i=0 ; msg[i] != 0x00 ; i++)
    {
        while(!(IFG2 & UCA0TXIFG));    //attente de fin du dernier envoi (UCA0TXIFG à 1 quand UCA0TXBUF vide)
        UCA0TXBUF=msg[i];
    }
}

/* ----------------------------------------------------------------------------
 * Fonction d'envoie d'un caractère sur USCI en SPI 3 fils MASTER Mode
 * Entree : Caractère à envoyer
 * Sorties: /
 */
void Send_char_SPI(unsigned char carac)
{
    while ((UCB0STAT & UCBUSY));   // attend que USCI_SPI soit dispo.
    while(!(IFG2 & UCB0TXIFG)); // p442
    UCB0TXBUF = carac;              // Put character in transmit buffer
    envoi_msg_UART((unsigned char *)cmd);   // slave echo
}

void interpreteur( void )
{
    if(strcmp((const char *)cmd, "h") == 0)          //----------------------------------- help
    {
        envoi_msg_UART("\r\nCommandes :");
        envoi_msg_UART("\r\n'ver' : version");
        envoi_msg_UART("\r\n'0' : LED off");
        envoi_msg_UART("\r\n'1' : LED on");
        envoi_msg_UART("\r\n'h' : help\r\n");
    }
    else if (strcmp((const char *)cmd, "0") == 0)
    {
        envoi_msg_UART("\r\n");
        envoi_msg_UART((unsigned char *)cmd);
        envoi_msg_UART("->");
        Send_char_SPI(0x30); // Send '0' over SPI to Slave
        envoi_msg_UART("\r\n");
    }
    else if (strcmp((const char *)cmd, "1") == 0)
    {
        envoi_msg_UART("\r\n");
        envoi_msg_UART((unsigned char *)cmd);
        envoi_msg_UART("->");
        Send_char_SPI(0x31); // Send '1' over SPI to Slave
        envoi_msg_UART("\r\n");
    }
    else if (strcmp((const char *)cmd, "ver") == 0)
    {
        envoi_msg_UART("\r\n");
        envoi_msg_UART(RELEASE);
        envoi_msg_UART("\r\n");
    }
    else                          //---------------------------- default choice
    {
        envoi_msg_UART("\r\n ?");
        envoi_msg_UART((unsigned char *)cmd);
    }
    envoi_msg_UART(PROMPT);        //---------------------------- command prompt
}


