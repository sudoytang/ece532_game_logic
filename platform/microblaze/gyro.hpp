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

constexpr unsigned GYRO0_RAW_ADDR = XPAR_GYRO0_RAW_GPIO_BASEADDR;
constexpr unsigned GYRO1_RAW_ADDR = XPAR_GYRO1_RAW_GPIO_BASEADDR;
constexpr unsigned GYRO0_IP_ADDR = XPAR_GYRO_READER_0_S00_AXI_BASEADDR;
constexpr unsigned GYRO1_IP_ADDR = XPAR_GYRO_READER_1_S00_AXI_BASEADDR;

struct GyroRawData {
	short X;
	short Y;
	short Z;
	short R;
	static GyroRawData get(void* raw_addr) {
		GyroRawData res;
		const unsigned raw_addr_u = (unsigned)raw_addr;
		const unsigned XY_RAW = raw_addr_u + 0x0;
		const unsigned ZR_RAW = raw_addr_u + 0x8;
		unsigned xy0_val = *(volatile unsigned*)XY_RAW;
		unsigned zr0_val = *(volatile unsigned*)ZR_RAW;
		res.X = (short)(xy0_val & 0xffff);
		res.Y = (short)((xy0_val >> 16) & 0xffff);
		res.Z = (short)(zr0_val & 0xffff);
		res.R = (short)((zr0_val >> 16) & 0xffff);
		return res;
	}
};


struct GYROManager {

	int16_t calibrateX[100];
	int16_t calibrateY[100];
	int16_t calibrateZ[100];

	volatile int16_t xAxis = 0;
	volatile int16_t yAxis = 0;
	volatile int16_t zAxis = 0;

	volatile int16_t xMax = 0;
	volatile int16_t yMax = 0;
	volatile int16_t zMax = 0;

	volatile int16_t xMin = 0;
	volatile int16_t yMin = 0;
	volatile int16_t zMin = 0;

	volatile int16_t xAdjust = 0;
	volatile int16_t yAdjust = 0;
	volatile int16_t zAdjust = 0;	// min and max values measured while at rest

	volatile float currentX = 0;
	volatile float currentY = 0;
	volatile float currentZ = 0;

	volatile int loop = 0;

	void* periph_addr;
	bool use_raw;

	volatile uint32_t btn_val;

	void init(void* periph_address, bool use_software) {
		periph_addr = periph_address;
		use_raw = use_software;
	}

	void calib() {
		if (use_raw) {
			return calib_raw();
		} else {
			// TODO: implement calib using IP
			return;
		}
	}


	void calib_raw() {
		print("Starting calibration in 10 seconds \nPlease place the gyro flat facing the monitor\n\n");
		usleep(10000000);
		// should be triggered by a button press or keyboard input
		// technically we are right now (by reset)

		print("Calibration starting!\nPlease leave the gyro at rest.\n");
		{
			for (int i = 0; i < 100; i++){
				auto raw_data = GyroRawData::get(periph_addr);
				calibrateX[i] = raw_data.X;
				calibrateY[i] = raw_data.Y;
				calibrateZ[i] = raw_data.Z;
				usleep(100000);
			}
			float calX, calY, calZ;
			for (int i = 0; i < 100; i++){
				calX += calibrateX[i];
				calY += calibrateY[i];
				calZ += calibrateZ[i];

				if (calibrateX[i] < xMin) xMin = calibrateX[i];
				if (calibrateX[i] > xMax) xMax = calibrateX[i];
				if (calibrateY[i] < yMin) yMin = calibrateY[i];
				if (calibrateY[i] > yMax) yMax = calibrateY[i];
				if (calibrateZ[i] < zMin) zMin = calibrateZ[i];
				if (calibrateZ[i] > zMax) zMax = calibrateZ[i];
			}
			xAdjust = calX / 100;
			yAdjust = calY / 100;
			zAdjust = calZ / 100;
		}
		print("Calibration ended.\n");
	}

	void poll() {
		if (use_raw) {
			return poll_raw();
		} else {
			// TODO: implement poll using IP
			return;
		}
	}

	void poll_raw() {

		float adjustedX = 0;
		float adjustedY = 0;
		float adjustedZ = 0;

		// current sensitivity, in degrees per second
		float dps = 250;
		// manually calculated, converts sensor units to degrees
		// would probably need to change is we tried
		float outputAdjust = 3.15 / dps;
//		print("Polling...\n\r");
		while (1) {
			auto raw_data = GyroRawData::get(periph_addr);
			xAxis = raw_data.X;
			yAxis = raw_data.Y;
			zAxis = raw_data.Z;
			btn_val = raw_data.R;

			// filter out small values (tend to fluctuate here when at rest)
			if (xAxis > xMin && xAxis < xMax){
			 adjustedX = 0;
			}
			else adjustedX = xAxis - xAdjust;
			if (yAxis > yMin && yAxis < yMax){
			 adjustedY = 0;
			}
			else adjustedY =  yAxis - yAdjust;
			if (zAxis > -100 && zAxis < 100){
			 adjustedZ = 0;
			}
			else adjustedZ = zAxis - zAdjust;


			// integrate to get our current angle
			currentX += (float)adjustedX * 0.001 * outputAdjust;	// 0.005 of a second has passed since our last measurement
			currentY += (float)adjustedY * 0.001 * outputAdjust;
			currentZ += (float)adjustedZ * 0.001 * outputAdjust;

			// don't print after every cycle
//			if (loop == 1000){
//				print("\n\n\n\n\n\n");	// clear previous values off screen (control codes don't seem to work)
//				print("Raw values:\n");
//				xil_printf("X Axis: %i\n", xAxis);
//				xil_printf("Y Axis: %i\n", yAxis);
//				xil_printf("Z Axis: %i\n\n", zAxis);
//
//				print("Adjusted values:\n");
//				xil_printf("X Axis: %i\n", adjustedX);
//				xil_printf("Y Axis: %i\n",adjustedY);
//				xil_printf("Z Axis: %i\n\n", adjustedZ);
//
//				print("Expected angles:\n");
//				xil_printf("X Axis: %i\n", (int)currentX);
//				xil_printf("Y Axis: %i\n", (int)currentY);
//				xil_printf("Z Axis: %i\n\n", (int)currentZ);
//				loop = 0;
//			}
			loop++;
//			break;
//			usleep(1000);
			break;
	   }
	}

	void reset() {
		currentX = 0.f;
		currentY = 0.f;
		currentZ = 0.f;
	}
};


#endif /* __MICROBLAZE__ */
#endif /* MICROBLAZE_GYRO_HPP_ */
