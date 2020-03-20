/*
 * Motor.h
 *
 *  Created on: 19 mars 2020
 *      Author: Marwane Ould-Ahmed
 */


#ifndef MOTOR_H_
#define MOTOR_H_

//initalise le moteur
void initialiser_moteur(void);

//change la vitesse des roues
void changer_vitesse_roue_gauche(int vitesse, int sens);
void changer_vitesse_roue_droite(int vitesse, int sens);

// tourner
void tourner_gauche(void);
void tourner_droite(void);

//avance
void avancer(int vitesse);

//recule
void reculer(int vitesse);

//arrete
void arreter(void);


#endif /* MOTOR_H_ */
