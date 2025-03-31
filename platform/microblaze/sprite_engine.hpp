/*
 * sprite_engine.hpp
 *
 *  Created on: Mar 30, 2025
 *      Author: lunar
 */

#ifndef SRC_PLATFORM_MICROBLAZE_SPRITE_ENGINE_HPP_
#define SRC_PLATFORM_MICROBLAZE_SPRITE_ENGINE_HPP_
#ifdef __MICROBLAZE__
#include <array>
#include <cstdint>
#include "../../graphics/color/color.hpp"
#include "../../graphics/image/image.hpp"
#include "xil_cache.h"

struct Tile16 {
	std::array<std::array<Color, 16>, 16> data;
};
struct Tile32 {
	std::array<std::array<Color, 32>, 32> data;
};

struct SpriteInfo {
	uint32_t sprite_x 	: 12;
	uint32_t sprite_y 	: 12;
	uint32_t tile_idx 	: 7;
	uint32_t is_32		: 1;

	static SpriteInfo getInvalid() {
		uint32_t res = 0xFFFFFFFF;
		return *(SpriteInfo*)&res;
	}
};

static_assert(sizeof(SpriteInfo) == sizeof(uint32_t));


struct Tile32Table {
	std::array<Tile32, 128> tile32_table;
	std::array<bool, 128> tile32_usage;
	SpriteInfo* sprite_entries;

	Tile32Table(SpriteInfo* info): tile32_table{}, tile32_usage{}, sprite_entries(info) {}

	uint32_t newTile32(ImageSlice img) {
		for (uint32_t i = 0; i < 128; i++) {
			if (tile32_usage[i] == false) {
				for (int y = 0; y < 32; y++) {
					for (int x = 0; x < 32; x++) {
						tile32_table[i].data[y][x] = img.get(x, y);
					}
				}
				tile32_usage[i] = true;
				return i;
			}
		}
		return (uint32_t)-1;

	}
	bool updateTile32(int i, ImageSlice img) {
		if (tile32_usage[i] == false) {
			return false;
		}
		for (int y = 0; y < 32; y++) {
			for (int x = 0; x < 32; x++) {
				tile32_table[i].data[y][x] = img.get(x, y);
			}
		}
		return true;

	}
	bool deleteTile32(int i) {
		if (tile32_usage[i] == false) {
			return false;
		}
		tile32_usage[i] = false;
		for (int j = 0; j < 32; j++) {
			// remove sprites of this tile
			if (sprite_entries[j].is_32 == false && sprite_entries[j].tile_idx == i) {
				sprite_entries[j] = SpriteInfo::getInvalid();
			}
		}
		return true;
	}
	void flush() {
		Xil_DCacheFlushRange((UINTPTR)tile32_table.data(), sizeof(tile32_table));
	}
	void reset() {
		for (int i = 0; i < 128; i++) {
			tile32_usage[i] = false;
		}
	}
};

struct Tile16Table {
	std::array<Tile16, 128> tile16_table;
	std::array<bool, 128> tile16_usage;
	SpriteInfo* sprite_entries;

	Tile16Table(SpriteInfo* info): tile16_table{}, tile16_usage{}, sprite_entries(info) {}

	uint32_t newTile16(ImageSlice img) {

		for (uint32_t i = 0; i < 128; i++) {
			if (tile16_usage[i] == false) {
				for (int y = 0; y < 16; y++) {
					for (int x = 0; x < 16; x++) {
						tile16_table[i].data[y][x] = img.get(x, y);
					}
				}
				tile16_usage[i] = true;
				return i;
			}
		}
		return (uint32_t)-1;


	}
	bool updateTile16(int i, ImageSlice img) {
		if (tile16_usage[i] == false) {
			return false;
		}
		for (int y = 0; y < 16; y++) {
			for (int x = 0; x < 16; x++) {
				tile16_table[i].data[y][x] = img.get(x, y);
			}
		}
		return true;

	}
	bool deleteTile16(int i) {
		if (tile16_usage[i] == false) {
			return false;
		}
		tile16_usage[i] = false;
		for (int j = 0; j < 32; j++) {
			// remove sprites of this tile
			if (sprite_entries[j].is_32 == false && sprite_entries[j].tile_idx == i) {
				sprite_entries[j] = SpriteInfo::getInvalid();
			}
		}
		return true;
	}
	void flush() {
		Xil_DCacheFlushRange((UINTPTR)tile16_table.data(), sizeof(tile16_table));
	}
	void reset() {
		for (int i = 0; i < 128; i++) {
			tile16_usage[i] = false;
		}
	}
};

struct SpriteEngine {

	const unsigned DEVICE_BASE_ADDR = XPAR_SPRITE_CONTROLLER_0_BASEADDR;
	volatile unsigned* 	const contrl_addr = (unsigned*)	(DEVICE_BASE_ADDR + 0);
	volatile unsigned* 	const status_addr = (unsigned*)	(DEVICE_BASE_ADDR + 4);
	volatile unsigned* 	const tile16_addr = (unsigned*)	(DEVICE_BASE_ADDR + 8);
	volatile unsigned*	const tile32_addr = (unsigned*)	(DEVICE_BASE_ADDR + 12);
	volatile unsigned*	const sprite_addr = (unsigned*)	(DEVICE_BASE_ADDR + 16);
	volatile unsigned*	const buffer_addr = (unsigned*) (DEVICE_BASE_ADDR + 20);

	std::array<SpriteInfo, 32> sprite_table = []() {
		std::array<SpriteInfo, 32> res;
		for (int i = 0; i < 32; i++) {
			res[i] = SpriteInfo::getInvalid();
		}
		return res;
	}();

	std::array<bool, 32> sprite_usage {};
	Tile16Table tile16_table {sprite_table.data()};
	Tile32Table tile32_table {sprite_table.data()};

	uint32_t newSprite(SpriteInfo info) {
		uint32_t i;
		for (i = 0; i < 32; i++) {
			if (sprite_usage[i] == false) {
				sprite_table[i] = info;
				sprite_usage[i] = true;
				return i;
			}
		}
		return (uint32_t)-1;
	}
	bool updateSprite(uint32_t idx, SpriteInfo info) {
		if (sprite_usage[idx] == false) {
			return false;
		}
		sprite_table[idx] = info;
		return true;
	}
	bool removeSprite(uint32_t idx) {
		if (sprite_usage[idx] == false) {
			return false;
		}
		sprite_table[idx] = SpriteInfo::getInvalid();
		sprite_usage[idx] = false;
		return true;
	}
	void setup(void* buffer) {
		*tile16_addr = (unsigned)tile16_table.tile16_table.data();
		*tile32_addr = (unsigned)tile32_table.tile32_table.data();
		*sprite_addr = (unsigned)sprite_table.data();
		*buffer_addr = (unsigned)buffer;
	}
	void draw() {
		*contrl_addr = 1;
//		xil_printf("[SPRITE ENGINE] CR: %x, SR: %x\n", *contrl_addr, *status_addr);
//		xil_printf("[SPRITE ENGINE] BA: %x\n", *buffer_addr);
	}
	void flush() {
		Xil_DCacheFlushRange((UINTPTR)sprite_table.data(), sizeof(sprite_table));
	}

	void reset() {
		for (int i = 0; i < 32; i++) {
			sprite_table[i] = SpriteInfo::getInvalid();
			sprite_usage[i] = false;
		}
		tile16_table.reset();
		tile32_table.reset();
	}
};


#endif  // __MICROBLAZE__
#endif /* SRC_PLATFORM_MICROBLAZE_SPRITE_ENGINE_HPP_ */
