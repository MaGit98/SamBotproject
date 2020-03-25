#include <msp430g2231.h>
/*
 * Interval max : 2.5ms -> 180°
 * Each ° need 11,1us
 */

unsigned int command = 0 ;

void tournerUSmoins90(){ //mise à l'angle min

      TACCR1=15000; //(20000-TACCR1=500)
}

void tournerUSplus90(){  // mise à l'angle max
      TACCR1=17500;
}
void tournerAOrigine(){
       TACCR1=18500;
}
void setAngle(command){

    TACCR1=15000+command*11;
}
