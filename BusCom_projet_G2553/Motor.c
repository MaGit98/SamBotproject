/*
 * Motor.c
 *
 *  Created on: 19 mars 2020
 *      Author: Marwane Ould-Ahmed
 */

#include <msp430.h>


void initialiser_moteur(void)
{
    BCSCTL1 = CALBC1_1MHZ; //réglage de la fréquence du timer
    DCOCTL = CALDCO_1MHZ;

    /* CODE ROUE GAUCHE */
    P2DIR |= BIT2; //P2.2 est définit comme une sortie

    P2SEL |= BIT2; //Permet de selectionner le mode génération
    P2SEL2 &= ~BIT2; //de signaux dans le multiplexage fonctionnel

    TA1CTL |= (MC_1 | TASSEL_2); //On choisit le mode de comptage 1 (mode UP) et le type d'horlge SMCLK

    TA1CCTL1 |= OUTMOD_7; //activation du mode de sortie 7 sur TA1.1

    TA1CCR0 = 4000; //periode (4ms) du signal PWM equivalent à 250hz
    TA1CCR1 = 2000; //détermine le rapport cyclique du signal sur TA.1

    P2DIR |= BIT1; //P2.1, donnant le sens de rotation est définit comme une sortie
    P2OUT &= ~BIT1; //P2.1 sens (0 pour aller en avant / 1 pour aller en arrière)

    /* CODE ROUE DROITE (bon) */
    P2DIR |= BIT4; //P2.4 est définit comme une sortie

    P2SEL |= BIT4; //Permet de selectionner le mode génération
    P2SEL2 &= ~BIT4; //de signaux dans le multiplexage fonctionne

    P2DIR |= BIT5; //P2.5, donnant le sens de rotation est définit comme une sortie
    P2OUT |= BIT5; //P2.5 sens(1 pour aller en avant, 0 pour aller en arrière)

    TA1CCTL2 |= OUTMOD_7; //activation du mode de sortie 7 sur TA1.2

    TA1CCR2 = 2000; //rapport cyclique sur T1.2
}


void changer_vitesse_roue_gauche(int vitesse, int sens)
{
    TA1CCR1 = (2000/100)*vitesse;
    if(sens == 0)
        P2OUT |= BIT1; //P2.0 sens (0 pour aller en avant / 1 pour aller en arrière)
    else if(sens == 1)
        P2OUT &= ~BIT1; //P2.0 sens (0 pour aller en avant / 1 pour aller en arrière)
}

/*Cette fonction permet de changer la vitesse et le sens de la roue droite.
 * paramètres : idem roue gauche
 */



void changer_vitesse_roue_droite(int vitesse, int sens)
{
    TA1CCR2 = (2000/100)*vitesse;
    if(sens == 1)
       P2OUT |= BIT5; //P2.5 sens (1 pour aller en avant / 0 pour aller en arrière)
    else if(sens == 0)
        P2OUT &= ~BIT5; //P2.5 sens (1 pour aller en avant / 0 pour aller en arrière)
}

void tourner_droite(void)
{
    changer_vitesse_roue_gauche(50, 1);
    changer_vitesse_roue_droite(20, 0);
}


void tourner_gauche(void)
{
    changer_vitesse_roue_gauche(20, 0);
    changer_vitesse_roue_droite(50, 1);
}

void avancer(int vitesse)
{

    changer_vitesse_roue_gauche(vitesse,1);
    changer_vitesse_roue_droite(vitesse,1);

}

void reculer(int vitesse)
{
   changer_vitesse_roue_gauche(vitesse,0);
   changer_vitesse_roue_droite(vitesse,0);
}

void arreter(void)
{
    changer_vitesse_roue_gauche(0, 1);
    changer_vitesse_roue_droite(0, 0);
}

