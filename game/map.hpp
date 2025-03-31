/*
 * map.hpp
 *
 *  Created on: Mar 3, 2025
 *      Author: lunar
 */

#ifndef SRC_GAME_MAP_HPP_
#define SRC_GAME_MAP_HPP_

#include <cstring>
#include <tuple>
#include <array>

#include "../graphics/image/image.hpp"
#include "../graphics/draw/draw.hpp"
#include "../graphics/color/color.hpp"
#include "data/map_0.h"
#include "data/map_1.h"
#include "data/map_2.h"

// assuming all maps are 800x600
struct MapData {
	const char* map_name;
	const void* map_image_src;
	const std::array<std::tuple<int/*x*/, int/*y*/, int/*direction*/>, 2> respawn_points;
	const std::tuple<int/*x*/, int/*y*/, int/*w*/, int/*h*/> start_end_collibox;
	const std::tuple<int/*x*/, int/*y*/, int/*w*/, int/*h*/> halfway_collibox;
};



struct Map {
	enum MapSelection {
		MAP_0,
		MAP_1,
		MAP_2,
		MAP_END
	};
    MapSelection sel = MAP_0;

    const std::array<MapData, MAP_END> map_data = {{
		{  // track 0
			.map_name = "Track 0",
			.map_image_src = img_map_0_data,
			.respawn_points = {{
				// respawn_point of map0, car0
				std::make_tuple(300, 490, 0),  // the third number is the direction index (see car.hpp)
				// respawn_point of map0, car1
				std::make_tuple(300, 520, 0),
			}},
			.start_end_collibox = std::make_tuple(340, 505, 16, 55),
			.halfway_collibox = std::make_tuple(150, 192, 16, 56),
		},
		{  // track 1
			.map_name = "Track 1",
			.map_image_src = img_map_1_data,
			.respawn_points = {{
				// respawn_point of map1, car0
				std::make_tuple(390, 300, 6),
				// respawn_point of map1, car1
				std::make_tuple(430, 300, 6),
			}},
			.start_end_collibox = std::make_tuple(370, 340, 115, 16),
			.halfway_collibox = std::make_tuple(365, 30, 16, 65),
		},
		{  // track 2
			.map_name = "Track 2",
			.map_image_src = img_map_2_data,
			.respawn_points = {{
				// respawn_point of map2, car0
				std::make_tuple(710, 520, 2),
				// respawn_point of map2, car1
				std::make_tuple(740, 520, 2),
			}},
			.start_end_collibox = std::make_tuple(697, 500, 70, 16),
			.halfway_collibox = std::make_tuple(41, 280, 80, 16),
		}
    }};

	uint16_t map_image[600][800];
    bool map[600][800];


    void init(MapSelection select) {
        //create map
    	sel = select;
    	auto& active_map = map_data[sel];
    	memcpy(map_image, active_map.map_image_src, sizeof(Color)*800*600);
    	for (int y = 0; y < 600; y++)
    	for (int x = 0; x < 800; x++) {
    		map[y][x] = map_image[y][x] > 0xF000;
    	}
    	auto [xstart, ystart, wstart, hstart] = active_map.start_end_collibox;
    	for (int h = 0; h < hstart; h++) {
    		for (int w = 0; w < wstart; w++) {
    			auto color = (h / 8 + w / 8) % 2 == 1 ? C_WHITE : C_BLACK;
    			map_image[ystart + h][xstart + w] = color.value;
    		}
    	}
    	auto [xhalf, yhalf, whalf, hhalf] = active_map.halfway_collibox;
    	for (int h = 0; h < hhalf; h++) {
    		for (int w = 0; w < whalf; w++) {
    			auto color = C_CYAN;
    			map_image[yhalf + h][xhalf + w] = color.value;
    		}
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
