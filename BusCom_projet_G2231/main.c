#include <msp430.h>
#include <intrinsics.h>
#include "USI_msp2231.h"
#include <msp430g2231.h>
#include <ServoMoteur.h>
#include "initAll.h"


int main (void){
    InitAll(); // initialise toutes les fonctions nécessaire au programme

    tournerUSmoins90();
    __delay_cycles(2000000);
    tournerUSplus90();


}
//test