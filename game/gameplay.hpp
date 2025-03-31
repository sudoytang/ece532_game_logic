/*
 * gameplay.hpp
 *
 *  Created on: Mar 30, 2025
 *      Author: lunar
 */

#ifndef SRC_GAME_GAMEPLAY_HPP_
#define SRC_GAME_GAMEPLAY_HPP_

#include "gameplaystate.hpp"

#ifdef __MICROBLAZE__
#include "../platform/microblaze/sprite_engine.hpp"
#endif





struct GamePlay {
    Car cars;
    Controller* controller;
    Map* map;
    int framestamp = 0;
#ifdef __MICROBLAZE__
    SpriteEngine sprite_engine;
    bool engine_initialized = false;
#endif



    std::array<GamePlayState, 2> states;
    std::array<int, 2> finish_framestamp;

    void init(Controller* con, Map* m, bool is_multiplayer) {
        map = m;
        controller = con;
        framestamp = 0;
        auto& active_map = map->map_data[map->sel];
        auto [x0, y0, d0] = active_map.respawn_points[0];
        auto [x1, y1, d1] = active_map.respawn_points[1];
        if (is_multiplayer) {
        	cars = Car(x0, y0, x1, y1, d0, d1);
        } else {
        	cars = Car(x0, y0, d0);
        }
        for (int id = 0; id < 1 + cars.has_2nd_car; id++) {
        	states[id] = GamePlayState{};
        	finish_framestamp[id] = 0;
        }
        sprite_engine.reset();
#ifdef __MICROBLAZE__
        engine_initialized = false;
        cars.init_sprite_engine(sprite_engine);
#endif

    }

#ifdef __MICROBLAZE__
    void init_engine(void* buffer) {
    	xil_printf("Initializing Sprite Engine...\n");
    	sprite_engine.setup(buffer);
    	engine_initialized = true;
    }
    void draw_dynamic_with_engine() {
    	cars.draw_with_engine(sprite_engine);
    }
#endif

    bool update() {
    	framestamp++;
    	auto impulse_0 = controller->getCarImpulse(0);
    	if (framestamp % 60 == 0) {
    		xil_printf("Car 0 Impulse: %d, %d\n", (int)(impulse_0.first * 100), (int)(impulse_0.second * 100));
    		xil_printf("Gyroscope 0 RAW: %d, %d\n", (int)(controller->gyro0.currentX * 100), (int)(controller->gyro0.currentY * 100));
    	}
    	cars.applyImpulseCartesian(0, impulse_0);
    	if (cars.has_2nd_car) {
        	cars.applyImpulseCartesian(1, controller->getCarImpulse(1));
    	}
        cars.update(*map, states);
        bool all_finished = true;
        for (int i = 0; i < 1 + cars.has_2nd_car; i++) {
        	if (states[i].state == GamePlayState::END) {
        		if (finish_framestamp[i] == 0) {
        			finish_framestamp[i] = framestamp;
        		}
        	} else {
        		all_finished = false;
        	}
        }
        return all_finished;
    }
    void draw_static(Draw screen_draw) {
    	map->draw(screen_draw);
    	screen_draw.FlushPixels(screen_draw.getSlice());
    }
    void draw_dynamic(Draw screen_draw) {
#ifdef __MICROBLAZE__
    	if (engine_initialized == false) {
    		init_engine(screen_draw.getFirstPixelAddress(screen_draw.getSlice()));
    	}
    	cars.draw_with_engine(sprite_engine);
#else
    	cars.draw(screen_draw);
#endif
    }
};



#endif /* SRC_GAME_GAMEPLAY_HPP_ */
