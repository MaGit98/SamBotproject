/*
 * testPins.c
 *
 *  Created on: 25 mars 2020
 *      Author: olivier
 */
#include <msp430.h>

void main (void){
    //--------------- Secure mode
     P1SEL = 0x00;        // GPIO
     P1DIR = 0x00;         // IN
// Définition des ports
       P1DIR |= BIT0;  //P1.0 en sortie
       P1OUT |= BIT0;
       P1DIR |= BIT1;
       P1OUT |= BIT1;
       P1DIR |= BIT4;  //1.4 en sortie
       P1OUT |= BIT4;  //Toogle P1.4
       P1DIR |= BIT3; // Port 1 ligne 3 en entrée
       P1OUT |= BIT3; // Resistance en Pull-Up
       P1DIR |= BIT2; // Pin 1.2 en sortie
       P1OUT |= BIT2; // mise à zero de 1.2
       P1DIR |= BIT5;  //P1.0 en sortie
       P1OUT |= BIT5;
       P1DIR |= BIT6;  //1.4 en sortie
       P1OUT |= BIT6;  //Toogle P1.4
       P1DIR |= BIT7; // Port 1 ligne 3 en entrée
       P1OUT |= BIT7; // Resistance en Pull-Up
}
