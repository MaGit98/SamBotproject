/*
 * initAll.c
 *
 *  Created on: 20 mars 2020
 *      Author: olivier
 */
#include <msp430g2231.h>
#include "ServoMoteur.h"

void InitPWM(void){
    TACTL &= ~MC_0; // arret du timer
        TACTL |= (TASSEL_2 | MC_1 | TAIFG); // BIT7,4 et 0 T
           TACCR0=20000; //50Hz soit 20ms chaque actualisation du servo
           TACCTL0 = OUTMOD_7; //Toogle
}

void Init2231 (void){

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

    // Définition des ports
       P1DIR |=  BIT0;  //P1.0 en sortie
       P1OUT &= ~BIT0;  //  mise à 0 de 1.0
       P1DIR |=  BIT4;  //1.4 en sortie
       P1OUT &= ~BIT4;  //Toogle P1.4
       P1DIR |= BIT2; // Pin 1.2 en sortie
       P1OUT &= ~BIT2; // mise à zero de 1.2

}

void InitUSI( void ){

    USICTL0 |= USISWRST; /* set High for USISWRST -> logic in reset state*/
    USICTL0 |= (USIPE7 | USIPE6 | USIPE5 | USILSB | USIGE | USIOE);
    USICTL0 &= ~(USIMST); /* set low for USIMST -> slave mod */
    USICTL1 |=  USIIE; /* enable interrupts */
    USICTL1 &= ~(USICKPH | USII2C); /* disable i2c mode and set date changement on first clock edge... */
    USICKCTL = 0;
    USICNT &= ~(USIIFGCC |USI16B); /* set the 8-bit register and USIIFG is auto cleared on USICNTx update*/

    while((P1IN & BIT5)); /* wait clock*/
    USICTL0 &= ~USISWRST; /* ready for operation*/

    __bis_SR_register(LPM4_bits | GIE); // general interrupts enable & Low Power Mode

}
void InitAll( void ){

    InitPWM();
    InitUSI();
    Init2231();
}

