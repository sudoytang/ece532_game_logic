/*
 * map.hpp
 *
 *  Created on: Mar 3, 2025
 *      Author: lunar
 */

#ifndef SRC_GAME_MAP_HPP_
#define SRC_GAME_MAP_HPP_

#include <cstring>

#include "../graphics/image/image.hpp"
#include "../graphics/draw/draw.hpp"
#include "../graphics/color/color.hpp"
#include "map_data.h"
struct Map {
    uint16_t map_image[600][800];
    bool map[600][800];

    void init() {
        //create map
    	memcpy(map_image, map_data, sizeof(Color)*800*600);
    	for (int y = 0; y < 600; y++)
    	for (int x = 0; x < 800; x++) {
    		map[y][x] = map_image[y][x] > 0xF000;
    	}
    }
    bool* operator[](size_t index) {
    	return map[index];
    }
    const bool* operator[](size_t index) const {
    	return map[index];
    }
    void draw(Draw screen_draw) {
    	screen_draw.drawImageView(0, 0, ImageView::fromMemory(map_image, 800, 600));
    }
};



#endif /* SRC_GAME_MAP_HPP_ */
