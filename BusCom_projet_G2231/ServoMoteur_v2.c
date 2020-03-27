/*
 * ServoMoteur_v2.c
 *
 *  Created on: 25 mars 2020
 *      Author: olivier
 *      Revision programme Yann Duchemein
 */

#define MOTORS_FREQUENCY 20000 // motors period T=20 ms (0.02 s) soit 50 Hz
#define MOTORS_DUTYCYCLE 10000 // motors duty cycle 50% soit 10 ms 10000 μs
#define STEP_ANGLE 11 // step for 1 deg PW[500-3000 μs]/180deg
#define PW_MIN 15000 // 500/72=7 7*72=504
#define PW_MAX 17500 // 3000/72=41 41*72=2952

/*
 * Headers
 */
#include <msp430.h>

/*
 * Prototypes
 */
void init_Board( void );
void init_Timer( void );

/*
 * Variables Globales pour interruptions
 */
unsigned int up = 0; // sens de variation
unsigned int cmd = 0; // periode du signal de commande moteur
/* ----------------------------------------------------------------------------
 * Fonction d'initialisation de la carte TI LauchPAD
 * Entree : -
 * Sorties: -
 */
void init_Board( void )
{
// Arret du "watchdog" pour eviter les redemarrages sur boucles infinies
WDTCTL = WDTPW | WDTHOLD;
// Calibration usine de l'oscillateur numerique interne
if(CALBC1_1MHZ==0xFF || CALDCO_1MHZ==0xFF)
__bis_SR_register(LPM4_bits);
else
{
DCOCTL = 0;
BCSCTL1 = CALBC1_1MHZ;
DCOCTL = CALDCO_1MHZ;
}
//--- Securisation des entrees/sorties
P1SEL = 0x00; // GPIO

P1SEL &= ~(BIT0 | BIT6); // Port 1, ligne 0 et 6 en fonction primaire
P1DIR |= (BIT0 | BIT6 ); // P1.0 et P1.6 en sortie
P1OUT &= ~(BIT0 | BIT6); // P1.0 et P1.6 à 0

P1DIR |= BIT2; // Pin 1.2 en sortie
P1OUT &= ~BIT2; // mise à zero de 1.2
P1SEL |= BIT2;

P1DIR &= ~BIT3; // Port 1 ligne 3 en entrée
P1REN |= BIT3; // Activation de la resistance de tirage
P1OUT |= BIT3; // Resistance en Pull-Up
P1IES &=~BIT3; // Detection de front montant
P1IE |= BIT3; // Activation des interruptions sur P1.3

}
/* ----------------------------------------------------------------------------
* FONCTION D'INITIALISATION DU TIMER
* Entree : -
* Sorties: -
*/
void init_Timer( void )
{
TACTL &= ~MC_0; // arret du timer
TACCR0 = MOTORS_FREQUENCY; // periode du signal PWM 2KHz
TACTL = (TASSEL_2 | MC_1 | ID_0 | TACLR); // select TimerA source SMCLK, set mode to up-counting
TACCTL0 = 0 | OUTMOD_7; // select timer compare mode
}

/* ----------------------------------------------------------------------------
* Fonction Principale
*/
void main(void)
{
init_Board();
init_Timer();

cmd = MOTORS_DUTYCYCLE;
up = 1;
TACCR0 = cmd;
__bis_SR_register(LPM0_bits | GIE); // general interrupts enable & Low Power Mode
}

/* ************************************************************************* */
/* VECTEUR INTERRUPTION PORT 1 */
/* ************************************************************************* */
#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
if( P1IFG & BIT3) // interruption Entree/sortie Port 1 ligne 3
{ // si appui sur le bouton P1.3
if( !up ) // Sens décroissant
{
P1OUT &=~BIT6; // Eteindre la Led du port 1 ligne 6
if (cmd > (PW_MIN+STEP_ANGLE) ) // Si Période mini non encore atteinte
{
cmd -= STEP_ANGLE; // Décrémenter la période
}
else // Sinon
    {
    cmd = PW_MIN; // Ajuster la période
    up = 1; // Changer le sens de boucle
    }
P1OUT ^= BIT0; // Faire clignoter la Led a chaque itération
}
else // Sinon
{
P1OUT &=~BIT0; // Eteindre la Led de P1.0
if(cmd < (PW_MAX-STEP_ANGLE) ) // Si Période inférieure au max
{
cmd += STEP_ANGLE; // Augmenter la période
}
else // Sinon
{
cmd = PW_MAX; // Ajuster la période
up = 0; // Inverser le sens de boucle
}
P1OUT ^= BIT6; // Faire clignoter la Led
}
TACCR0 = cmd; // Modifier la valeur de commptage Timer (Rapport Cyclique)
}
P1IFG &= ~BIT3; // Acquiter l'interruption
}
