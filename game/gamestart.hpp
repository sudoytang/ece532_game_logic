/*
 * gamestart.hpp
 *
 *  Created on: Mar 30, 2025
 *      Author: lunar
 */

#ifndef SRC_GAME_GAMESTART_HPP_
#define SRC_GAME_GAMESTART_HPP_

#include "data/title.h"

struct GameStart {
	static constexpr int SEL_X 		= 300;
	static constexpr int SEL_Y		= 458;
	static constexpr int SEL_W		= 200;
	static constexpr int SEL_H		= 42;
	static constexpr int P1_SEL_Y	= SEL_Y;
	static constexpr int P2_SEL_Y 	= P1_SEL_Y + SEL_H;
	static constexpr int OP_SEL_Y	= P2_SEL_Y + SEL_H;
//	static constexpr std::tuple<int/*x*/, int/*y*/, int/*w*/, int/*h*/> p1_selection = {220, 270, 360, 70};
//	static constexpr std::tuple<int/*x*/, int/*y*/, int/*w*/, int/*h*/> p2_selection = {220, 340, 360, 70};
//	static constexpr std::tuple<int/*x*/, int/*y*/, int/*w*/, int/*h*/> op_selection = {465, 530, 130, 130};
	Image main_menu;
	Controller* controller;

	enum Selection {
		S_1PLAYER,
		S_2PLAYER,
		S_OPTION,
		S_END_SELECTION  // don't add new selection below this line!
	};
	Selection selection = S_1PLAYER;
	bool selection_changed = true;
	bool first_draw_dyn = true;

	void init(Controller* con) {
		controller = con;
		if (!main_menu.data) {
			main_menu = Image::fromMemory((void*)img_title_data, 800, 600);
		}
		first_draw_dyn = true;
	}

	void draw_static(Draw bg_draw) {
		bg_draw.drawImage(0, 0, main_menu);
		bg_draw.FlushPixels(bg_draw.getSlice());
	}

	void draw_dynamic(Draw fg_draw) {
		if (!selection_changed && !first_draw_dyn) return;
		first_draw_dyn = false;
		int x, y, w, h;

		switch (selection) {
		case S_1PLAYER:
			x = SEL_X, y = P2_SEL_Y, w = SEL_W, h = SEL_H;
			fg_draw.drawRect(x, y, w, h, C_TRANSPARENT);
			fg_draw.FlushPixels(fg_draw.getSlice().slice(x, y, w, h));
			x = SEL_X, y = OP_SEL_Y, w = SEL_W, h = SEL_H;
			fg_draw.drawRect(x, y, w, h, C_TRANSPARENT);
			fg_draw.FlushPixels(fg_draw.getSlice().slice(x, y, w, h));
			x = SEL_X, y = P1_SEL_Y, w = SEL_W, h = SEL_H;
			fg_draw.drawRect(x, y, w, h, C_WHITE);
			fg_draw.FlushPixels(fg_draw.getSlice().slice(x, y, w, h));
			break;
		case S_2PLAYER:
			x = SEL_X, y = P1_SEL_Y, w = SEL_W, h = SEL_H;
			fg_draw.drawRect(x, y, w, h, C_TRANSPARENT);
			fg_draw.FlushPixels(fg_draw.getSlice().slice(x, y, w, h));
			x = SEL_X, y = OP_SEL_Y, w = SEL_W, h = SEL_H;
			fg_draw.drawRect(x, y, w, h, C_TRANSPARENT);
			fg_draw.FlushPixels(fg_draw.getSlice().slice(x, y, w, h));
			x = SEL_X, y = P2_SEL_Y, w = SEL_W, h = SEL_H;
			fg_draw.drawRect(x, y, w, h, C_WHITE);
			fg_draw.FlushPixels(fg_draw.getSlice().slice(x, y, w, h));
			break;
		case S_OPTION:
			x = SEL_X, y = P1_SEL_Y, w = SEL_W, h = SEL_H;
			fg_draw.drawRect(x, y, w, h, C_TRANSPARENT);
			fg_draw.FlushPixels(fg_draw.getSlice().slice(x, y, w, h));
			x = SEL_X, y = P2_SEL_Y, w = SEL_W, h = SEL_H;
			fg_draw.drawRect(x, y, w, h, C_TRANSPARENT);
			fg_draw.FlushPixels(fg_draw.getSlice().slice(x, y, w, h));
			x = SEL_X, y = OP_SEL_Y, w = SEL_W, h = SEL_H;
			fg_draw.drawRect(x, y, w, h, C_WHITE);
			fg_draw.FlushPixels(fg_draw.getSlice().slice(x, y, w, h));
			break;
		default:
			x = SEL_X, y = P1_SEL_Y, w = SEL_W, h = SEL_H;
			fg_draw.drawRect(x, y, w, h, C_TRANSPARENT);
			fg_draw.FlushPixels(fg_draw.getSlice().slice(x, y, w, h));
			x = SEL_X, y = P2_SEL_Y, w = SEL_W, h = SEL_H;
			fg_draw.drawRect(x, y, w, h, C_TRANSPARENT);
			fg_draw.FlushPixels(fg_draw.getSlice().slice(x, y, w, h));
			x = SEL_X, y = OP_SEL_Y, w = SEL_W, h = SEL_H;
			fg_draw.drawRect(x, y, w, h, C_TRANSPARENT);
			fg_draw.FlushPixels(fg_draw.getSlice().slice(x, y, w, h));
			break;
		}
	}

	// return true means the center button is pressed.
	bool update() {
		auto btn_event = controller->GetButtonDownEvent();
		if (btn_event) {
			xil_printf("Button Event %x\n", btn_event);
		}
		int stat = selection;
		if (btn_event & BTND) {
			stat = (stat + 1) % S_END_SELECTION;
		}
		if (btn_event & BTNU) {
			stat = (stat + S_END_SELECTION - 1) % S_END_SELECTION;
		}
		if (stat != selection) {
			selection = (Selection)stat;
			selection_changed = true;
		} else {
			selection_changed = false;
		}

		if (btn_event & BTNC) {
			return true;
		} else {
			return false;
		}
	}
};

#endif /* SRC_GAME_GAMESTART_HPP_ */
