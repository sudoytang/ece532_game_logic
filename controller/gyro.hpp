/*
 * gyro.hpp
 *
 *  Created on: Mar 4, 2025
 *      Author: lunar
 */

#ifndef SRC_CONTROLLER_GYRO_HPP_
#define SRC_CONTROLLER_GYRO_HPP_




extern "C" {
#include "sleep.h"
#include "PmodGYRO.h"
}

static int16_t calibrateX[100];
static int16_t calibrateY[100];
static int16_t calibrateZ[100];

struct GYROManager {
	PmodGYRO myDevice;
	volatile int16_t xAxis = 0;
	volatile int16_t yAxis = 0;
	volatile int16_t zAxis = 0;

	volatile int16_t xMax = 0;
	volatile int16_t yMax = 0;
	volatile int16_t zMax = 0;
	volatile int16_t xMin = 0;
	volatile int16_t yMin = 0;
	volatile int16_t zMin = 0;
	volatile int16_t xAdjust;
	volatile int16_t yAdjust;
	volatile int16_t zAdjust;	// min and max values measured while at rest

	volatile float currentX = 0;
	volatile float currentY = 0;
	volatile float currentZ = 0;
	volatile int loop = 0;

	void init() {
		GYRO_begin(&myDevice,
			XPAR_PMODGYRO_0_AXI_LITE_SPI_BASEADDR,
			XPAR_PMODGYRO_0_AXI_LITE_GPIO_BASEADDR
		);
		// Set Threshold Registers
		GYRO_setThsXH(&myDevice, 0x0F);
		GYRO_setThsYH(&myDevice, 0x0F);
		GYRO_setThsZH(&myDevice, 0x0F);
		GYRO_enableInt1(&myDevice, GYRO_INT1_XHIE);    // Threshold interrupt
		GYRO_enableInt2(&myDevice, GYRO_REG3_I2_DRDY); // Data Rdy/FIFO interrupt
	}

	void calib() {
		print("Starting calibration in 10 seconds \nPlease place the gyro flat facing the monitor\n\n");
		usleep(10000000);
		// should be triggered by a button press or keyboard input
		// technically we are right now (by reset)

		print("Calibration starting!\nPlease leave the gyro at rest.\n");
		{
			for (int i = 0; i < 100; i++){
				calibrateX[i] = GYRO_getX(&myDevice);
				calibrateY[i] = GYRO_getY(&myDevice);
				calibrateZ[i] = GYRO_getZ(&myDevice);
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

			xAxis = GYRO_getX(&myDevice);
			yAxis = GYRO_getY(&myDevice);
			zAxis = GYRO_getZ(&myDevice);

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



#endif /* SRC_CONTROLLER_GYRO_HPP_ */
