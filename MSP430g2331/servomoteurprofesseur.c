/*---------------------------------------------------------------------------
 * Carte LAUCHPAD Rev.1.5 - MSP430G2553 - Yann DUCHEMIN/ESIGELEC
 * Project demo for Servo Motor Control - Rev.27XII2018
 * Motor GWS S03N or similar (i.e. Futaba 3003)
 * Vcc (3.3V) & GND, Command
  * ---------------------------------------------------------------------------
 * MSP430G2553 / Launchpad Rev.1.5
 * +--------------------------+
 * | 1 |Vcc \/ Vss | 20 |
 * P1.0 | 2 |LED-1 | 19 |P2.6 TA0.1
 * P1.1 | 3 |Rx - Tx-RN42 | 18 |P2.7
 * P1.2 | 4 |Tx - Rx-RN42 | 17 |Tst
 * P1.3 | 5 | | 16 |!RST
 * P1.4 | 6 | | 15 |P1.7
 * P1.5 | 7 |TA0.0 LED-2| 14 |P1.6 TA0.1
 * P2.0 | 8 |TA1.0 | 13 |P2.5 TA1.2
 * P2.1 | 9 |TA1.1 | 12 |P2.4 TA1.2
 * P2.2 | 10 |TA1.1 [MOTEUR 1] | 11 |P2.3 TA1.0
 * +--------------------------+
 * ---------------------------------------------------------------------------
 */
#define MOTORS_FREQUENCY 33333 // motors period T=30 ms (0.0333 s) soit 30 Hz
#define MOTORS_DUTYCYCLE 1250 // motors duty cycle 50% soit 0.00125 ms 1250 μs
#define STEP_ANGLE 72 // step for 1 deg PW[500-3000 μs]/180deg
#define PW_MIN 504 // 500/72=7 7*72=504
#define PW_MAX 2448 // 3000/72=41 41*72=2952

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
P1SEL2 = 0x00; // GPIO
P2SEL = 0x00; // GPIO
P2SEL2 = 0x00; // GPIO
P1DIR = 0x00; // IN
P2DIR = 0x00; // IN
//---

P1SEL &= ~(BIT0 | BIT6); // Port 1, ligne 0 et 6 en fonction primaire
P1SEL2 &= ~(BIT0 | BIT6); // GPIO
P1DIR |= (BIT0 | BIT6 ); // P1.0 et P1.6 en sortie
P1OUT &= ~(BIT0 | BIT6); // P1.0 et P1.6 à 0

P1DIR &=~BIT3; // Port 1 ligne 3 en entrée
P1REN |= BIT3; // Activation de la resistance de tirage
P1OUT |= BIT3; // Resistance en Pull-Up
P1IES &=~BIT3; // Detection de front montant
P1IE |= BIT3; // Activation des interruptions sur P1.3
P2SEL |= BIT2; // Port 2, ligne 2 en fonction secondaire
P2SEL2 &= ~BIT2; // Timer
P2DIR |= BIT2; // Port 2, ligne 2 en sortie
}
/* ----------------------------------------------------------------------------
* FONCTION D'INITIALISATION DU TIMER
* Entree : -
* Sorties: -
*/
void init_Timer( void )
{
TA1CTL &= ~MC_0; // arret du timer
TA1CCR0 = MOTORS_FREQUENCY; // periode du signal PWM 2KHz
TA1CTL = (TASSEL_2 | MC_1 | ID_0 | TACLR); // select TimerA source SMCLK, set mode to up-counting
TA1CCTL1 = 0 | OUTMOD_7; // select timer compare mode
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
TA1CCR1 = cmd;
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
TA1CCR1 = cmd; // Modifier la valeur de commptage Timer (Rapport Cyclique)
}
P1IFG &= ~BIT3; // Acquiter l'interruption
}
