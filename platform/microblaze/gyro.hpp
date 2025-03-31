/*
 * gyro.hpp
 *
 *  Created on: Mar 4, 2025
 *      Author: lunar
 */

#ifndef MICROBLAZE_GYRO_HPP_
#define MICROBLAZE_GYRO_HPP_

#ifdef __MICROBLAZE__


extern "C" {
#include "sleep.h"
#include "xparameters.h"
}

constexpr unsigned GYRO0_IP_ADDR = XPAR_TEST3_V1_0_0_BASEADDR;
constexpr unsigned GYRO1_IP_ADDR = XPAR_TEST3_V1_0_1_BASEADDR;


constexpr unsigned GYRO_X_ADDR_OFFSET = 0x4;
constexpr unsigned GYRO_Y_ADDR_OFFSET = 0x8;
constexpr unsigned GYRO_Z_ADDR_OFFSET = 0xC;

constexpr unsigned GYROCON_ADDR = XPAR_AXI_GPIO_GYROCON_BASEADDR;

enum GyroconResetVec {
	GC_GYRO0_SYSTEM_RST	= 0,
	GC_GYRO0_ANGLE_RST	= 1,
	GC_GYRO1_SYSTEM_RST	= 2,
	GC_GYRO1_ANGLE_RST	= 3,
};




struct GYROManager {
	int id;
	unsigned device_addr;
	void init(int gyro_id) {
		id = gyro_id;
		if (id == 0) {
			device_addr = GYRO0_IP_ADDR;
		} else {
			device_addr = GYRO1_IP_ADDR;
		}
	}

	void calib() {
		xil_printf("[GYRO] Calibrating %d\n", id);
		if (id == 0) {
			*(volatile unsigned*)GYROCON_ADDR = (1 << GC_GYRO0_SYSTEM_RST);
			*(volatile unsigned*)GYROCON_ADDR = 0;
		} else {
			*(volatile unsigned*)GYROCON_ADDR = (1 << GC_GYRO1_SYSTEM_RST);
			*(volatile unsigned*)GYROCON_ADDR = 0;
		}
	}

	void read(short& x, short& y, short& z) {
		x = -(short)*(volatile unsigned*)(device_addr + GYRO_X_ADDR_OFFSET);
		y = (short)*(volatile unsigned*)(device_addr + GYRO_Y_ADDR_OFFSET);
		z = (short)*(volatile unsigned*)(device_addr + GYRO_Z_ADDR_OFFSET);
	}

	void reset() {
		xil_printf("[GYRO] Resetting %d\n", id);
		if (id == 0) {
			*(volatile unsigned*)GYROCON_ADDR = (1 << GC_GYRO0_ANGLE_RST);
			*(volatile unsigned*)GYROCON_ADDR = 0;
		} else {
			*(volatile unsigned*)GYROCON_ADDR = (1 << GC_GYRO1_ANGLE_RST);
			*(volatile unsigned*)GYROCON_ADDR = 0;
		}
	}
};


#endif /* __MICROBLAZE__ */
#endif /* MICROBLAZE_GYRO_HPP_ */
