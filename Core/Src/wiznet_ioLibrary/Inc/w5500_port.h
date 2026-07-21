/*
 * w5500_port.h
 *
 *  Created on: Jul 17, 2026
 *      Author: katev
 */

#ifndef WIZNET_IOLIBRARY_W5500_PORT_H_
#define WIZNET_IOLIBRARY_W5500_PORT_H_


#include"stdint.h"

void w5500_io_init(void);
uint8_t wizchip_read_byte(void);
void wizchip_write_byte(uint8_t data);

#endif /* WIZNET_IOLIBRARY_W5500_PORT_H_ */
