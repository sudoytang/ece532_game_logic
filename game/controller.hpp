#ifndef GAME_CONTROLLER_HPP_
#define GAME_CONTROLLER_HPP_

#include <cstdint>
#include <utility>
#include "../platform/microblaze/gyro.hpp"
#include "xparameters.h"
#include "../platform/microblaze/button_switch.hpp"
#include "../platform/microblaze/keyboard.hpp"

struct Controller {
    enum ControllerBinding {
        BIND_UNASSIGNED = 0b0000,
		BIND_GYRO0      = 0b0001,
		BIND_GYRO1      = 0b0010,
		BIND_BUTTON     = 0b0100,
		BIND_KEYBOARD   = 0b1000,
    };

    static constexpr float BUTTON_IMPULSE = 0.1f;
    static constexpr float GYRO_IMPULSE_SCALE = 1.0f/5000.f;
    static constexpr short GYRO_IMPULSE_THRESH = 100;

    ControllerBinding bindings[2] = {BIND_UNASSIGNED, BIND_UNASSIGNED};

    GYROManager gyro0;
    GYROManager gyro1;
    Keyboard kb;

    void init() {
    	gyro0.init(0);
    	gyro1.init(1);
    	kb.init();
    }

    void setBinding(int id, ControllerBinding binding) {
        bindings[id] = binding;
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
    	short x, y, z;
        gyro0.read(x, y, z);
//        xil_printf("GYRO0: %d, %d, %d\n", x, y, z);
        float impulse_x = 0, impulse_y = 0;
        impulse_x =
            (std::abs(x) < GYRO_IMPULSE_THRESH) ?
            0.f :
            x * GYRO_IMPULSE_SCALE;
        impulse_y =
            (std::abs(y) < GYRO_IMPULSE_THRESH) ?
            0.f :
            y * GYRO_IMPULSE_SCALE;
        return {impulse_x, impulse_y};
    }
    void resetGyro0() {
        gyro0.reset();
    }
    void calibrateGyro0() {
        gyro0.calib();
    }

    std::pair<float, float> readGyro1() {
    	short x, y, z;
        gyro1.read(x, y, z);

        float impulse_x = 0, impulse_y = 0;
        impulse_x =
            (std::abs(x) < GYRO_IMPULSE_THRESH) ?
            0.f :
            -x * GYRO_IMPULSE_SCALE;
        impulse_y =
            (std::abs(y) < GYRO_IMPULSE_THRESH) ?
            0.f :
            -y * GYRO_IMPULSE_SCALE;
        return {impulse_x, impulse_y};
    }
    void resetGyro1() {
        gyro1.reset();
    }
    void calibrateGyro1() {
        gyro1.calib();
    }
    std::pair<float, float> readKeyboard() {
        float impulse_x = 0, impulse_y = 0;

        if (kb.key_w) {
            impulse_y -= BUTTON_IMPULSE;
        }
        if (kb.key_s) {
            impulse_y += BUTTON_IMPULSE;
        }
        if (kb.key_a) {
            impulse_x -= BUTTON_IMPULSE;
        }
        if (kb.key_d) {
            impulse_x += BUTTON_IMPULSE;
        }
//        xil_printf("Read Keyboard impulse %d%%, %d%%\n", (int)(100 * impulse_x), (int)(100 * impulse_y));
        return {impulse_x, impulse_y};
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

    uint16_t sw_prev = 0;
    std::pair<uint16_t/*on*/, uint16_t/*off*/> GetSwitchOnOffEvent() {
    	uint16_t sw = GetSwitchValue();
    	uint16_t on_res = sw & ~sw_prev;
    	uint16_t off_res = ~sw & sw_prev;
    	sw_prev = sw;
    	if (on_res || off_res) {
    		xil_printf("Switch: %x\n", sw);
    	}
    	return {on_res, off_res};
    }

};

#endif  // GAME_CONTROLLER_HPP_
