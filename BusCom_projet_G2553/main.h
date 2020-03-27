/*
 * main.h
 *
 *  Created on: 20 mars 2020
 *      Author: Marwane Ould-Ahmed
 */

#ifndef MAIN_H_
#define MAIN_H_


/*
 * Definitions
 */
#define RELEASE "\r\t\tSPI-rIII162018"
#define PROMPT  "\r\nmaster>"
#define PROMPT_ROBOT  "\r\nmaster_robot>"
#define PROMPT_SLAVE  "\r\nslave>"
#define NEW_LINE "\r\n"
#define CMDLEN  15

#define TRUE    1
#define FALSE   0

#define LF      0x0A            // line feed or \n
#define CR      0x0D            // carriage return or \r
#define BSPC    0x08            // back space
#define DEL     0x7F            // SUPRESS
#define ESC     0x1B            // escape

#define _CS         BIT4            // chip select for SPI Master->Slave ONLY on 4 wires Mode
#define SCK         BIT5            // Serial Clock
#define DATA_OUT    BIT6            // DATA out
#define DATA_IN     BIT7            // DATA in

#define LED_R       BIT0            // Red LED on Launchpad
#define LED_G       BIT6            // Green LED


void init_BOARD( void );


#endif /* MAIN_H_ */
