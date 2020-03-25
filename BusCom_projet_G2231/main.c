#include <msp430.h>
#include <intrinsics.h>
#include "USI_msp2231.h"
#include <msp430g2231.h>
#include <ServoMoteur.h>




void InitUSI(){
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

    /*
     * Initialisation de la PWM
     * mise en position 0° (PWM =0.5ms)
     */
    TACTL |= (BIT7 | BIT4 | TAIFG); // BIT7,4 et 1
    TACCR0=1500;
    TACCTL0 = OUTMOD_7; //Toogle
    TACCR1=100;
}


void main (void){

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
       P1DIR |=  BIT0;  // Selection P1.0
       P1OUT &= ~BIT0;  // Définit P1.0 en sortie
       P1DIR |=  BIT4;
       P1OUT &= ~BIT4;
       tournerUSplus45();
       tournerUSmoins45();
       while(1);
}
