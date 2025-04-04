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

#include "gameplaystate.hpp"


#include "map.hpp"
#include "../graphics/draw/draw.hpp"
#include "../graphics/image/image.hpp"

#include "data/mario__combined.h"
#include "data/luigi__combined.h"

#ifdef __MICROBLAZE__
#include "../platform/microblaze/sprite_engine.hpp"
#endif


// Force applied per key press
// const float IMPULSE_FORCE = 0.5;
// Deceleration due to friction
const float FRICTION = 0.01;
constexpr int CAR_TILE_NUMBER = 8;
constexpr int CAR_TILE_SIZE = 32;
constexpr int COLLIBOX_Y0 = 20;
constexpr int COLLIBOX_Y1 = 27;
constexpr int COLLIBOX_X0 = 5;
constexpr int COLLIBOX_X1 = 26;
constexpr int COLLIBOX_XMID = (COLLIBOX_X0 + COLLIBOX_X1) / 2;



struct Car {

	enum CarDirection {
		RIGHT = 0,
		UPRIGHT = 1,
		UP = 2,
		UPLEFT = 3,
		LEFT = 4,
		DOWNLEFT = 5,
		DOWN = 6,
		DOWNRIGHT = 7,
	};

	struct CarTile {
		const void* tile_data;
		std::pair<int/*x*/, int/*y*/> GetTileSlicePosByDirection(CarDirection dir) const {
			switch (dir) {
			case RIGHT:		return {0, 32 * 4};
			case UPRIGHT:	return {0, 32 * 7};
			case UP:		return {0, 32 * 5};
			case UPLEFT:	return {0, 32 * 6};
			case LEFT:		return {0, 32 * 3};
			case DOWNLEFT:	return {0, 32 * 1};
			case DOWN: 		return {0, 32 * 0};
			case DOWNRIGHT:	return {0, 32 * 2};
			default:		return {0, 0};  // should not happen
			}
		}
		ImageSlice getTileByDirection(CarDirection dir) const {
			auto [slice_x, slice_y] = GetTileSlicePosByDirection(dir);
			return ImageView::fromMemory((void*)tile_data, 32, 32 * 8).slice(slice_x, slice_y, 32, 32);
		}
	};
	std::array<CarTile, 2> tiles = {{
		{(void*)img_mario__combined_data}, {(void*)img_luigi__combined_data}
	}};
	std::array<float, 2> x, y;		// Position of the car
	std::array<int, 2> prev_draw_x, prev_draw_y;
	std::array<float, 2> vx, vy;	// Velocity components
	std::array<float, 2> ax, ay;	// Acceleration components
	std::array<CarDirection, 2> d;		// Direction


#ifdef __MICROBLAZE__
	std::array<std::array<uint32_t, 8>, 2> dir_to_engine_tile_id;
	std::array<uint32_t, 2> engine_sprite_id;
#endif

    bool has_2nd_car;
    static constexpr int SIZE = CAR_TILE_SIZE;

    Car(): Car(0, 0, 0) {}
    Car(int x_init, int y_init, int d_init) {
    	// singleplayer
    	has_2nd_car = false;
    	x[0] = float(x_init);
    	y[0] = float(y_init);
    	d[0] = CarDirection(d_init);
    	prev_draw_x[0] = x_init;
    	prev_draw_y[0] = y_init;
    	vx[0] = vy[0] = ax[0] = ay[0] = 0.f;
    }
    Car(int x_init_0, int y_init_0, int x_init_1, int y_init_1, int d_0, int d_1) {
    	// multiplayer
    	has_2nd_car = true;
    	x = {float(x_init_0), float(x_init_1)};
    	y = {float(y_init_0), float(y_init_1)};
    	prev_draw_x = {x_init_0, x_init_1};
    	prev_draw_y = {y_init_0, y_init_1};
    	vx = {0, 0};
    	vy = {0, 0};
    	ax = {0, 0};
    	ay = {0, 0};
    	d  = {(CarDirection)d_0, (CarDirection)d_1};
    }
#ifdef __MICROBLAZE__
    void init_sprite_engine(SpriteEngine& engine) {
    	xil_printf("Initializing Car Sprites...\n");
    	for (int id = 0; id < 1 + has_2nd_car; id++) {
    		for (int dir = 0; dir < 8; dir++) {
    			dir_to_engine_tile_id[id][dir] = engine.tile32_table.newTile32(
					tiles[id].getTileByDirection((CarDirection)dir)
				);
    		}
    		SpriteInfo info;
    		info.is_32 = true;
    		info.tile_idx = dir_to_engine_tile_id[id][d[id]];
    		info.sprite_x = x[id];
    		info.sprite_y = y[id];
    		engine_sprite_id[id] = engine.newSprite(info);
    	}
    	engine.tile32_table.flush();
    	engine.flush();
    }
#endif

    void applyImpulseCartesian(int id, float impulse_x, float impulse_y) {
    	ax[id] += impulse_x;
    	ay[id] += impulse_y;
    }
    void applyImpulseCartesian(int id, std::pair<float, float> impulse_xy) {
    	applyImpulseCartesian(id, impulse_xy.first, impulse_xy.second);
    }

    static CarDirection calcDirection(float vx, float vy, CarDirection last_dir) {
    	// using vy / vx to determine direction
    	if (vx == 0) {
    		if (vy == 0) {
    			return last_dir;
    		} else if (vy > 0) {
    			// down
    			return DOWN;
    		} else {
    			return UP;
    		}
    	} else if (vx < 0) {
			float slope = vy / vx;
			if (slope > +2.0f) return UP;
			if (slope > +0.5f) return UPLEFT;
			if (slope > -0.5f) return LEFT;
			if (slope > -2.0f) return DOWNLEFT;
			return DOWN;
    	} else {
    		float slope = vy / vx;
			if (slope > +2.0f) return DOWN;
			if (slope > +0.5f) return DOWNRIGHT;
			if (slope > -0.5f) return RIGHT;
			if (slope > -2.0f) return UPRIGHT;
			return UP;
    	}
    }

    static std::pair<bool /* x-collide */, bool /* y-collide */>
    calcMapCollision(const Map& map, float pos_x, float pos_y, float v_x, float v_y) {
    	int ipos_x = int(pos_x);
    	int ipos_y = int(pos_y);

    	bool collide_x = false;
    	bool collide_y = false;
    	if (v_x > 0) {
    		v_x++;
    	} else if (v_x < 0) {
    		v_x--;
    	}
    	if (v_y > 0) {
    		v_y++;
    	} else if (v_x < 0) {
    		v_y--;
    	}
    	// test x-axis
    	for (int py = ipos_y + COLLIBOX_Y0; py <= ipos_y + COLLIBOX_Y1; py++)
    	for (int px = ipos_x + COLLIBOX_X0; px <= ipos_x + COLLIBOX_X1; px++) {
    		if (map[py][px]) {
    			xil_printf("already stuck!\n");
    		}
    		if (map[int(py + v_y)][px]) {
    			collide_y = true;
    		}
    		if (map[py][int(px + v_x)]) {
    			collide_x = true;
    		}
    		if (collide_x && collide_y) {
    			return {collide_x, collide_y};
    		}
    		if (!collide_x && !collide_y && map[int(py + v_y)][int(px + v_x)]) {
    			collide_x = true;
    			collide_y = true;
    			return {collide_x, collide_y};
    		}
    	}




//    	if (map[ipos_y + COLLIBOX_Y0][new_x + COLLIBOX_X0]
//		||  map[ipos_y + COLLIBOX_Y0][new_x + COLLIBOX_XMID]
//		||  map[ipos_y + COLLIBOX_Y0][new_x + COLLIBOX_X1] // top 3 points
//		||  map[ipos_y + COLLIBOX_Y1][new_x + COLLIBOX_X0]
//		||  map[ipos_y + COLLIBOX_Y1][new_x + COLLIBOX_XMID]
//		||  map[ipos_y + COLLIBOX_Y1][new_x + COLLIBOX_X1] // bottom 3 points
//    	) {
//    		collide_x = true;
//    	}
//    	// test y-axis
//    	if (map[new_y + COLLIBOX_Y0][ipos_x + COLLIBOX_X0]
//		||  map[new_y + COLLIBOX_Y0][ipos_x + COLLIBOX_XMID]
//		||  map[new_y + COLLIBOX_Y0][ipos_x + COLLIBOX_X1] // top 3 points
//		||  map[new_y + COLLIBOX_Y1][ipos_x + COLLIBOX_X0]
//		||  map[new_y + COLLIBOX_Y1][ipos_x + COLLIBOX_XMID]
//		||  map[new_y + COLLIBOX_Y1][ipos_x + COLLIBOX_X1] // bottom 3 points
//    	) {
//    		collide_y = true;
//    	}
//    	if (collide_x || collide_y) {
//        	return {collide_x, collide_y};
//    	}

    	// there is a problem if the corner of the car hits the boundary:
    	// if [ipos_y, new_x] is ok, and [new_y, iposx] is ok
    	// but [ipos_y, ipos_x] is not ok


//    	if (map[new_y + COLLIBOX_Y0][new_x + COLLIBOX_X0]
//		||  map[new_y + COLLIBOX_Y0][new_x + COLLIBOX_XMID]
//		||  map[new_y + COLLIBOX_Y0][new_x + COLLIBOX_X1] // top 3 points
//		||  map[new_y + COLLIBOX_Y1][new_x + COLLIBOX_X0]
//		||  map[new_y + COLLIBOX_Y1][new_x + COLLIBOX_XMID]
//		||  map[new_y + COLLIBOX_Y1][new_x + COLLIBOX_X1] // bottom 3 points
//    	) {
//    		collide_x = true;
//    		collide_y = true;
//    	}

    	return {collide_x, collide_y};
    }
    std::pair<bool /* x-collide */, bool /* y-collide */>
    calcCarsCollision() {
    	float x0 = x[0];
    	float y0 = y[0];
    	float x1 = x[1];
    	float y1 = y[1];
    	float min_xr = std::min(x0 + COLLIBOX_X1, x1 + COLLIBOX_X1);
    	float max_xl = std::max(x0 + COLLIBOX_X0, x1 + COLLIBOX_X0);
    	float min_yd = std::min(y0 + COLLIBOX_Y1, y1 + COLLIBOX_Y1);
    	float max_yu = std::max(y0 + COLLIBOX_Y0, y1 + COLLIBOX_Y0);
    	float overlap_x = min_xr - max_xl;
    	float overlap_y = min_yd - max_yu;
    	// min_xr means the right bound of the car at the left
    	// max_xl means the left bound of the car at the right
    	// min_xr - max_xl > 0 means two cars have overlap on x-axis

    	if (overlap_x > 0 && overlap_y > 0) {
    		xil_printf("WARNING: Car collision detected before updating!\n");
    	}

    	x0 = x[0] + vx[0];
    	y0 = y[0] + vy[0];
    	x1 = x[1] + vx[1];
    	y1 = y[1] + vy[1];
    	min_xr = std::min(x0 + COLLIBOX_X1, x1 + COLLIBOX_X1);
    	max_xl = std::max(x0 + COLLIBOX_X0, x1 + COLLIBOX_X0);
    	min_yd = std::min(y0 + COLLIBOX_Y1, y1 + COLLIBOX_Y1);
    	max_yu = std::max(y0 + COLLIBOX_Y0, y1 + COLLIBOX_Y0);

    	float nxt_overlap_x = min_xr - max_xl;
    	float nxt_overlap_y = min_yd - max_yu;

    	bool res_x = false, res_y = false;
    	if (nxt_overlap_x > 0 && nxt_overlap_y > 0) {
    		// ok now we have a collision
    		// determine the axis of collision
    		if (overlap_x <= 0) {
    			// if last time cars didn't overlap on this axis, then this is the axis to blame
    			res_x = true;
    		}
    		if (overlap_y <= 0) {
    			res_y = true;
    		}
    	}

    	return {res_x, res_y};
    }

    void update(const Map& map, std::array<GamePlayState, 2>& states) {
    	std::array<std::pair<bool/*x*/, bool/*y*/>, 2> map_collision;
        for (int id = 0; id < 1 + has_2nd_car; id++) {
            // Update velocity with acceleration
            vx[id] += ax[id];
            vy[id] += ay[id];

            // Apply friction (deceleration)
            vx[id] *= (1 - FRICTION);
            vy[id] *= (1 - FRICTION);

            map_collision[id] = calcMapCollision(map, x[id], y[id], vx[id], vy[id]);
            // Wall collision
            if (map_collision[id].first) {
            	vx[id] = 0.f;
            }
            if (map_collision[id].second) {
            	vy[id] = 0.f;
            }
        }

        // Collision between cars
        // assuming completely inelastic collision
        if (has_2nd_car) {
            auto [collide_x, collide_y]  = calcCarsCollision();
            if (collide_x) {
            	if (map_collision[0].first || map_collision[1].first) {
            		vx[0] = 0.f;
            		vx[1] = 0.f;
            	} else {
                    float v = (vx[0] + vx[1])/2;
                    vx[0] = v;
                    vx[1] = v;
            	}
            }
            if (collide_y) {
            	if (map_collision[0].second || map_collision[1].second) {
            		vy[0] = 0.f;
            		vy[1] = 0.f;
            	} else {
                    float v = (vy[0] + vy[1])/2;
                    vy[0] = v;
                    vy[1] = v;
            	}
            }
        }

        auto IsPointInTrigger = [](int x, int y, int tx, int ty, int tw, int th) -> bool {
        	int xmin = tx;
        	int xmax = tx + tw;
        	int ymin = ty;
        	int ymax = ty + th;

        	return x >= xmin && x < xmax && y >= ymin && y < ymax;
        };

        // check hit of trigger points
        auto& active_map_data = map.map_data[map.sel];
        for (int id = 0; id < 1 + has_2nd_car; id++) {
        	auto [sx, sy, sw, sh] = active_map_data.start_end_collibox;
        	int ix = int(x[id]);
        	int iy = int(y[id]);
        	if (IsPointInTrigger(ix + COLLIBOX_X0, 	 iy + COLLIBOX_Y0, sx, sy, sw, sh)
        	 ||	IsPointInTrigger(ix + COLLIBOX_XMID, iy + COLLIBOX_Y0, sx, sy, sw, sh)
			 || IsPointInTrigger(ix + COLLIBOX_X1,   iy + COLLIBOX_Y0, sx, sy, sw, sh)
			 || IsPointInTrigger(ix + COLLIBOX_X0, 	 iy + COLLIBOX_Y1, sx, sy, sw, sh)
        	 ||	IsPointInTrigger(ix + COLLIBOX_XMID, iy + COLLIBOX_Y1, sx, sy, sw, sh)
			 || IsPointInTrigger(ix + COLLIBOX_X1,   iy + COLLIBOX_Y1, sx, sy, sw, sh)) {
        		states[id].onHitFinishLine();
        	}

        	auto [hx, hy, hw, hh] = active_map_data.halfway_collibox;
        	if (IsPointInTrigger(ix + COLLIBOX_X0, 	 iy + COLLIBOX_Y0, hx, hy, hw, hh)
        	 ||	IsPointInTrigger(ix + COLLIBOX_XMID, iy + COLLIBOX_Y0, hx, hy, hw, hh)
			 || IsPointInTrigger(ix + COLLIBOX_X1,   iy + COLLIBOX_Y0, hx, hy, hw, hh)
			 || IsPointInTrigger(ix + COLLIBOX_X0, 	 iy + COLLIBOX_Y1, hx, hy, hw, hh)
        	 ||	IsPointInTrigger(ix + COLLIBOX_XMID, iy + COLLIBOX_Y1, hx, hy, hw, hh)
			 || IsPointInTrigger(ix + COLLIBOX_X1,   iy + COLLIBOX_Y1, hx, hy, hw, hh)) {
        		states[id].onHitHalfwayLine();
        	}
        }

        // update position
        for (int id = 0; id < 1 + has_2nd_car; id++) {
            x[id] += vx[id];
            y[id] += vy[id];

            // Reset acceleration for the next frame
            ax[id] = 0;
            ay[id] = 0;
        }

        // Update direction (8-way)
        for (int id = 0; id < 1 + has_2nd_car; id++) {
        	d[id] = calcDirection(vx[id], vy[id], d[id]);
        }
    }


    std::pair<
		std::tuple<int /*x*/, int /*y*/, int /*width*/, int /*height*/>,
		std::tuple<int /*x*/, int /*y*/, int /*width*/, int /*height*/>
    >
	draw(Draw screen_draw) {
    	decltype(draw(std::declval<Draw>())) res;
		for (int id = 0; id < 1 + has_2nd_car; id++) {
			int draw_x = int(x[id]);
			int draw_y = int(y[id]);
			const ImageSlice tile = tiles[id].getTileByDirection(d[id]);
			screen_draw.drawRect(prev_draw_x[id], prev_draw_y[id], tile.width, tile.height, C_TRANSPARENT, true);
			screen_draw.FlushPixels(
				screen_draw.getSlice().slice(prev_draw_x[id], prev_draw_y[id], tile.width, tile.height)
			);
			screen_draw.drawImageSlice(draw_x, draw_y, tile);
			screen_draw.FlushPixels(
				screen_draw.getSlice().slice(draw_x, draw_y, tile.width, tile.height)
			);

			if (id == 0) {
				std::get<0>(res) = {draw_x, draw_y, tile.width, tile.height};
			} else {
				std::get<1>(res) = {draw_x, draw_y, tile.width, tile.height};
			}
			prev_draw_x[id] = draw_x;
			prev_draw_y[id] = draw_y;
		}
		return res;
	}

#ifdef __MICROBLAZE__
    void draw_with_engine(SpriteEngine& engine) {
    	for (int id = 0; id < 1 + has_2nd_car; id++) {
    		auto info = engine.sprite_table[engine_sprite_id[id]];
    		info.tile_idx = dir_to_engine_tile_id[id][d[id]];
    		info.sprite_x = uint32_t(x[id]);
    		info.sprite_y = uint32_t(y[id]);
    		engine.updateSprite(engine_sprite_id[id], info);
    		engine.flush();
    	}
    	engine.draw();
    }
#endif
};



#endif /* SRC_GAME_CAR_HPP_ */
