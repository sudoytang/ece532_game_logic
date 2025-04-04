/*
 * gameoption.hpp
 *
 *  Created on: Mar 30, 2025
 *      Author: lunar
 */

#ifndef SRC_GAME_GAMEOPTION_HPP_
#define SRC_GAME_GAMEOPTION_HPP_


struct GameOption {

	enum Selection {
		S_CAR0_UNASSIGNED,
		S_CAR0_GYRO0,
		S_CAR0_GYRO1,
		S_CAR0_BUTTON,
		S_CAR0_KEYBOARD,
		S_CAR1_UNASSIGNED,
		S_CAR1_GYRO0,
		S_CAR1_GYRO1,
		S_CAR1_BUTTON,
		S_CAR1_KEYBOARD,
		S_MAP0,
		S_MAP1,
		S_MAP2,
		S_LAP_DECR,
		S_LAP_INCR,
		S_BACK,
		S_END_SELECTION
	};
	static constexpr int PADDING_X				= 50;
	static constexpr int PADDING_Y				= 50;
	static constexpr int CTRL_BINDING_TEXT_X 	= PADDING_X;
	static constexpr int CTRL_BINDING_TEXT_Y 	= PADDING_Y;
	static constexpr int CAR0_BINDING_TEXT_X 	= PADDING_X;
	static constexpr int CAR0_BINDING_TEXT_Y 	= CTRL_BINDING_TEXT_Y + 50;
	static constexpr int CAR1_BINDING_TEXT_X 	= CAR0_BINDING_TEXT_X;
	static constexpr int CAR1_BINDING_TEXT_Y 	= CAR0_BINDING_TEXT_Y + 30;
	static constexpr int GYRO0_STAT_BOX_X		= PADDING_X;
	static constexpr int GYRO0_STAT_BOX_Y		= CAR1_BINDING_TEXT_Y + 50;
	static constexpr int GYRO0_STAT_CENTER_X	= GYRO0_STAT_BOX_X + 50;
	static constexpr int GYRO0_STAT_CENTER_Y	= GYRO0_STAT_BOX_Y + 50;
	static constexpr int GYRO1_STAT_BOX_X		= GYRO0_STAT_BOX_X + 120;
	static constexpr int GYRO1_STAT_BOX_Y		= GYRO0_STAT_BOX_Y;
	static constexpr int GYRO1_STAT_CENTER_X	= GYRO1_STAT_BOX_X + 50;
	static constexpr int GYRO1_STAT_CENTER_Y	= GYRO1_STAT_BOX_Y + 50;
	static constexpr int GYRO_CALIB_HINT_X		= GYRO1_STAT_BOX_X + 120;
	static constexpr int GYRO_CALIB_HINT_Y_0	= GYRO0_STAT_BOX_Y;
	static constexpr int GYRO_CALIB_HINT_Y_1	= GYRO_CALIB_HINT_Y_0 + 40;

	static constexpr int MAP_SELECTION_TEXT_X	= PADDING_X;
	static constexpr int MAP_SELECTION_TEXT_Y   = 330;
	static constexpr int MAP0_THUMBNAIL_X		= PADDING_X;
	static constexpr int MAP0_THUMBNAIL_Y		= MAP_SELECTION_TEXT_Y + 50;
	static constexpr int MAP1_THUMBNAIL_X		= MAP0_THUMBNAIL_X + 200;
	static constexpr int MAP1_THUMBNAIL_Y		= MAP0_THUMBNAIL_Y;
	static constexpr int MAP2_THUMBNAIL_X		= MAP1_THUMBNAIL_X + 200;
	static constexpr int MAP2_THUMBNAIL_Y		= MAP0_THUMBNAIL_Y;
	static constexpr int MAP0_NAME_TEXT_X		= MAP0_THUMBNAIL_X + 16;
	static constexpr int MAP0_NAME_TEXT_Y		= MAP0_THUMBNAIL_Y + 130;
	static constexpr int MAP1_NAME_TEXT_X		= MAP1_THUMBNAIL_X + 16;
	static constexpr int MAP1_NAME_TEXT_Y		= MAP1_THUMBNAIL_Y + 130;
	static constexpr int MAP2_NAME_TEXT_X		= MAP2_THUMBNAIL_X + 16;
	static constexpr int MAP2_NAME_TEXT_Y		= MAP2_THUMBNAIL_Y + 130;
	static constexpr int LAP_TEXT_X				= MAP2_THUMBNAIL_X + 200;
	static constexpr int LAP_TEXT_Y				= MAP0_THUMBNAIL_Y;
	static constexpr int LAP_SELECT_TEXT_X		= LAP_TEXT_X;
	static constexpr int LAP_SELECT_TEXT_Y		= LAP_TEXT_Y + 30;
	static constexpr int LAP_NUMBER_TEXT_X		= LAP_SELECT_TEXT_X + 32;
	static constexpr int LAP_NUMBER_TEXT_Y		= LAP_SELECT_TEXT_Y;
	static constexpr int LAP_DECREASE_X			= LAP_SELECT_TEXT_X;
	static constexpr int LAP_DECREASE_Y			= LAP_SELECT_TEXT_Y;
	static constexpr int LAP_INCREASE_X			= LAP_SELECT_TEXT_X + 64;
	static constexpr int LAP_INCREASE_Y			= LAP_SELECT_TEXT_Y;
	static constexpr int BACK_TEXT_X			= 368;
	static constexpr int BACK_TEXT_Y    		= 540;
	const std::array<std::tuple<int/*x*/, int/*y*/, int/*w*/, int/*h*/>, S_END_SELECTION> selection_boxes = {{
		{CAR0_BINDING_TEXT_X + 16 * 7 	- 1, CAR0_BINDING_TEXT_Y - 1, 64 + 2, 16 + 2},
		{CAR0_BINDING_TEXT_X + 16 * 12 	- 1, CAR0_BINDING_TEXT_Y - 1, 64 + 2, 16 + 2},
		{CAR0_BINDING_TEXT_X + 16 * 17 	- 1, CAR0_BINDING_TEXT_Y - 1, 64 + 2, 16 + 2},
		{CAR0_BINDING_TEXT_X + 16 * 22 	- 1, CAR0_BINDING_TEXT_Y - 1, 64 + 2, 16 + 2},
		{CAR0_BINDING_TEXT_X + 16 * 27 	- 1, CAR0_BINDING_TEXT_Y - 1, 64 + 2, 16 + 2},
		{CAR1_BINDING_TEXT_X + 16 * 7 	- 1, CAR1_BINDING_TEXT_Y - 1, 64 + 2, 16 + 2},
		{CAR1_BINDING_TEXT_X + 16 * 12 	- 1, CAR1_BINDING_TEXT_Y - 1, 64 + 2, 16 + 2},
		{CAR1_BINDING_TEXT_X + 16 * 17 	- 1, CAR1_BINDING_TEXT_Y - 1, 64 + 2, 16 + 2},
		{CAR1_BINDING_TEXT_X + 16 * 22 	- 1, CAR1_BINDING_TEXT_Y - 1, 64 + 2, 16 + 2},
		{CAR1_BINDING_TEXT_X + 16 * 27 	- 1, CAR1_BINDING_TEXT_Y - 1, 64 + 2, 16 + 2},
		{MAP0_THUMBNAIL_X - 1,				 MAP0_THUMBNAIL_Y - 1	, 160 + 2, 120 + 10 + 16 + 2},
		{MAP1_THUMBNAIL_X - 1,				 MAP1_THUMBNAIL_Y - 1	, 160 + 2, 120 + 10 + 16 + 2},
		{MAP2_THUMBNAIL_X - 1,				 MAP2_THUMBNAIL_Y - 1	, 160 + 2, 120 + 10 + 16 + 2},
		{LAP_DECREASE_X - 1,				 LAP_DECREASE_Y - 1		, 16 + 2, 16 + 2},
		{LAP_INCREASE_X - 1,				 LAP_INCREASE_Y - 1		, 16 + 2, 16 + 2},
		{BACK_TEXT_X - 1, 					 BACK_TEXT_Y - 1		, 64 + 2, 16 + 2},
	}};

	const std::array<std::pair<int/*id*/, Controller::ControllerBinding>, S_END_SELECTION> ctrlbd_mapper = {{
		{0, Controller::BIND_UNASSIGNED},
		{0, Controller::BIND_GYRO0},
		{0, Controller::BIND_GYRO1},
		{0, Controller::BIND_BUTTON},
		{0, Controller::BIND_KEYBOARD},
		{1, Controller::BIND_UNASSIGNED},
		{1, Controller::BIND_GYRO0},
		{1, Controller::BIND_GYRO1},
		{1, Controller::BIND_BUTTON},
		{1, Controller::BIND_KEYBOARD},
	}};
	Selection selection;
	Controller* controller;
	const MapData* map_datas;
	const Map::MapSelection* map_selection;
	bool selection_changed = true;
	bool first_draw_static = true;
	bool first_draw_dyn = true;
	int* lap_number;
	void init(Controller* con, Map* p_map, int* laps) {
		controller = con;
		map_datas = p_map->map_data.data();
		map_selection = &p_map->sel;
		selection = S_BACK;
		first_draw_dyn = true;
		first_draw_static = true;
		lap_number = laps;
	}
	void draw_static(Draw draw_bg) {
		xil_printf("Draw Static Option Page!\n");
		if (first_draw_static) {
			draw_bg.setBlank(C_BLACK);
			first_draw_static = false;
		}
		draw_bg.drawText(Font::DEFAULT_FONT32, CTRL_BINDING_TEXT_X, CTRL_BINDING_TEXT_Y,
			"Controller Binding:", C_WHITE);
		draw_bg.drawText(Font::DEFAULT_FONT16, CAR0_BINDING_TEXT_X, CAR0_BINDING_TEXT_Y,
			"Car 0:  N/A  GY0  GY1  BTN  KBD", C_WHITE);
		draw_bg.drawText(Font::DEFAULT_FONT16, CAR1_BINDING_TEXT_X, CAR1_BINDING_TEXT_Y,
			"Car 1:  N/A  GY0  GY1  BTN  KBD", C_WHITE);

		draw_bg.drawRect(GYRO0_STAT_BOX_X, GYRO0_STAT_BOX_Y, 100, 100, C_WHITE);
		draw_bg.drawRect(GYRO1_STAT_BOX_X, GYRO1_STAT_BOX_Y, 100, 100, C_WHITE);
		draw_bg.drawText(Font::DEFAULT_FONT16, GYRO0_STAT_BOX_X, GYRO0_STAT_BOX_Y,
			"GY0", C_WHITE);
		draw_bg.drawText(Font::DEFAULT_FONT16, GYRO1_STAT_BOX_X, GYRO1_STAT_BOX_Y,
			"GY1", C_WHITE);
		draw_bg.drawText(Font::DEFAULT_FONT16, GYRO_CALIB_HINT_X, GYRO_CALIB_HINT_Y_0,
			"Flip SW0 to ON to reset GY0", C_WHITE);
		draw_bg.drawText(Font::DEFAULT_FONT16, GYRO_CALIB_HINT_X, GYRO_CALIB_HINT_Y_1,
			"Flip SW15 to ON to reset GY1", C_WHITE);


		draw_bg.drawText(Font::DEFAULT_FONT32, MAP_SELECTION_TEXT_X, MAP_SELECTION_TEXT_Y,
			"Track Selection:", C_WHITE);
		draw_bg.drawImage(MAP0_THUMBNAIL_X, MAP0_THUMBNAIL_Y,
			ImageView::fromMemory((void*)map_datas[0].map_image_src, 800, 600).downscale(5));
		draw_bg.drawImage(MAP1_THUMBNAIL_X, MAP1_THUMBNAIL_Y,
			ImageView::fromMemory((void*)map_datas[1].map_image_src, 800, 600).downscale(5));
		draw_bg.drawImage(MAP2_THUMBNAIL_X, MAP2_THUMBNAIL_Y,
			ImageView::fromMemory((void*)map_datas[2].map_image_src, 800, 600).downscale(5));

		draw_bg.drawText(Font::DEFAULT_FONT16, MAP0_NAME_TEXT_X, MAP0_NAME_TEXT_Y,
			map_datas[0].map_name, C_WHITE);
		draw_bg.drawText(Font::DEFAULT_FONT16, MAP1_NAME_TEXT_X, MAP1_NAME_TEXT_Y,
			map_datas[1].map_name, C_WHITE);
		draw_bg.drawText(Font::DEFAULT_FONT16, MAP2_NAME_TEXT_X, MAP2_NAME_TEXT_Y,
			map_datas[2].map_name, C_WHITE);
		draw_bg.drawText(Font::DEFAULT_FONT16, LAP_TEXT_X, LAP_TEXT_Y,
			"LAPS:", C_WHITE);
		draw_bg.drawText(Font::DEFAULT_FONT16, LAP_SELECT_TEXT_X, LAP_SELECT_TEXT_Y,
			"-   +", C_WHITE);
		char buf[2];
		buf[1] = 0;
		buf[0] = '0' + *lap_number;
		xil_printf("lap_number = %d\n", *lap_number);
		draw_bg.drawText(Font::DEFAULT_FONT16, LAP_NUMBER_TEXT_X, LAP_NUMBER_TEXT_Y,
			buf, C_WHITE);
		draw_bg.drawText(Font::DEFAULT_FONT16, BACK_TEXT_X, BACK_TEXT_Y,
			"BACK", C_WHITE);
		auto bd0 = controller->bindings[0];
		auto bd1 = controller->bindings[1];
		auto getTextOffset = [](Controller::ControllerBinding bd) -> int {
			switch (bd) {
			case Controller::BIND_UNASSIGNED:
				return 7;
			case Controller::BIND_GYRO0:
				return 12;
			case Controller::BIND_GYRO1:
				return 17;
			case Controller::BIND_BUTTON:
				return 22;
			case Controller::BIND_KEYBOARD:
				return 27;
			default:
				return 0;
			}
		};
		if (*map_selection == Map::MAP_0) {
			draw_bg.drawText(Font::DEFAULT_FONT16, MAP0_NAME_TEXT_X - 16, MAP0_NAME_TEXT_Y, ">", C_WHITE);
			draw_bg.drawText(Font::DEFAULT_FONT16, MAP1_NAME_TEXT_X - 16, MAP1_NAME_TEXT_Y, " ", C_WHITE);
			draw_bg.drawText(Font::DEFAULT_FONT16, MAP2_NAME_TEXT_X - 16, MAP2_NAME_TEXT_Y, " ", C_WHITE);
		} else if (*map_selection == Map::MAP_1) {
			draw_bg.drawText(Font::DEFAULT_FONT16, MAP0_NAME_TEXT_X - 16, MAP0_NAME_TEXT_Y, " ", C_WHITE);
			draw_bg.drawText(Font::DEFAULT_FONT16, MAP1_NAME_TEXT_X - 16, MAP1_NAME_TEXT_Y, ">", C_WHITE);
			draw_bg.drawText(Font::DEFAULT_FONT16, MAP2_NAME_TEXT_X - 16, MAP2_NAME_TEXT_Y, " ", C_WHITE);
		} else if (*map_selection == Map::MAP_2) {
			draw_bg.drawText(Font::DEFAULT_FONT16, MAP0_NAME_TEXT_X - 16, MAP0_NAME_TEXT_Y, " ", C_WHITE);
			draw_bg.drawText(Font::DEFAULT_FONT16, MAP1_NAME_TEXT_X - 16, MAP1_NAME_TEXT_Y, " ", C_WHITE);
			draw_bg.drawText(Font::DEFAULT_FONT16, MAP2_NAME_TEXT_X - 16, MAP2_NAME_TEXT_Y, ">", C_WHITE);
		} else {
			xil_printf("Invalid Map Selection: 0x%x\n", *map_selection);
		}

		int bd_offset0 = getTextOffset(bd0);
		if (bd_offset0 != 0) {
			draw_bg.drawText(Font::DEFAULT_FONT16, CAR0_BINDING_TEXT_X + 16 * bd_offset0, CAR0_BINDING_TEXT_Y, ">", C_WHITE);
		} else {
			xil_printf("Invalid Controller Binding: 0x%x\n", bd0);
		}
		int bd_offset1 = getTextOffset(bd1);
		if (bd_offset0 != 0) {
			draw_bg.drawText(Font::DEFAULT_FONT16, CAR1_BINDING_TEXT_X + 16 * bd_offset1, CAR1_BINDING_TEXT_Y, ">", C_WHITE);
		} else {
			xil_printf("Invalid Controller Binding: 0x%x\n", bd1);
		}
		draw_bg.FlushPixels(draw_bg.getSlice());
	}

	void draw_dynamic(Draw draw_fg) {
		static int last_draw_x0;
		static int last_draw_y0;
		static int last_draw_x1;
		static int last_draw_y1;
		auto normalize = [](short val) -> short {
			if (val > 950) {
				val = 950;
			} else if (val < -950) {
				val = -950;
			}
			return val / 20;
		};
		short x, y, z;
		int draw_x, draw_y;
		controller->gyro0.read(x, y, z);

		draw_x = GYRO0_STAT_CENTER_X + normalize(x) - 1;
		draw_y = GYRO0_STAT_CENTER_Y + normalize(y) - 1;
		draw_fg.drawRect(last_draw_x0, last_draw_y0, 3, 3, C_TRANSPARENT, true);
		draw_fg.drawRect(draw_x, draw_y, 3, 3, C_WHITE, true);
		draw_fg.FlushPixels(draw_fg.getSlice().slice(GYRO0_STAT_BOX_X, GYRO0_STAT_BOX_Y, 100, 100));
		last_draw_x0 = draw_x;
		last_draw_y0 = draw_y;

		controller->gyro1.read(x, y, z);
		x = -x;
		y = -y;
		draw_x = GYRO1_STAT_CENTER_X + normalize(x) - 1;
		draw_y = GYRO1_STAT_CENTER_Y + normalize(y) - 1;
		draw_fg.drawRect(last_draw_x1, last_draw_y1, 3, 3, C_TRANSPARENT, true);
		draw_fg.drawRect(draw_x, draw_y, 3, 3, C_WHITE, true);
		draw_fg.FlushPixels(draw_fg.getSlice().slice(GYRO1_STAT_BOX_X, GYRO1_STAT_BOX_Y, 100, 100));
		last_draw_x1 = draw_x;
		last_draw_y1 = draw_y;

		if (!selection_changed && !first_draw_dyn) return;
		first_draw_dyn = false;
		xil_printf("Option Dynamic Redraw\n");
		for (int i = 0; i < S_END_SELECTION; i++) {
			auto [x, y, w, h] = selection_boxes[i];
			if (i == selection) {
				draw_fg.drawRect(x, y, w, h, C_WHITE);
			} else {
				draw_fg.drawRect(x, y, w, h, C_TRANSPARENT);
			}
			draw_fg.FlushPixels(draw_fg.getSlice().slice(x, y, w, h + 1));
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
			if (stat == S_BACK) {
				// do nothing
			} else if (stat >= S_MAP0 && stat <= S_LAP_INCR) {
				stat = S_BACK;
			} else if (stat >= S_CAR1_UNASSIGNED && stat <= S_CAR1_GYRO0) {
				stat = S_MAP0;
			} else if (stat >= S_CAR1_GYRO1 && stat <= S_CAR1_KEYBOARD) {
				stat = S_MAP1;
			} else if (stat >= S_CAR0_UNASSIGNED && stat <= S_CAR0_KEYBOARD) {
				stat = stat + S_CAR1_UNASSIGNED - S_CAR0_UNASSIGNED;
			} else {
				stat = S_BACK;  // default
			}
		}
		if (btn_event & BTNU) {
			if (stat == S_BACK) {
				stat = S_MAP1;
			} else if (stat == S_MAP1) {
				stat = S_CAR1_GYRO1;
			} else if (stat == S_MAP0) {
				stat = S_CAR1_UNASSIGNED;
			} else if (stat >= S_MAP2 && stat <= S_LAP_INCR) {
				stat = S_CAR1_KEYBOARD;
			} else if (stat >= S_CAR1_UNASSIGNED && stat <= S_CAR1_KEYBOARD) {
				stat = stat + S_CAR0_UNASSIGNED - S_CAR1_UNASSIGNED;
			} else if (stat >= S_CAR0_UNASSIGNED && stat <= S_CAR0_KEYBOARD) {
				// do nothing
			} else {
				stat = S_BACK;  // default
			}
		}
		if (btn_event & BTNL) {
			stat = (stat + S_END_SELECTION - 1) % S_END_SELECTION;
		}
		if (btn_event & BTNR) {
			stat = (stat + 1) % S_END_SELECTION;
		}
		if (stat != selection) {
			selection = (Selection)stat;
			selection_changed = true;
		} else {
			selection_changed = false;
		}

		if (btn_event & BTNC) {
			first_draw_dyn = true;
			// return true means a static redraw is (probably) needed
			return true;
		} else {
			return false;
		}
	}
};




#endif /* SRC_GAME_GAMEOPTION_HPP_ */
