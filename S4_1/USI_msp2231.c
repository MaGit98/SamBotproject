#include <msp430.h>
#include <intrinsics.h>
/*
 * Pin connectées :
 * SCLK P1.5
 * SIMO P1.7
 * MOSI P1.6
 */
/*
 * USI_mp2231.c
 *
 *  Created on: 14 févr. 2020
 *      Author: olivi
 *      révision 1.2
 */


/*
void InitUSI(){
    USICTL0 |= (USIPE7 | USIPE6 | USIPE5 | USILSB);   // enable USIPE7 (spi mode), SDO, USIPE5 (input slave mode) USILSB (LSB first)

    USICTL1 |= (USICKPH | USIIE ) ; // ENABLE USIIE interrupt enable et clock phase on 1

    USICKCTL= 0x00;
    /* BIT COUNTER REGISTER     *//*
    USICNT = 0x32; //Active USIIFGCC -> ne clear pas automatiquement le compteur
}
*/

void SendData(){
    unsigned char c;
    c = USISRL;
    TXdata(c);
    USICNT &= ~USI16B;  // re-load counter & ignore USISRH
        USICNT = 0x08;  // 8 bits count, that re-enable USI for next transfert

}

#pragma vector=USI_VECTOR
__interrupt void USI(void){
    if(USISRL == 'a')
    {
    P1OUT |= BIT6; // alllumer la led
    }
    else
    {
        if(USISRL == 'e')
        {
            P1OUT &= ~BIT6; //Eteindre la LED
        }
    }
    USICNT &= ~USI16B;  // initialise le compteur
    USICNT = 0x08;      // re-activce la communication usi (prochaine communication)
    //USIIFG = 0x00;
}
