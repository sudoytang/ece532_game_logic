#ifndef GAME_HPP_
#define GAME_HPP_
#include <tuple>

#include "controller.hpp"
#include "map.hpp"
#include "car.hpp"

#include "gamestart.hpp"
#include "gameoption.hpp"
#include "gameplay.hpp"
#include "gameend.hpp"



struct Game {
    enum MainState {
        START,
		OPTION,
        PLAY,
        END,
    };
    MainState mode;
    GameStart gameStart;
    GameOption gameOption;
    GamePlay gamePlay;
    GameEnd gameEnd;
    Controller controller;
    Map map;
    int total_laps;

    void init() {
    	mode = START;
    	gameStart.init(&controller);
    	map.init(Map::MAP_0);  // default initialization
    	total_laps = 2;
    }
    void draw_static(Draw bg_draw) {
    	if (mode == START) {
    		gameStart.draw_static(bg_draw);
    	} else if (mode == OPTION) {
    		gameOption.draw_static(bg_draw);
    	} else if (mode == PLAY) {
    		gamePlay.draw_static(bg_draw);
    	} else if (mode == END) {
    		gameEnd.draw_static(bg_draw);
    	}
    }

    void draw_dynamic(Draw fg_draw) {
    	if (mode == START) {
    		gameStart.draw_dynamic(fg_draw);
    	} else if (mode == OPTION) {
    		gameOption.draw_dynamic(fg_draw);
    	} else if (mode == PLAY) {
    		gamePlay.draw_dynamic(fg_draw);
    	} else if (mode == END) {
    		// nothing to draw dynamically
    	}
    }

    // returns: if we need a static redraw
    bool update() {
    	// if the switch[7] is on, we return to gamestart state
    	uint16_t switch_event = controller.GetSwitchOnOffEvent().first;
    	if (switch_event & SW_SOFT_RESET) {
    		xil_printf("[SOFT RESET] See you!\n");
    		microblaze_disable_interrupts();
    		((void(*)())0x0)();  // jump to reset vector
    	}
    	if (switch_event & SW_BACK_TO_MAIN) {
    		mode = START;
    		gameStart.init(&controller);
    		return true;
    	}
    	if (switch_event & SW_GYRO0_RESET) {
    		controller.gyro0.reset();
    	}
    	if (switch_event & SW_GYRO0_CALIB) {
    		controller.gyro0.calib();
    	}
    	if (switch_event & SW_GYRO1_RESET) {
    		controller.gyro1.reset();
    	}
    	if (switch_event & SW_GYRO1_CALIB) {
    		controller.gyro1.calib();
    	}


    	if (mode == START) {
    		if (gameStart.update()) {
    			if (gameStart.selection == gameStart.S_OPTION) {
    				mode = OPTION;
    				gameOption.init(&controller, &map, &total_laps);
    				return true;
    			} else if (gameStart.selection == gameStart.S_1PLAYER) {
    				mode = PLAY;
    				gamePlay.init(&controller, &map, false, total_laps);
    				return true;
    			} else if (gameStart.selection == gameStart.S_2PLAYER) {
    				mode = PLAY;
    				gamePlay.init(&controller, &map, true, total_laps);
    				return true;
    			}
    		}
    	} else if (mode == OPTION) {
    		if (gameOption.update()) {
    			if (gameOption.selection == GameOption::S_BACK) {
    				mode = START;
    				gameStart.init(&controller);
    				return true;
    			} else if (gameOption.selection == GameOption::S_MAP0) {
    				map.init(Map::MAP_0);
    				return true;
    			} else if (gameOption.selection == GameOption::S_MAP1) {
    				map.init(Map::MAP_1);
    				return true;
    			} else if (gameOption.selection == GameOption::S_MAP2) {
    				map.init(Map::MAP_2);
    				return true;
    			} else if (gameOption.selection == GameOption::S_LAP_DECR) {
    				if (total_laps == 1) return false;
    				total_laps--;
    				return true;
    			} else if (gameOption.selection == GameOption::S_LAP_INCR) {
    				if (total_laps == 9) return false;
    				total_laps++;
    				return true;
    			} else {
    				auto [id, bd] = gameOption.ctrlbd_mapper[gameOption.selection];
    				controller.setBinding(id, bd);
    				return true;
    			}
    		}
    	} else if (mode == PLAY) {
    		if (gamePlay.update()) {
    			mode = END;
    			gameEnd.init(&controller, gamePlay.cars.has_2nd_car,
					gamePlay.finish_framestamp[0],
					gamePlay.finish_framestamp[1]);
    			return true;
    		}
    		return false;
    	} else if (mode == END) {
    		if (gameEnd.update()) {
    	    	mode = START;
    	    	gameStart.init(&controller);
    	    	return true;
    		}
    		return false;
    	}
    	return false;
    }
};


#endif  // GAME_HPP_
