/*
 * Infrarouge.c
 *

 */
 // capteur infrarouge sur le bit3 de PDIR
#include "ADC.h"
#include "msp430.h"

int detecter_obstacle(void)
{
    ADC_Demarrer_conversion(3);  // car sur le bit3

    if (ADC_Lire_resultat() < 200) {
        return 0;   //retourne 0 si un obstacle n'est pas détectée
    } else {
        return 1;   //retourne 1 un obstacle est détectée
    }
}


void initialisation_capteurs()
{

    P1DIR &= ~BIT3;     // capteur IF detection obstacle

}
