#include <msp430.h>
#include <intrinsics.h>
#include "USI_msp2231.h"
#include <msp430g2231.h>
#include <ServoMoteur.h>
#include "initAll.h"


int main (void){
    InitAll();
    tournerUSmoins90();
    while(1);


}
