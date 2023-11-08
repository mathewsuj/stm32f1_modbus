/*
 * config.h
 *
 *  Created on: Jun 15, 2014
 *      Author: DELL
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define MODBUS_ON

#ifdef MODBUS_ON
#define MODBUS_SLAVE
#define modbus_ascii
#define MODBUS_ID_MIN 10
#define MODBUS_ID_MAX 20
#endif

#define baudrate 19200

#endif /* CONFIG_H_ */
