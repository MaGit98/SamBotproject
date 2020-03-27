/*
 * Uart.h
 *
 *  Created on: 19 mars 2020
 *      Author: Marwane Ould-Ahmed
 */



#ifndef UART_H_
#define UART_H_


void init_UART( void );

void envoi_msg_UART(unsigned char * );
void interpreteur( void );
void interpreteur_robot( void );

#endif /* UART_H_ */


