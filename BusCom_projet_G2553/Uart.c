/*
 * uart.c
 *
 *  Created on: 19 mars 2020
 *      Author: Marwane Ould-Ahmed
 */

#include <msp430.h>
#include <string.h>
#include "main.h"
#include "Motor.h"
#include "Spi.h"
#include "Uart.h"

/*
 * Variables globales
 */
unsigned char cmd[CMDLEN];
unsigned int interpreteur_state = 0;

void init_UART( void )
{
    P1SEL  |= (BIT1 | BIT2);                    // P1.1 = RXD, P1.2=TXD
    P1SEL2 |= (BIT1 | BIT2);                    // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSWRST;                        // SOFTWARE RESET
    UCA0CTL1 |= UCSSEL_2;                       // SMCLK (2 - 3)
    UCA0BR0 = 104;                             // 104 1MHz, OSC16, 9600 (8Mhz : 52) : 8 115k - 226/12Mhz
    UCA0BR1 = 0;                                // 1MHz, OSC16, 9600 - 4/12Mhz
    UCA0MCTL = 10;
    UCA0CTL0 &= ~(UCPEN  | UCMSB | UCDORM);
    UCA0CTL0 &= ~(UC7BIT | UCSPB  | UCMODE_3 | UCSYNC); // dta:8 stop:1 usci_mode3uartmode
    UCA0CTL1 &= ~UCSWRST;                       // **Initialize USCI state machine**
    /* Enable USCI_A0 RX interrupt */
    IE2 |= UCA0RXIE;
}

/* ----------------------------------------------------------------------------
 * Fonction d'initialisation de l'USCI POUR SPI SUR UCB0
 * Entree : -
 * Sorties: -
 */

void envoi_msg_UART(unsigned char *msg)
{
    unsigned int i = 0;
    for(i=0 ; msg[i] != 0x00 ; i++)
    {
        while(!(IFG2 & UCA0TXIFG));    //attente de fin du dernier envoi (UCA0TXIFG � 1 quand UCA0TXBUF vide)
        UCA0TXBUF=msg[i];
    }
}

void go (void)
{
    avancer(20);
}

void stop (void)
{
    avancer(0);
}

void forward (void)
{
    avancer(20);
}

void back (void)
{
    reculer(20);
}
void right (void)
{
    tourner_droite();
}
void left (void)
{
    tourner_gauche();
}
void detection_on (void)
{
    tourner_gauche();
}
void detection_off (void)
{
    tourner_gauche();
}

void interpreteur( void )
{
    if(strcmp((const char *)cmd, "h") == 0)          //----------------------------------- help
    {
        envoi_msg_UART("\r\nCommandes :");
        envoi_msg_UART("\r\n'ver' : version");
        envoi_msg_UART("\r\n'0' : LED off");
        envoi_msg_UART("\r\n'1' : LED on");
        envoi_msg_UART("\r\n'2' : LED switch");
        envoi_msg_UART("\r\n'h' : help");
        envoi_msg_UART("\r\n'LED test' : test if the distant Led is off");
        envoi_msg_UART("\r\n'robot order' : display the order");
        envoi_msg_UART("\r\n'robot menu' : go to the robot menu\r\n");
    }
    else if (strcmp((const char *)cmd, "0") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART((unsigned char *)cmd);
        envoi_msg_UART("->");
        Send_char_SPI(0x30); // Send '0' over SPI to Slave
        envoi_msg_UART(NEW_LINE);
        P1OUT &= ~LED_R;
    }
    else if (strcmp((const char *)cmd, "LED test") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART((unsigned char *)cmd);
        envoi_msg_UART("->");
        Send_text_SPI("LED test"); // Send '0' over SPI to Slave
        envoi_msg_UART(NEW_LINE);
    }
    else if (strcmp((const char *)cmd, "1") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART((unsigned char *)cmd);
        envoi_msg_UART("->");
        Send_char_SPI(0x31); // Send '1' over SPI to Slave
        envoi_msg_UART(NEW_LINE);
        P1OUT|=LED_R;
    }
    else if (strcmp((const char *)cmd, "2") == 0)
     {
         envoi_msg_UART(NEW_LINE);
         envoi_msg_UART((unsigned char *)cmd);
         envoi_msg_UART("->");
         Send_char_SPI(0x32); // Send '2' over SPI to Slave
         envoi_msg_UART(NEW_LINE);
         P1OUT^=LED_R;
     }
    else if (strcmp((const char *)cmd, "ver") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART(RELEASE);
        envoi_msg_UART(NEW_LINE);
    }
    else if (strcmp((const char *)cmd, "robot order") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART("'stop' : stop the robot   -   ");
        envoi_msg_UART("'go' : move forward the robot");
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART("'forward' : the robot move forward   -   ");
        envoi_msg_UART("'back' : the robot move back off");
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART("'right' : the robot move to its right   -   ");
        envoi_msg_UART("'left' : the robot move to its left");
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART("'detection on' : activate the obstacle detection   -   ");
        envoi_msg_UART("'detection off' : deactivate the obstacle detection");
        envoi_msg_UART(NEW_LINE);
    }
    else if (strcmp((const char *)cmd, "robot menu") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART((unsigned char *)cmd);
        envoi_msg_UART(NEW_LINE);
        interpreteur_state = 1;
        //initialiser_moteur();
        envoi_msg_UART("\rRobot Ready !\r\n"); // user prompt
        envoi_msg_UART(PROMPT_ROBOT);        //---------------------------- command prompt
     }

    else if (strcmp((const char *)cmd, "") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART(RELEASE);
        envoi_msg_UART(NEW_LINE);
    }
    else if (strcmp((const char *)cmd, 0x10) == 0)
    {
        envoi_msg_UART(DEL);
    }
    else                          //---------------------------- default choice
    {
        envoi_msg_UART("\r\n ?");
        envoi_msg_UART((unsigned char *)cmd);
    }
    envoi_msg_UART(PROMPT);        //---------------------------- command prompt
}


void interpreteur_robot( void )
{
    if(strcmp((const char *)cmd, "h") == 0)          //----------------------------------- help
    {
        envoi_msg_UART("\r\nCommandes :");
        envoi_msg_UART("\r\n'h' : help");
        envoi_msg_UART("\r\n'robot order' : display the order\r\n");
        envoi_msg_UART("\r\n'robot menu' : go to the robot menu\r\n");
        envoi_msg_UART("\r\n'menu' : go to the menu\r\n");
    }

    else if (strcmp((const char *)cmd, "robot order") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART("'stop' : stop the robot   -   ");
        envoi_msg_UART("'go' : move forward the robot");
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART("'forward' : the robot move forward   -   ");
        envoi_msg_UART("'back' : the robot move back off");
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART("'right' : the robot move to its right   -   ");
        envoi_msg_UART("'left' : the robot move to its left");
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART("'detection on' : activate the obstacle detection   -   ");
        envoi_msg_UART("'detection off' : deactivate the obstacle detection");
        envoi_msg_UART(NEW_LINE);
    }
    else if (strcmp((const char *)cmd, "go") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART((unsigned char *)cmd);
        envoi_msg_UART("->");
        envoi_msg_UART(NEW_LINE);
        go();
    }
    else if (strcmp((const char *)cmd, "stop") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART((unsigned char *)cmd);
        envoi_msg_UART("->");
        envoi_msg_UART(NEW_LINE);
        stop();
    }
    else if (strcmp((const char *)cmd, "forward") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART((unsigned char *)cmd);
        envoi_msg_UART("->");
        envoi_msg_UART(NEW_LINE);
        forward();
    }
    else if (strcmp((const char *)cmd, "back") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART((unsigned char *)cmd);
        envoi_msg_UART("->");
        envoi_msg_UART(NEW_LINE);
        back();
    }
    else if (strcmp((const char *)cmd, "right") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART((unsigned char *)cmd);
        envoi_msg_UART("->");
        envoi_msg_UART(NEW_LINE);
        right();
    }
    else if (strcmp((const char *)cmd, "left") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART((unsigned char *)cmd);
        envoi_msg_UART("->");
        envoi_msg_UART(NEW_LINE);
        left();
    }
    else if (strcmp((const char *)cmd, "detection_on") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART((unsigned char *)cmd);
        envoi_msg_UART("->");
        envoi_msg_UART(NEW_LINE);
        detection_on();
    }
    else if (strcmp((const char *)cmd, "detection_off") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART((unsigned char *)cmd);
        envoi_msg_UART("->");
        envoi_msg_UART(NEW_LINE);
        detection_off();
    }
    else if (strcmp((const char *)cmd, "") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART(RELEASE);
        envoi_msg_UART(NEW_LINE);
    }
    else if (strcmp((const char *)cmd, "menu") == 0)
    {
        envoi_msg_UART(NEW_LINE);
        envoi_msg_UART((unsigned char *)cmd);
        envoi_msg_UART(NEW_LINE);
        interpreteur_state = 0;
        envoi_msg_UART("\rReady !\r\n"); // user prompt
        envoi_msg_UART(PROMPT);        //---------------------------- command prompt
     }
    else                          //---------------------------- default choice
    {
        envoi_msg_UART("\r\n ?");
        envoi_msg_UART((unsigned char *)cmd);
    }
    envoi_msg_UART(PROMPT_ROBOT);        //---------------------------- command prompt
}

