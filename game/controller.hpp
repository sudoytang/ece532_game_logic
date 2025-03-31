#ifndef GAME_CONTROLLER_HPP_
#define GAME_CONTROLLER_HPP_

#include <cstdint>
#include <utility>
#include "../platform/microblaze/gyro.hpp"
#include "../platform/microblaze/button.hpp"
#include "xparameters.h"

struct Controller {
    enum ControllerBinding {
        BIND_UNASSIGNED = 0b0000,
		BIND_GYRO0      = 0b0001,
		BIND_GYRO1      = 0b0010,
		BIND_BUTTON     = 0b0100,
		BIND_KEYBOARD   = 0b1000,
    };

    static constexpr float BUTTON_IMPULSE = 0.1f;
    static constexpr float GYRO_IMPULSE_SCALE = 1.0f/10000.f;
    static constexpr float GYRO_IMPULSE_THRESH = 100.f;

    ControllerBinding bindings[2] = {BIND_UNASSIGNED, BIND_UNASSIGNED};

    GYROManager gyro0;
    GYROManager gyro1;

    void GyroSubtick() {
    	if (bindings[0] == BIND_GYRO0 || bindings[1] == BIND_GYRO0) {
    		gyro0.poll();
    	}
    	if (bindings[0] == BIND_GYRO1 || bindings[1] == BIND_GYRO1) {
    		gyro1.poll();
    	}
    }

    void setBinding(int id, ControllerBinding binding) {
        bindings[id] = binding;
        if (binding == BIND_GYRO0) {
        	gyro0.init((void*)GYRO0_RAW_ADDR, true);
        	gyro0.calib();
        }
        if (binding == BIND_GYRO1) {
        	gyro1.init((void*)GYRO1_RAW_ADDR, true);
        	gyro1.calib();
        }
    }

    std::pair<float, float> getCarImpulse(int id) {
        switch (bindings[id]) {
        case BIND_GYRO0:
            return readGyro0();
            break;
        case BIND_GYRO1:
            return readGyro1();
            break;
        case BIND_BUTTON:
            return readButton();
            break;
        case BIND_KEYBOARD:
            return readKeyboard();
            break;
        default:
            return {0.f, 0.f};
            break;
        }
    };

    std::pair<float, float> readGyro0() {
        gyro0.poll();
        float impulse_x = 0, impulse_y = 0;
        impulse_x =
            (gyro0.currentX < GYRO_IMPULSE_THRESH) ?
            0.f :
            - gyro0.currentX * GYRO_IMPULSE_SCALE;
        impulse_y =
            (gyro0.currentY < GYRO_IMPULSE_THRESH) ?
            0.f :
            gyro0.currentY * GYRO_IMPULSE_SCALE;
        return {impulse_x, impulse_y};
    }
    void resetGyro0() {
        gyro0.reset();
    }
    void calibrateGyro0() {
        gyro0.calib();
    }

    std::pair<float, float> readGyro1() {
        gyro1.poll();
        float impulse_x = 0, impulse_y = 0;
        impulse_x =
            (gyro1.currentX < GYRO_IMPULSE_THRESH) ?
            0.f :
            gyro1.currentX * GYRO_IMPULSE_SCALE;
        impulse_y =
            (gyro1.currentY < GYRO_IMPULSE_THRESH) ?
            0.f :
            - gyro0.currentY * GYRO_IMPULSE_SCALE;
        return {impulse_x, impulse_y};
    }
    void resetGyro1() {
        gyro1.reset();
    }
    void calibrateGyro1() {
        gyro1.calib();
    }
    std::pair<float, float> readKeyboard() {
        // TODO!
    	return {};
    }

    std::pair<float, float> readButton() {
        uint16_t btn = GetButtonValue();
        float impulse_x = 0, impulse_y = 0;
        if (btn & BTNU) {
            impulse_y -= BUTTON_IMPULSE;
        }
        if (btn & BTND) {
            impulse_y += BUTTON_IMPULSE;
        }
        if (btn & BTNL) {
            impulse_x -= BUTTON_IMPULSE;
        }
        if (btn & BTNR) {
            impulse_x += BUTTON_IMPULSE;
        }
        return {impulse_x, impulse_y};
    }
    uint16_t btn_prev = 0;
    uint16_t GetButtonDownEvent() {
    	uint16_t btn = GetButtonValue();
    	uint16_t res = btn & ~btn_prev;
    	btn_prev = btn;
    	return res;
    }

    uint16_t GetSwitchesValue() {
    	volatile unsigned* switches = (unsigned*)(XPAR_AXI_GPIO_CTRL_BASEADDR + 8);
    	return (uint16_t)(*switches);
    }

};

#endif  // GAME_CONTROLLER_HPP_
