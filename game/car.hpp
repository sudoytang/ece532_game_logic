/*
 * car.hpp
 *
 *  Created on: Mar 3, 2025
 *      Author: lunar
 */

#ifndef SRC_GAME_CAR_HPP_
#define SRC_GAME_CAR_HPP_


#include <cstdint>
#include <cmath>
#include <tuple>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


#include "map.hpp"
#include "../graphics/draw/draw.hpp"
#include "../graphics/image/image.hpp"

// Force applied per key press
#define IMPULSE_FORCE 1
// const float IMPULSE_FORCE = 0.5;
// Deceleration due to friction
const float FRICTION = 0.05;



class Car {
private:
    float x, y;       	// Position of the car
    float vx, vy;     	// Velocity components
    float ax, ay;     	// Acceleration components
    Image tile[3];

    static constexpr int SIZE = 16;
    Map* map;

public:
    Car(uint16_t x_init, uint16_t y_init, Map* map)
	: x(x_init), y(y_init),
	  vx(0), vy(0),
	  ax(0), ay(0),
	  map(map) {
    	tile[0] = Image::fromBlank(SIZE, SIZE, C_YELLOW);
    	tile[1] = Image::fromBlank(SIZE, SIZE, C_CYAN);
    	tile[2] = Image::fromBlank(SIZE, SIZE, C_MAGENTA);
    }

    void applyImpulsePolar(int direction, float impulse = IMPULSE_FORCE) {
        // TODO: The direction may be reversed
    	// CHANGE made by yushun:
    	// I changed this to standard polar coordinate system
    	// 0 degree - right
    	// 90 degree - up
    	applyImpulseCartesian(
			impulse * cos(direction*M_PI/180),
			- impulse * sin(direction*M_PI/180)
    	);
    }
    void applyImpulseCartesian(float impulse_x, float impulse_y) {
    	ax += impulse_x;
    	ay += impulse_y;
    }

    float getX() { return x; }
    float getY() { return y; }
    float getVX() { return vx; }
    float getVY() { return vy; }
    float getAX() { return ax; }
    float getAY() { return ay; }

    void update() {
        // Update velocity with acceleration
        vx += ax;
        vy += ay;

        // Apply friction (deceleration)
        vx *= (1 - FRICTION);
        vy *= (1 - FRICTION);

        // Update position with velocity
        if ((*map)[int(y)][int(x + vx)]
		 || (*map)[int(y)][int(x + vx + SIZE)]
		 || (*map)[int(y + SIZE)][int(x + vx)]
		 || (*map)[int(y + SIZE)][int(x + vx + SIZE)]
		)
            vx = 0;
        if ((*map)[int(y + vy)][int(x)]
		 || (*map)[int(y + vy)][int(x + SIZE)]
		 || (*map)[int(y + vy + SIZE)][int(x)]
		 || (*map)[int(y + vy + SIZE)][int(x + SIZE)]
		)
            vy = 0;
        x += vx;
        y += vy;

        // Reset acceleration for the next frame
        ax = 0;
        ay = 0;
    }

    std::tuple<int /*x*/, int /*y*/, int /*width*/, int /*height*/>
    draw(Draw screen_draw, int tile_id) const {
    	screen_draw.drawImage(int(x), int(y), tile[tile_id]);
    	return {int(x), int(y), tile[tile_id].width, tile[tile_id].height};
    }
};



#endif /* SRC_GAME_CAR_HPP_ */
