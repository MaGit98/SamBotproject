/*
 * Spi.c
 *
 *  Created on: 25 mars 2020
 *      Author: Marwane Ould-Ahmed
 */
#include <msp430.h>
#include <string.h>
#include "main.h"
#include "Uart.h"


/*
 * Variables globales
 */
// static const char spi_in = 0x37;
extern unsigned char cmd[CMDLEN];      // tableau de caracteres lie a la commande user


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
    //           x=3 -> i�c.
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
 * Fonction d'envoie d'un caract�re sur USCI en SPI 3 fils MASTER Mode
 * Entree : Caract�re � envoyer
 * Sorties: /
 */
void Send_char_SPI(unsigned char carac)
{
    while ((UCB0STAT & UCBUSY));   // attend que USCI_SPI soit dispo.
    while(!(IFG2 & UCB0TXIFG)); // p442
    UCB0TXBUF = carac;              // Put character in transmit buffer
    envoi_msg_UART((unsigned char *)cmd);   // slave echo
}

void Send_text_SPI(unsigned char  text[])
{
    unsigned char *p;
    p = text;
    Send_char_SPI(0x23);  //all communication begin by "#" and end by "!"
    while(*p != 0)
    {
        Send_char_SPI(*p);
        p++;
    }
    Send_char_SPI(0x21);
}

/*
 * Function to reorganize the bits at the reception
 */
char rearrange_spi(void){
    char arrange[8]= {0,0,0,0,0,0,0,0};
    char var;
    var= UCB0RXBUF;
    int i;
    for (i = 7; i > 0; i--){
        arrange[i]= (var >>i );
    }
    for(i=0; i<8; i++){
       var = var >>i;
       var = arrange[i];
    }
    return var;
}

//dsiplay text from SPI to UART screen
void Display_text_SPI(void)
{
    if(UCB0RXBUF == "#")
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART(PROMPT_SLAVE);
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART("->");
        while (UCB0RXBUF == "!")
        {

            envoi_msg_UART(UCB0RXBUF);
        }
    }
    else
    {

    }
}
