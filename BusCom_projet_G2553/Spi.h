/*
 * Spi.h
 *
 *  Created on: 25 mars 2020
 *      Author: Marwane Ould-Ahmed
 */

#ifndef SPI_H_
#define SPI_H_

void init_USCI( void );
void Send_char_SPI( unsigned char );
void Send_text_SPI(unsigned char text[]);
void Display_text_SPI(void);
char rearrange_spi(void);

#endif /* SPI_H_ */

