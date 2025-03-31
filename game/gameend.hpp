/*
 * gameend.hpp
 *
 *  Created on: Mar 30, 2025
 *      Author: lunar
 */

#ifndef SRC_GAME_GAMEEND_HPP_
#define SRC_GAME_GAMEEND_HPP_

struct GameEnd {


	const std::array<std::pair<int/*x*/, int/*y*/>, 10> text_position = {{
		std::make_pair(256, 180), 			// Finished!
		std::make_pair(288, 260),  			// Time
		std::make_pair(288 + 16 * 5, 260), 	// MM:SS.cc
		std::make_pair(96, 260),   			// Winner is
		std::make_pair(96 + 32 * 10, 260), 	// (Winner is) Player 1/2
		std::make_pair(208, 320),			// Player 1 Time
		std::make_pair(208 + 16 * 14, 320),	// MM:SS.cc
		std::make_pair(208, 350),			// Player 2 Time
		std::make_pair(208 + 16 * 14, 350),	// MM:SS.cc
		std::make_pair(192, 460),			// Press Any Button to Return
	}};

	Controller* controller;
	Image image;
	int update_counter = 0;

	static const char* frames_to_time(int frames) {
		static char buffer[9];
		int MM, SS, cc;  // MM: minute, SS: second, cc: centisecond
		// assuming 60 fps
		int total_cs = frames * 100 / 60;
		MM = total_cs / 6000;
		SS = (total_cs % 6000) / 100;
		cc = (total_cs % 100);
	    buffer[0] = '0' + (MM / 10);
	    buffer[1] = '0' + (MM % 10);
	    buffer[2] = ':';
	    buffer[3] = '0' + (SS / 10);
	    buffer[4] = '0' + (SS % 10);
	    buffer[5] = '.';
	    buffer[6] = '0' + (cc / 10);
	    buffer[7] = '0' + (cc % 10);
	    buffer[8] = '\0';
	    return buffer;
	}

	void init(Controller* con, bool is_multiplayer, int p1_frames, int p2_frames) {
		controller = con;
		update_counter = 0;
		image = Image::fromBlank(800, 600, C_BLACK);
		Draw draw = Draw(image.slice());
		draw.drawText(Font::DEFAULT_FONT32, text_position[0].first, text_position[0].second,
			"Finished!", C_WHITE);
		if (is_multiplayer) {
			const char* winner;
			if (p1_frames < p2_frames) {
				winner = "Player 1";
			} else if (p1_frames > p2_frames) {
				winner = "Player 2";
			} else {
				winner = "...Tie!";
			}
			draw.drawText(Font::DEFAULT_FONT32, text_position[3].first, text_position[3].second,
				"Winner is ", C_WHITE);
			draw.drawText(Font::DEFAULT_FONT32, text_position[4].first, text_position[4].second,
				winner, C_WHITE);
			draw.drawText(Font::DEFAULT_FONT16, text_position[5].first, text_position[5].second,
				"Player 1 Time ", C_WHITE);
			draw.drawText(Font::DEFAULT_FONT16, text_position[6].first, text_position[6].second,
				frames_to_time(p1_frames), C_WHITE);
			draw.drawText(Font::DEFAULT_FONT16, text_position[7].first, text_position[7].second,
				"Player 2 Time", C_WHITE);
			draw.drawText(Font::DEFAULT_FONT16, text_position[8].first, text_position[8].second,
				frames_to_time(p2_frames), C_WHITE);
		} else {
			draw.drawText(Font::DEFAULT_FONT16, text_position[1].first, text_position[1].second,
				"Time ", C_WHITE);
			draw.drawText(Font::DEFAULT_FONT16, text_position[2].first, text_position[2].second,
				frames_to_time(p1_frames), C_WHITE);
		}
		draw.drawText(Font::DEFAULT_FONT16, text_position[9].first, text_position[9].second,
			"Press Any Button to Return", C_WHITE);
	}
	bool update() {
		update_counter++;
		auto btn_event = controller->GetButtonDownEvent();
		if (btn_event && update_counter >= 100) {
			return true;
		}
		return false;
	}
	void draw_static(Draw bg_draw) {
		bg_draw.drawImage(0, 0, image);
		bg_draw.FlushPixels(bg_draw.getSlice());
	}
};



#endif /* SRC_GAME_GAMEEND_HPP_ */
