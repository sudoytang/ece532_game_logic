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

constexpr int Y0 = 20;
constexpr int Y1 = 27;
constexpr int X0 = 5;
constexpr int X1 = 26;
constexpr int Xmid = 15;




class Car {
public:
    float x[2], y[2];       	// Position of the car
    float vx[2], vy[2];     	// Velocity components
    float ax[2], ay[2];     	// Acceleration components
    uint16_t d[2];              // Direction
    Image tile[3];

    static constexpr int SIZE = 16;
    Map* map;

public:
    Car(uint16_t x_init_0, uint16_t y_init_0, uint16_t x_init_1, uint16_t y_init_1, Map* map)
	: 
	  map(map) {
    	tile[0] = Image::fromBlank(SIZE, SIZE, C_YELLOW);
    	tile[1] = Image::fromBlank(SIZE, SIZE, C_CYAN);
    	tile[2] = Image::fromBlank(SIZE, SIZE, C_MAGENTA);
        x[0] = x_init_0;
        y[0] = y_init_0;
        x[1] = x_init_1;
        y[1] = y_init_1;
        vx[0] = 0;
        vy[0] = 0;
        vx[1] = 0;
        vy[1] = 0;
        ax[0] = 0;
        ay[0] = 0;
        ax[1] = 0;
        ay[1] = 0;
        d[0] = 0;
        d[1] = 0;
    }

    void applyImpulsePolar(int id, int direction, float impulse = IMPULSE_FORCE) {
        // TODO: The direction may be reversed
    	// CHANGE made by yushun:
    	// I changed this to standard polar coordinate system
    	// 0 degree - right
    	// 90 degree - up
    	applyImpulseCartesian(
            id,
			impulse * cos(direction*M_PI/180),
			- impulse * sin(direction*M_PI/180)
    	);
    }
    void applyImpulseCartesian(int id, float impulse_x, float impulse_y) {
    	ax[id] += impulse_x;
    	ay[id] += impulse_y;
    }

    // float getX() { return x; }
    // float getY() { return y; }
    // float getVX() { return vx; }
    // float getVY() { return vy; }
    // float getAX() { return ax; }
    // float getAY() { return ay; }

    void update() {
        for (int id = 0; id < 2; id++) {
            // Update velocity with acceleration
            vx[id] += ax[id];
            vy[id] += ay[id];

            // Apply friction (deceleration)
            vx[id] *= (1 - FRICTION);
            vy[id] *= (1 - FRICTION);

            // Wall collision
            if ((*map)[int(y[id]) + Y0][int(x[id] + vx[id]) + X0]
            ||  (*map)[int(y[id]) + Y0][int(x[id] + vx[id]) + Xmid]
            ||  (*map)[int(y[id]) + Y0][int(x[id] + vx[id]) + X1] // top 3 points
            ||  (*map)[int(y[id]) + Y1][int(x[id] + vx[id]) + X0]
            ||  (*map)[int(y[id]) + Y1][int(x[id] + vx[id]) + Xmid]
            ||  (*map)[int(y[id]) + Y1][int(x[id] + vx[id]) + X1] // bottom 3 points
            )
                vx[id] = 0;
            if ((*map)[int(y[id] + vy[id]) + Y0][int(x[id]) + X0]
            ||  (*map)[int(y[id] + vy[id]) + Y0][int(x[id]) + Xmid]
            ||  (*map)[int(y[id] + vy[id]) + Y0][int(x[id]) + X1]
            ||  (*map)[int(y[id] + vy[id]) + Y1][int(x[id]) + X0]
            ||  (*map)[int(y[id] + vy[id]) + Y1][int(x[id]) + Xmid]
            ||  (*map)[int(y[id] + vy[id]) + Y1][int(x[id]) + X1]
            )
                vy[id] = 0;

        }
        
        // Collision between cars
        if (std::abs(x[0] + vx[0] - x[1] - vx[1]) < (X1-X0)) {
            float v = (vx[0] + vx[1])/2;
            vx[0] = v;
            vx[1] = v;
        }
        if (std::abs(y[0] + vy[0] - y[1] - vy[1]) < (Y1-Y0)) {
            float v = (vy[0] + vy[1])/2;
            vy[0] = v;
            vy[1] = v;
        }

        for (int id = 0; id < 2; id++) {
            x[id] += vx[id];
            y[id] += vy[id];

            // Reset acceleration for the next frame
            ax[id] = 0;
            ay[id] = 0;
        }

        // Update direction
        for (int id = 0; id < 2; id++) {
            int direction;
            if(vx[id] == 0)
                direction = 90 + (int)(2*(vy[id]>0)*90);
            else if(vx[id]>=0 && vy[id]<=0)
                direction = (int)(atan(-vy[id]/vx[id])*180/M_PI);
            else if(vx[id]<0 && vy[id]<=0)
                direction = (int)(180-atan(vy[id]/vx[id])*180/M_PI);
            else if(vx[id]<0 && vy[id]<0)
                direction = (int)(360-atan(vy[id]/vx[id])*180/M_PI);
            else
                direction = (int)(180+atan(-vy[id]/vx[id])*180/M_PI);

            if (vx[id] != 0 || vy[id] != 0)
                d[id] = direction;
        }
    }



    // std::tuple<int /*x*/, int /*y*/, int /*width*/, int /*height*/>
    // draw(Draw screen_draw, int tile_id) const {
    // 	screen_draw.drawImage(int(x), int(y), tile[tile_id]);
    // 	return {int(x), int(y), tile[tile_id].width, tile[tile_id].height};
    // }
};



#endif /* SRC_GAME_CAR_HPP_ */