#include <msp430g2231.h>


void tournerUSmoins45(){
    TACTL |= (BIT7| BIT4 | TAIFG); // BIT7,4 et 1
      TACCR0=1000;
      TACCTL0 = OUTMOD_7; //Toogle
      TACCR1=100;
}

void tournerUSplus45(){
    TACTL |= (BIT7 | BIT4 | TAIFG); // BIT7,4 et 1
      TACCR0=2000;
      TACCTL0 = OUTMOD_7; //Toogle
      TACCR1=100;
}
