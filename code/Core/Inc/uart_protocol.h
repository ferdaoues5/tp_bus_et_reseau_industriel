/*
 * uart_protocol.h
 *      Author: Slimen wissal et Marzouk ferdawes
 */

#ifndef INC_UART_PROTOCOL_H_
#define INC_UART_PROTOCOL_H_

#include "main.h"
#include "string.h"
#include "BMP280.h"

void protocol(char RX_Pi_buffer[RX_BUFFER_SIZE], uint8_t Size);



#endif /* INC_UART_PROTOCOL_H_ */
